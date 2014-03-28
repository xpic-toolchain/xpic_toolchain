/* BFD back-end for convertions from elf32-xpic to ARM assembler sections file
 */


#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "libiberty.h"
#include "safe-ctype.h"


/* Macros for converting between hex and binary.  */

static const char digs[] = "0123456789ABCDEF";

#define NIBBLE(x)    hex_value(x)
#define HEX(buffer) ((NIBBLE ((buffer)[0])<<4) + NIBBLE ((buffer)[1]))
#define TOHEX(d, x, ch) \
	d[1] = digs[(x) & 0xf]; \
	d[0] = digs[((x)>>4)&0xf]; \
	ch += ((x) & 0xff);
#define	ISHEX(x)    hex_p(x)

/* The maximum number of address+data+crc bytes on a line is FF.  */
#define MAXCHUNK_X 0xff

/* Default size for a CHUNK.  */
#define DEFAULT_CHUNK_X 16

/* The number of data bytes we actually fit onto a line on output.
   This variable can be modified by objcopy's --srec-len parameter.
   For a 0x75 byte record you should set --srec-len=0x70.  */
unsigned int Chunk_x = DEFAULT_CHUNK_X;

/* The type of srec output (free or forced to S3).
   This variable can be modified by objcopy's --srec-forceS3
   parameter.  */
bfd_boolean S3Forced_x = FALSE;

/* When writing an S-record file, the S-records can not be output as
   they are seen.  This structure is used to hold them in memory.  */

struct ldrxpic_data_list_struct
{
  struct ldrxpic_data_list_struct *next;
  bfd_byte *data;
  bfd_vma where;
  bfd_size_type size;
  sec_ptr section;
};


typedef struct ldrxpic_data_list_struct ldrxpic_data_list_type;

/* When scanning the S-record file, a linked list of srec_symbol
   structures is built to represent the symbol table (if there is
   one).  */

struct ldrxpic_symbol
{
  struct ldrxpic_symbol *next;
  const char *name;
  bfd_vma val;
};

/* The S-record tdata information.  */

typedef struct srec_data_struct
  {
    ldrxpic_data_list_type *head;
    ldrxpic_data_list_type *tail;
    unsigned int type;
    struct ldrxpic_symbol *symbols;
    struct ldrxpic_symbol *symtail;
    asymbol *csymbols;
  }
tdata_type;

/* Initialize by filling in the hex conversion array.  */

static void
ldrxpic_init (void)
{
  static bfd_boolean inited = FALSE;

  if (! inited)
    {
      inited = TRUE;
      hex_init ();
    }
}

void ldrxpic_major_set(unsigned short set_val);
void ldrxpic_minor_set(unsigned short set_val);

/* Set up the S-record tdata information.  */

static bfd_boolean
ldrxpic_mkobject (bfd *abfd)
{
  tdata_type *tdata;

  ldrxpic_init ();

  tdata = (tdata_type *) bfd_alloc (abfd, sizeof (tdata_type));
  if (tdata == NULL)
    return FALSE;

  abfd->tdata.srec_data = tdata;
  tdata->type = 1;
  tdata->head = NULL;
  tdata->tail = NULL;
  tdata->symbols = NULL;
  tdata->symtail = NULL;
  tdata->csymbols = NULL;

  return TRUE;
}

/* Read a byte from an S record file.  Set *ERRORPTR if an error
   occurred.  Return EOF on error or end of file.  */

static int
ldrxpic_get_byte (bfd *abfd, bfd_boolean *errorptr)
{
  bfd_byte c;

  if (bfd_bread (&c, (bfd_size_type) 1, abfd) != 1)
    {
      if (bfd_get_error () != bfd_error_file_truncated)
	*errorptr = TRUE;
      return EOF;
    }

  return (int) (c & 0xff);
}

/* Report a problem in an S record file.  FIXME: This probably should
   not call fprintf, but we really do need some mechanism for printing
   error messages.  */

static void
ldrxpic_bad_byte (bfd *abfd,
	       unsigned int lineno,
	       int c,
	       bfd_boolean error)
{
  if (c == EOF)
    {
      if (! error)
	bfd_set_error (bfd_error_file_truncated);
    }
  else
    {
      char buf[10];

      if (! ISPRINT (c))
	sprintf (buf, "\\%03o", (unsigned int) c);
      else
	{
	  buf[0] = c;
	  buf[1] = '\0';
	}
      (*_bfd_error_handler)
	(_("%B:%d: Unexpected character `%s' in S-record file\n"),
	 abfd, lineno, buf);
      bfd_set_error (bfd_error_bad_value);
    }
}

/* Add a new symbol found in an S-record file.  */

static bfd_boolean
ldrxpic_new_symbol (bfd *abfd, const char *name, bfd_vma val)
{
  struct ldrxpic_symbol *n;

  n = (struct ldrxpic_symbol *) bfd_alloc (abfd, sizeof (* n));
  if (n == NULL)
    return FALSE;

  n->name = name;
  n->val = val;

  if (abfd->tdata.srec_data->symbols == NULL)
    abfd->tdata.srec_data->symbols = n;
  else
    abfd->tdata.srec_data->symtail->next = n;
  abfd->tdata.srec_data->symtail = n;
  n->next = NULL;

  ++abfd->symcount;

  return TRUE;
}

/* Read the S record file and turn it into sections.  We create a new
   section for each contiguous set of bytes.  */

static bfd_boolean
ldrxpic_scan (bfd *abfd)
{
  int c;
  unsigned int lineno = 1;
  bfd_boolean error = FALSE;
  bfd_byte *buf = NULL;
  size_t bufsize = 0;
  asection *sec = NULL;
  char *symbuf = NULL;

  if (bfd_seek (abfd, (file_ptr) 0, SEEK_SET) != 0)
    goto error_return;

  while ((c = ldrxpic_get_byte (abfd, &error)) != EOF)
    {
      /* We only build sections from contiguous S-records, so if this
	 is not an S-record, then stop building a section.  */
      if (c != 'S' && c != '\r' && c != '\n')
	sec = NULL;

      switch (c)
	{
	default:
	  ldrxpic_bad_byte (abfd, lineno, c, error);
	  goto error_return;

	case '\n':
	  ++lineno;
	  break;

	case '\r':
	  break;

	case '$':
	  /* Starting a module name, which we ignore.  */
	  while ((c = ldrxpic_get_byte (abfd, &error)) != '\n'
		 && c != EOF)
	    ;
	  if (c == EOF)
	    {
	      ldrxpic_bad_byte (abfd, lineno, c, error);
	      goto error_return;
	    }

	  ++lineno;
	  break;

	case ' ':
	  do
	    {
	      bfd_size_type alc;
	      char *p, *symname;
	      bfd_vma symval;

	      /* Starting a symbol definition.  */
	      while ((c = ldrxpic_get_byte (abfd, &error)) != EOF
		     && (c == ' ' || c == '\t'))
		;

	      if (c == '\n' || c == '\r')
		break;

	      if (c == EOF)
		{
		  ldrxpic_bad_byte (abfd, lineno, c, error);
		  goto error_return;
		}

	      alc = 10;
	      symbuf = (char *) bfd_malloc (alc + 1);
	      if (symbuf == NULL)
		goto error_return;

	      p = symbuf;

	      *p++ = c;
	      while ((c = ldrxpic_get_byte (abfd, &error)) != EOF
		     && ! ISSPACE (c))
		{
		  if ((bfd_size_type) (p - symbuf) >= alc)
		    {
		      char *n;

		      alc *= 2;
		      n = (char *) bfd_realloc (symbuf, alc + 1);
		      if (n == NULL)
			goto error_return;
		      p = n + (p - symbuf);
		      symbuf = n;
		    }

		  *p++ = c;
		}

	      if (c == EOF)
		{
		  ldrxpic_bad_byte (abfd, lineno, c, error);
		  goto error_return;
		}

	      *p++ = '\0';
	      symname = (char *) bfd_alloc (abfd, (bfd_size_type) (p - symbuf));
	      if (symname == NULL)
		goto error_return;
	      strcpy (symname, symbuf);
	      free (symbuf);
	      symbuf = NULL;

	      while ((c = ldrxpic_get_byte (abfd, &error)) != EOF
		     && (c == ' ' || c == '\t'))
		;
	      if (c == EOF)
		{
		  ldrxpic_bad_byte (abfd, lineno, c, error);
		  goto error_return;
		}

	      /* Skip a dollar sign before the hex value.  */
	      if (c == '$')
		{
		  c = ldrxpic_get_byte (abfd, &error);
		  if (c == EOF)
		    {
		      ldrxpic_bad_byte (abfd, lineno, c, error);
		      goto error_return;
		    }
		}

	      symval = 0;
	      while (ISHEX (c))
		{
		  symval <<= 4;
		  symval += NIBBLE (c);
		  c = ldrxpic_get_byte (abfd, &error);
		  if (c == EOF)
		    {
		      ldrxpic_bad_byte (abfd, lineno, c, error);
		      goto error_return;
		    }
		}

	      if (! ldrxpic_new_symbol (abfd, symname, symval))
		goto error_return;
	    }
	  while (c == ' ' || c == '\t')
	    ;

	  if (c == '\n')
	    ++lineno;
	  else if (c != '\r')
	    {
	      ldrxpic_bad_byte (abfd, lineno, c, error);
	      goto error_return;
	    }

	  break;

	case 'S':
	  {
	    file_ptr pos;
	    char hdr[3];
	    unsigned int bytes;
	    bfd_vma address;
	    bfd_byte *data;
	    unsigned char check_sum;

	    /* Starting an S-record.  */

	    pos = bfd_tell (abfd) - 1;

	    if (bfd_bread (hdr, (bfd_size_type) 3, abfd) != 3)
	      goto error_return;

	    if (! ISHEX (hdr[1]) || ! ISHEX (hdr[2]))
	      {
		if (! ISHEX (hdr[1]))
		  c = hdr[1];
		else
		  c = hdr[2];
		ldrxpic_bad_byte (abfd, lineno, c, error);
		goto error_return;
	      }

	    check_sum = bytes = HEX (hdr + 1);
	    if (bytes * 2 > bufsize)
	      {
		if (buf != NULL)
		  free (buf);
		buf = (bfd_byte *) bfd_malloc ((bfd_size_type) bytes * 2);
		if (buf == NULL)
		  goto error_return;
		bufsize = bytes * 2;
	      }

	    if (bfd_bread (buf, (bfd_size_type) bytes * 2, abfd) != bytes * 2)
	      goto error_return;

	    /* Ignore the checksum byte.  */
	    --bytes;

	    address = 0;
	    data = buf;
	    switch (hdr[0])
	      {
	      case '0':
	      case '5':
		/* Prologue--ignore the file name, but stop building a
		   section at this point.  */
		sec = NULL;
		break;

	      case '3':
		check_sum += HEX (data);
		address = HEX (data);
		data += 2;
		--bytes;
		/* Fall through.  */
	      case '2':
		check_sum += HEX (data);
		address = (address << 8) | HEX (data);
		data += 2;
		--bytes;
		/* Fall through.  */
	      case '1':
		check_sum += HEX (data);
		address = (address << 8) | HEX (data);
		data += 2;
		check_sum += HEX (data);
		address = (address << 8) | HEX (data);
		data += 2;
		bytes -= 2;

		if (sec != NULL
		    && sec->vma + sec->size == address)
		  {
		    /* This data goes at the end of the section we are
		       currently building.  */
		    sec->size += bytes;
		  }
		else
		  {
		    char secbuf[20];
		    char *secname;
		    bfd_size_type amt;
		    flagword flags;

		    sprintf (secbuf, ".sec%d", bfd_count_sections (abfd) + 1);
		    amt = strlen (secbuf) + 1;
		    secname = (char *) bfd_alloc (abfd, amt);
		    strcpy (secname, secbuf);
		    flags = SEC_HAS_CONTENTS | SEC_LOAD | SEC_ALLOC;
		    sec = bfd_make_section_with_flags (abfd, secname, flags);
		    if (sec == NULL)
		      goto error_return;
		    sec->vma = address;
		    sec->lma = address;
		    sec->size = bytes;
		    sec->filepos = pos;
		  }

		while (bytes > 0)
		  {
		    check_sum += HEX (data);
		    data += 2;
		    bytes--;
		  }
		check_sum = 255 - (check_sum & 0xff);
		if (check_sum != HEX (data))
		  {
		    (*_bfd_error_handler)
		      (_("%B:%d: Bad checksum in S-record file\n"),
		       abfd, lineno);
		    bfd_set_error (bfd_error_bad_value);
		    goto error_return;
		  }

		break;

	      case '7':
		check_sum += HEX (data);
		address = HEX (data);
		data += 2;
		/* Fall through.  */
	      case '8':
		check_sum += HEX (data);
		address = (address << 8) | HEX (data);
		data += 2;
		/* Fall through.  */
	      case '9':
		check_sum += HEX (data);
		address = (address << 8) | HEX (data);
		data += 2;
		check_sum += HEX (data);
		address = (address << 8) | HEX (data);
		data += 2;

		/* This is a termination record.  */
		abfd->start_address = address;

		check_sum = 255 - (check_sum & 0xff);
		if (check_sum != HEX (data))
		  {
		    (*_bfd_error_handler)
		      (_("%B:%d: Bad checksum in S-record file\n"),
		       abfd, lineno);
		    bfd_set_error (bfd_error_bad_value);
		    goto error_return;
		  }

		if (buf != NULL)
		  free (buf);

		return TRUE;
	      }
	  }
	  break;
	}
    }

  if (error)
    goto error_return;

  if (buf != NULL)
    free (buf);

  return TRUE;

 error_return:
  if (symbuf != NULL)
    free (symbuf);
  if (buf != NULL)
    free (buf);
  return FALSE;
}

/* Check whether an existing file is an S-record file.  */

static const bfd_target *
ldrxpic_object_p (bfd *abfd)
{
  void * tdata_save;
  bfd_byte b[4];

  ldrxpic_init ();

  if (bfd_seek (abfd, (file_ptr) 0, SEEK_SET) != 0
      || bfd_bread (b, (bfd_size_type) 4, abfd) != 4)
    return NULL;

  if (b[0] != 'S' || !ISHEX (b[1]) || !ISHEX (b[2]) || !ISHEX (b[3]))
    {
      bfd_set_error (bfd_error_wrong_format);
      return NULL;
    }

  tdata_save = abfd->tdata.any;
  if (! ldrxpic_mkobject (abfd) || ! ldrxpic_scan (abfd))
    {
      if (abfd->tdata.any != tdata_save && abfd->tdata.any != NULL)
	bfd_release (abfd, abfd->tdata.any);
      abfd->tdata.any = tdata_save;
      return NULL;
    }

  if (abfd->symcount > 0)
    abfd->flags |= HAS_SYMS;

  return abfd->xvec;
}

/* Check whether an existing file is an S-record file with symbols.  */

static const bfd_target *
symbolldrxpic_object_p (bfd *abfd)
{
  void * tdata_save;
  char b[2];

  ldrxpic_init ();

  if (bfd_seek (abfd, (file_ptr) 0, SEEK_SET) != 0
      || bfd_bread (b, (bfd_size_type) 2, abfd) != 2)
    return NULL;

  if (b[0] != '$' || b[1] != '$')
    {
      bfd_set_error (bfd_error_wrong_format);
      return NULL;
    }

  tdata_save = abfd->tdata.any;
  if (! ldrxpic_mkobject (abfd) || ! ldrxpic_scan (abfd))
    {
      if (abfd->tdata.any != tdata_save && abfd->tdata.any != NULL)
	bfd_release (abfd, abfd->tdata.any);
      abfd->tdata.any = tdata_save;
      return NULL;
    }

  if (abfd->symcount > 0)
    abfd->flags |= HAS_SYMS;

  return abfd->xvec;
}

/* Read in the contents of a section in an S-record file.  */

static bfd_boolean
ldrxpic_read_section (bfd *abfd, asection *section, bfd_byte *contents)
{
  int c;
  bfd_size_type sofar = 0;
  bfd_boolean error = FALSE;
  bfd_byte *buf = NULL;
  size_t bufsize = 0;

  if (bfd_seek (abfd, section->filepos, SEEK_SET) != 0)
    goto error_return;

  while ((c = ldrxpic_get_byte (abfd, &error)) != EOF)
    {
      bfd_byte hdr[3];
      unsigned int bytes;
      bfd_vma address;
      bfd_byte *data;

      if (c == '\r' || c == '\n')
	continue;

      /* This is called after ldrxpic_scan has already been called, so we
	 ought to know the exact format.  */
      BFD_ASSERT (c == 'S');

      if (bfd_bread (hdr, (bfd_size_type) 3, abfd) != 3)
	goto error_return;

      BFD_ASSERT (ISHEX (hdr[1]) && ISHEX (hdr[2]));

      bytes = HEX (hdr + 1);

      if (bytes * 2 > bufsize)
	{
	  if (buf != NULL)
	    free (buf);
	  buf = (bfd_byte *) bfd_malloc ((bfd_size_type) bytes * 2);
	  if (buf == NULL)
	    goto error_return;
	  bufsize = bytes * 2;
	}

      if (bfd_bread (buf, (bfd_size_type) bytes * 2, abfd) != bytes * 2)
	goto error_return;

      address = 0;
      data = buf;
      switch (hdr[0])
	{
	default:
	  BFD_ASSERT (sofar == section->size);
	  if (buf != NULL)
	    free (buf);
	  return TRUE;

	case '3':
	  address = HEX (data);
	  data += 2;
	  --bytes;
	  /* Fall through.  */
	case '2':
	  address = (address << 8) | HEX (data);
	  data += 2;
	  --bytes;
	  /* Fall through.  */
	case '1':
	  address = (address << 8) | HEX (data);
	  data += 2;
	  address = (address << 8) | HEX (data);
	  data += 2;
	  bytes -= 2;

	  if (address != section->vma + sofar)
	    {
	      /* We've come to the end of this section.  */
	      BFD_ASSERT (sofar == section->size);
	      if (buf != NULL)
		free (buf);
	      return TRUE;
	    }

	  /* Don't consider checksum.  */
	  --bytes;

	  while (bytes-- != 0)
	    {
	      contents[sofar] = HEX (data);
	      data += 2;
	      ++sofar;
	    }

	  break;
	}
    }

  if (error)
    goto error_return;

  BFD_ASSERT (sofar == section->size);

  if (buf != NULL)
    free (buf);

  return TRUE;

 error_return:
  if (buf != NULL)
    free (buf);
  return FALSE;
}

/* Get the contents of a section in an S-record file.  */

static bfd_boolean
ldrxpic_get_section_contents (bfd *abfd,
			   asection *section,
			   void * location,
			   file_ptr offset,
			   bfd_size_type count)
{
  if (count == 0)
    return TRUE;

  if (offset + count < count
      || offset + count > section->size)
    {
      bfd_set_error (bfd_error_invalid_operation);
      return FALSE;
    }

  if (section->used_by_bfd == NULL)
    {
      section->used_by_bfd = bfd_alloc (abfd, section->size);
      if (section->used_by_bfd == NULL)
	return FALSE;

      if (! ldrxpic_read_section (abfd, section,
                               (bfd_byte *) section->used_by_bfd))
	return FALSE;
    }

  memcpy (location, (bfd_byte *) section->used_by_bfd + offset,
	  (size_t) count);

  return TRUE;
}

/* Set the architecture.  We accept an unknown architecture here.  */

static bfd_boolean
ldrxpic_set_arch_mach (bfd *abfd, enum bfd_architecture arch, unsigned long mach)
{
  if (arch != bfd_arch_unknown)
    return bfd_default_set_arch_mach (abfd, arch, mach);

  abfd->arch_info = & bfd_default_arch_struct;
  return TRUE;
}






/* We have to save up all the Srecords for a splurge before output.  */

static bfd_boolean
ldrxpic_set_section_contents (bfd *abfd,
			   sec_ptr section,
			   const void * location,
			   file_ptr offset,
			   bfd_size_type bytes_to_do)
{
  tdata_type *tdata = abfd->tdata.srec_data;
  ldrxpic_data_list_type *entry;



  if (bytes_to_do
      && (section->flags & SEC_ALLOC)
      && (section->flags & SEC_LOAD))
    {
      bfd_byte *data;

  entry = (ldrxpic_data_list_type *) bfd_alloc (abfd, sizeof (* entry));
  if (entry == NULL)
    return FALSE;

      data = (bfd_byte *) bfd_alloc (abfd, bytes_to_do);
      if (data == NULL)
	return FALSE;
      memcpy ((void *) data, location, (size_t) bytes_to_do);

      /* Ff S3Forced_x is TRUE then always select S3 records,
	 regardless of the siez of the addresses.  */
#if 0
      if (S3Forced_x)
	tdata->type = 3;
      else if ((section->lma + offset + bytes_to_do - 1) <= 0xffff)
	;  /* The default, S1, is OK.  */
      else if ((section->lma + offset + bytes_to_do - 1) <= 0xffffff
	       && tdata->type <= 2)
	tdata->type = 2;
      else
	tdata->type = 3;
#endif
      entry->data  = data;
      entry->where = section->lma + offset;
      entry->size  = bytes_to_do;
      /* save section name for print to output file */
      entry->section = section;

      /* Sort the records by address.  Optimize for the common case of
	 adding a record to the end of the list.  */

      if (tdata->tail != NULL
	  && entry->where >= tdata->tail->where)
	{
	  tdata->tail->next = entry;
	  entry->next = NULL;
	  tdata->tail = entry;

	}
      else

	{
	  ldrxpic_data_list_type **look;

	  for (look = &tdata->head;
	       *look != NULL && (*look)->where < entry->where;
	       look = &(*look)->next)
	    ;
	  entry->next = *look;
	  *look = entry;
	  if (entry->next == NULL)
	    tdata->tail = entry;
	}

    }
  return TRUE;
}






///
///
///   L D R X P I C    ---    ARM ASSEMBLER   BACKEND TYPE
///
///

/** Write a ldrxpic-record: like assembler file with an array.
    Each array will be placed inside a section, named with a xpic-section name.
  */
/* some definitions */
#define XPIC_PRAM_ADDR  0x1018c000
#define XPIC_DRAM_ADDR  0x1018e000
char XPIC_PRAM_Str[] = "XPIC_PRAM";
char XPIC_DRAM_Str[] = "XPIC_DRAM";
char NETX_RAM_Str[]  = "some_RAM";

#define XPIC_PRAM_SEC_LEN  	8
#define XPIC_DRAM_SEC_LEN	6
#define XPIC_RAM_SEC_LEN	64

#define STRING_LENTH 100
#define BUFFER_LENTH 50


#define COMMENT_STRING "# "

bfd_boolean ldrxpic_printout_str(bfd *abfd, char *str);

/* Function make format of string, adding a comment chars after '\n' symbol*/
bfd_boolean ldrxpic_printout_str(bfd *abfd, char *str)
{
  char *cur = str;

  while( (*cur) != '\0')
  {
    /* comment string */
    if( bfd_bwrite ((void *) COMMENT_STRING, sizeof(COMMENT_STRING) - 1, abfd) != (unsigned)sizeof(COMMENT_STRING) - 1 )
      return FALSE;

    while(*cur != '\n')
    {
      cur++;
    };
    cur++;

    if( bfd_bwrite ((void *) str, cur-str, abfd) != (unsigned)(cur-str) )
      return FALSE;

    if( (*cur) == '\0' )
      return TRUE;

    str = cur;
  }

  return TRUE;
}


static bfd_boolean
ldrxpic_write_section_s(bfd *abfd,
		   ldrxpic_data_list_type *list,
		   const bfd_byte *data)
{
  const bfd_byte *end = data + list->size;
  sec_ptr section     = list->section;

  char buffer[BUFFER_LENTH];
  char *dst   = buffer;
  char *start = buffer;
  char *section_name = (char *)section->name;
  unsigned int section_name_len, byte_no_align;
  char SectionDirective[]=".section ";
  char SectionFlags[]=", \"a\" ";
  char MessageStr2[]="The end of the section ";


  /* length of file name, section name and full possible length*/
  section_name_len = strlen(section_name);


  /** END OF COMMENTS */


  /* print out the section name*/



  if( bfd_bwrite ((void *) SectionDirective, sizeof(SectionDirective)-1, abfd) != sizeof(SectionDirective)-1)
    return FALSE;
  if( bfd_bwrite ((void *) section_name, section_name_len, abfd) != section_name_len)
    return FALSE;
  /* set section flags     */
  if( bfd_bwrite ((void *) SectionFlags, sizeof(SectionFlags)-1, abfd) != sizeof(SectionFlags)-1)
    return FALSE;

  /* buffer initialization */
  start = buffer;
  dst   = start;

  /* newline after section declaration */
  dst += sprintf(dst,"\r\n");
  if( bfd_bwrite ((void *) start, dst-start, abfd) != (unsigned)(dst-start))
    return FALSE;

  /** write data */
  dst=start;
  /* .long 0x  */
  dst += sprintf(dst,"\r\n.long 0x");

  /* Address "end - data" must be 4byte aligned  */ 
  byte_no_align = (unsigned int)(end-data)&3;
  end -= byte_no_align;

  /* write out "\r\n.long 0xXXXXXXXX" */
  while( data < end)
  {
    dst[7] = digs[ *data       & 0x0f ];
    dst[6] = digs[ *data++ >>4 & 0x0f ];
    dst[5] = digs[ *data       & 0x0f ];
    dst[4] = digs[ *data++ >>4 & 0x0f ];
    dst[3] = digs[ *data       & 0x0f ];
    dst[2] = digs[ *data++ >>4 & 0x0f ];
    dst[1] = digs[ *data       & 0x0f ];
    dst[0] = digs[ *data++ >>4 & 0x0f ];

    if( bfd_bwrite ((void *) start, dst-start+8, abfd) != (unsigned)(dst-start+8))
      return FALSE;
  }

  /* if address "end - data" was not 4 byte aligned */
  if(byte_no_align > 0)
  {
    dst = start + 3;
    *dst++ ='b';
    *dst++ ='y';
    *dst++ ='t';
    *dst++ ='e';
    dst += 3;
    while( byte_no_align-- > 0 )
    {
      dst[1] = digs[ *data       & 0x0f ];
      dst[0] = digs[ *data++ >>4 & 0x0f ];

      if( bfd_bwrite ((void *) start, dst-start+2, abfd) != (unsigned)(dst-start+2))
        return FALSE;
    }
  }

  /* two times new line at end of the section data */
  dst=start;
  dst += sprintf(dst,"\r\n\r\n");
  if( bfd_bwrite ((void *) start, dst-start, abfd) != (unsigned)(dst-start))
    return FALSE;

  /* comment in the end of the section */
  dst=start;
  /* the comment string */
  dst += sprintf(dst,COMMENT_STRING);
  strcpy(dst, MessageStr2 );
  dst += (sizeof(MessageStr2) - 1); /* 1 is a NULL, end of string */
  memcpy(dst,section_name, section_name_len);
  dst += section_name_len;
  /* print out the first comment string */
  if( bfd_bwrite ((void *) start, dst-start, abfd) != (unsigned)(dst-start) )
    return FALSE;


  /* four times new line at end of the section */
  dst=start;
  dst += sprintf(dst,"\r\n\r\n\r\n\r\n");

  if( bfd_bwrite ((void *) start, dst-start, abfd) != (unsigned)(dst-start))
    return FALSE;

  /* return OK*/
  return TRUE;
}


/** ldrxpic target: write comment, like example for linker script  */
/**

 *  Correct your linker-script for ARM core according to this template: * /

MEMORY
{
  XPIC_PRAM   (rwx) : ORIGIN = 0x1018c000, LENGTH = 8K
  XPIC_DRAM   (rwx) : ORIGIN = 0x1018e000, LENGTH = 8K
  RAM1        (rwx) : ORIGIN = 0x08010000, LENGTH = 8K 
}

SECTIONS
{
	.FILENAME_SECNAME :
	{
		KEEP(*FILENAME.o (.text)) ;
	} > 0x1018C000 SECADDR

........................................................
........................................................
........................................................
}
*/


static bfd_boolean
ldrxpic_write_header_s (bfd *abfd, tdata_type *tdata)
{
char MessageStr1[] = "\
/*  Correct your linker-script for ARM core according to this template: */\r\n\
\r\n\
MEMORY\r\n\
{\r\n\
/* definition of memory sections, to load a code for ARM core     */\r\n\
....................................................................\r\n\
....................................................................\r\n\
....................................................................\r\n\
\r\n\
/* definition of memory sections, to load a code for xPIC core    */\r\n";

char MessageStr2[] = "\
}\r\n\
\r\n\
\r\n\
SECTIONS\r\n\
{\r\n\
/* At first the programm and data sections, to load for xPIC core */\r\n";

char MessageStr3[] = "\
\r\n\
/* Then programm and data sections, to load for ARM core          */\r\n\
....................................................................\r\n\
....................................................................\r\n\
....................................................................\r\n\
\r\n\
}\r\n";

  char buffer[STRING_LENTH];
  char *dst   = buffer;
  char *start = buffer;
  unsigned int section_name_len, file_name_len, max_len, add_sectns=0, sect_len=0;
  char *file_name, *file_name_end=NULL;
  ldrxpic_data_list_type *list;
  

  /* extract only felename without path and extension:  /abcd/aaa/filename.ext -> filename */
  file_name = (char *)abfd->filename;
  {
    char *curr_char = file_name;
    while( *curr_char )
    {
       /* */
       if(*curr_char == '.')
         file_name_end = curr_char;

       if(*curr_char == '/' || *curr_char == '\\')
         file_name = curr_char+1;

       curr_char++;
    }

    /* point to last symbol in the filename string */
    curr_char--;

    /* if file has no extension  or  a dot was in the path before, set the end of the filename the last character */
    if(file_name_end == NULL || file_name_end < file_name)
      file_name_end = curr_char;

    /* in case of wrong definition, take full name */
    if(file_name > curr_char || file_name_end > curr_char)
    {
      file_name_end = curr_char;
      file_name     = (char *)abfd->filename;
    }
  }

  /* length of file name */
  file_name_len    = file_name_end - file_name;

  if( !ldrxpic_printout_str(abfd, MessageStr1) )
    return FALSE;


  /*  Memory definitions  */
  /* Now wander though all the sections  */
  list = tdata->head;
  while (list != (ldrxpic_data_list_type *) NULL)
  {
    bfd_vma address = list->where;
    dst=start;
    /* the comment string */
    dst += sprintf(dst,COMMENT_STRING);

    *dst++ = '\t';

/* any known section? Such as XPIC_INTPRAM or XPIC_INTDRAM */
    if(address == XPIC_PRAM_ADDR)
    {
      dst += sprintf(dst,"%s", XPIC_PRAM_Str);
      sect_len=XPIC_PRAM_SEC_LEN;
    }
    else if(address == XPIC_DRAM_ADDR)
    {
      dst += sprintf(dst,"%s", XPIC_DRAM_Str);
      sect_len=XPIC_DRAM_SEC_LEN;
    }
    else 
    {
      dst += sprintf(dst,"%s%d", NETX_RAM_Str, add_sectns);
      add_sectns++;
      sect_len=XPIC_RAM_SEC_LEN;
    }

    dst += sprintf(dst,"\t(rwx) : ORIGIN = 0x%08X, LENGTH = %dK", (unsigned int)address, sect_len);

    if(sect_len == XPIC_RAM_SEC_LEN)
    {
       dst += sprintf(dst," /* check the length of this section  */");
    }

    *dst++ ='\r';
    *dst++ ='\n';

    /* print out the first comment string */
    if( bfd_bwrite ((void *) start, dst-start, abfd) != (unsigned)(dst-start) )
      return FALSE;

    list = list->next;
  }


  /* print out the end of memory definition and start of SECTIONS*/
 if( !ldrxpic_printout_str(abfd, MessageStr2) )
   return FALSE;

  add_sectns=0;

  /* Sections definitions                               */
  /* Now wander though all the sections  one more time  */
  list = tdata->head;
  while (list != (ldrxpic_data_list_type *) NULL)
  {
    bfd_vma address    = list->where;
    const char *section_name = list->section->name;

    section_name_len = strlen(section_name);
    max_len = section_name_len + file_name_len + 15 ; /* 15 -> diff chars    */
    if(max_len >= STRING_LENTH)
    {
       section_name_len = STRING_LENTH - file_name_len - 15;
    }

    /* the first string                                 */
    dst = start;
    /**                                                 */
    /* the comment string */
    dst += sprintf(dst,COMMENT_STRING);
    /** #                                               */
    *dst++ ='\t';
    *dst++ ='.';
    /** #	.                                       */
    memcpy(dst,file_name, file_name_len);
    dst += file_name_len;
    /** #	.filename                               */
    *dst++ ='_';
    /** #	.filename_                              */
    memcpy(dst,section_name+1/*skip dot*/, section_name_len-1);
    dst += section_name_len-1;
    /** #	.filename_sectionname                   */
    dst += sprintf(dst," :\r\n");
    /** #	.filename_sectionname :\r\n             */
    /* print out this string                            */
    if( bfd_bwrite ((void *) start, dst-start, abfd) != (unsigned)(dst-start) )
      return FALSE;


    /* the second string                                */
    dst = start;
    /**                                                 */
    /* the comment string */
    dst += sprintf(dst,COMMENT_STRING);
    /** #                                               */
    dst += sprintf(dst,"\t{\r\n");
    /** #	{\r\n                                   */
    /* print out this string                            */
    if( bfd_bwrite ((void *) start, dst-start, abfd) != (unsigned)(dst-start) )
      return FALSE;

    /* the third string                                 */
    dst = start;
    /**                                                 */
    /* the comment string */
    dst += sprintf(dst,COMMENT_STRING);
    /** #                                               */
    dst += sprintf(dst,"\t\tKEEP(*");
    /** #		KEEP(*                          */
    memcpy(dst,file_name, file_name_len);
    dst += file_name_len;
    /** #		KEEP(*filename                  */
    dst += sprintf(dst,".o (");
    /** #		KEEP(*filename.o (              */
    memcpy(dst,section_name, section_name_len);
    dst += section_name_len;
    /** #		KEEP(*filename.o (.sectionname  */
    dst += sprintf(dst,")) ;\r\n");
    /** #		KEEP(*filename.o (.sectionname)) ;\r\n  */
    /* print out this string                            */
    if( bfd_bwrite ((void *) start, dst-start, abfd) != (unsigned)(dst-start) )
      return FALSE;

    /* the last string                                  */
    dst = start;
    /**                                                 */
    /* the comment string */
    dst += sprintf(dst,COMMENT_STRING);
    /** #                                               */
    dst += sprintf(dst,"\t} > ");
    /** #	} >                                     */
    /* Get name of the section                          */
    /* any known section? Such as XPIC_INTPRAM or XPIC_INTDRAM */
    if(address == XPIC_PRAM_ADDR)
    {
      memcpy(dst,XPIC_PRAM_Str, sizeof(XPIC_PRAM_Str)-1);
      dst += sizeof(XPIC_PRAM_Str)-1;
    }
    else if(address == XPIC_DRAM_ADDR)
    {
      memcpy(dst,XPIC_DRAM_Str, sizeof(XPIC_DRAM_Str)-1);
      dst += sizeof(XPIC_DRAM_Str)-1;
    }
    else 
    {
      dst += sprintf(dst,"RAM%d", add_sectns);
      add_sectns++;
    }
    /** #	} > SECTION_NAME                        */
    dst += sprintf(dst,"\r\n%s\r\n",COMMENT_STRING);
    /** #	} > SECTION_NAME\r\n                    */
    /* print out this string                            */
    if( bfd_bwrite ((void *) start, dst-start, abfd) != (unsigned)(dst-start) )
      return FALSE;

    list = list->next;
  }

  /* print out the end of SECTIONs definition           */
  if( !ldrxpic_printout_str(abfd, MessageStr3) )
    return FALSE;

  /* newline at end of header                           */
    dst = start;
    dst += sprintf(dst,"\r\n\r\n");
    /* print out this string                            */
    if( bfd_bwrite ((void *) start, dst-start, abfd) != (unsigned)(dst-start) )
      return FALSE;

  return TRUE;
}

#define NOT_USED(x) ( x = x );

/*
static bfd_boolean
ldrxpic_write_finalize_s (bfd *abfd, tdata_type *tdata)
{
	NOT_USED(abfd);
	NOT_USED(tdata);
   return TRUE;
}
*/
///
///
///   L D R X P I C    ---    C   BACKEND TYPE
///
///
#include <time.h>
/* ldrxpic target options */
#define LDRXPIC_MAJOR_DEFAULT	1
#define LDRXPIC_MINOR_DEFAULT	0
static unsigned short ldrxpic_major = LDRXPIC_MAJOR_DEFAULT;
static unsigned short ldrxpic_minor = LDRXPIC_MINOR_DEFAULT;

void ldrxpic_major_set(unsigned short set_val)
{
  ldrxpic_major = set_val;
}

void ldrxpic_minor_set(unsigned short set_val)
{
  ldrxpic_minor = set_val;
}

static bfd_boolean
ldrxpic_write_header_c (bfd *abfd, tdata_type *tdata)
{
char MessageStr1[] = "\
/* Timestamp: sec,min,hour,day,month,year-1900,seconds since 1.1.1970 */\r\n\
const unsigned long g_aul";

char MessageStr2[] = "const unsigned long g_aul";

	NOT_USED(tdata);

// const unsigned long g_aulXpicPrgBuildTime[7] = {31,9,12,21,11,110,1292929771};


  char buffer[STRING_LENTH];
  char *dst   = buffer;
  char *start = buffer;
  /* unsigned int section_name_len; */
  unsigned int file_name_len;
	/* unsigned int max_len; */
	/* unsigned int add_sectns=0; */
	/* unsigned int sect_len=0; */
  char *file_name, *file_name_end=NULL;
  /* ldrxpic_data_list_type *list; */
  

  /* extract only felename without path and extension:  /abcd/aaa/filename.ext -> filename */
  file_name = (char *)abfd->filename;
  {
    char *curr_char = file_name;
    while( *curr_char )
    {
       /* */
       if(*curr_char == '.')
         file_name_end = curr_char;

       if(*curr_char == '/' || *curr_char == '\\')
         file_name = curr_char+1;

       curr_char++;
    }

    /* point to last symbol in the filename string */
    curr_char--;

    /* if file has no extension  or  a dot was in the path before, set the end of the filename the last character */
    if(file_name_end == NULL || file_name_end < file_name)
      file_name_end = curr_char;

    /* in case of wrong definition, take full name */
    if(file_name > curr_char || file_name_end > curr_char)
    {
      file_name_end = curr_char;
      file_name     = (char *)abfd->filename;
    }
  }


  /**                                                                          */
  /* length of file name */
  file_name_len    = file_name_end - file_name;

  if( bfd_bwrite ((void *) MessageStr1, sizeof(MessageStr1)-1, abfd) != (unsigned)(sizeof(MessageStr1)-1) )
    return FALSE;

  /**  Timestamp: sec,min,hour,day,month,year-1900,seconds since 1.1.1970   */
  /**  const unsigned long g_aul                                               */

  if( bfd_bwrite ((void *) file_name, file_name_len, abfd) != (unsigned)(file_name_len) )
    return FALSE;
  /**  const unsigned long g_aulFILENAME                                       */

  /* time and date */
  time_t rawtime;
  struct tm * ptm;
  // time_t seconds;
  time ( &rawtime );
  ptm = gmtime ( &rawtime );

  dst += sprintf(dst,"PrgBuildTime[7] = { %d, %d, %d, %d, %d, %d, %zu};\r\n\r\n", ptm->tm_sec, ptm->tm_min, ptm->tm_hour, ptm->tm_mday, ptm->tm_mon+1,  ptm->tm_year, rawtime);
  if( bfd_bwrite ((void *) start, dst-start, abfd) != (unsigned)(dst-start) )
    return FALSE;
  /**  const unsigned long g_aulFILENAMEPrgBuildTime[7] = { sec, min, hour, day, month, year-1900, second since 1.1.1970  */

  dst=start;

  if( bfd_bwrite ((void *) MessageStr2, sizeof(MessageStr2)-1, abfd) != (unsigned)(sizeof(MessageStr2)-1) )
    return FALSE;
  /**  const unsigned long g_aul                                               */

  if( bfd_bwrite ((void *) file_name, file_name_len, abfd) != (unsigned)(file_name_len) )
    return FALSE;
  /**  const unsigned long g_aulFILENAME                                       */

  unsigned int   Par0 = 0;
  unsigned int   Par1 = 0;
  unsigned int   Par2 = 0x43495058;
  unsigned short versionMajor = ldrxpic_major;
  unsigned short versionMinor = ldrxpic_minor;
  dst += sprintf(dst,"\
Prg[] = {\r\n\
\t/* HEADER */\r\n\
\t%d, %d, 0x%08x, 0x%04x%04x,\
\r\n", Par0, Par1, Par2,versionMajor, versionMinor);

  if( bfd_bwrite ((void *) start, dst-start, abfd) != (unsigned)(dst-start) )
    return FALSE;

  return TRUE;
}


static bfd_boolean
ldrxpic_write_section_c(bfd *abfd,
		   ldrxpic_data_list_type *list,
		   const bfd_byte *data
		   )
{
  const bfd_byte *end = data + list->size;
  sec_ptr section     = list->section;

  char buffer[STRING_LENTH];
  char *dst   = buffer;
  char *start = buffer;
  char *section_name = (char *)section->name;


  unsigned int sec_size = end-data;
  unsigned int align;
  unsigned int column;
  /* 0 if dword aligned */
  align = sec_size & 3;

  /* if not dword aligned, align section size to dword */
  if(align)
  {
    sec_size += 4 - align;
    end -= align;
  }


  char HeadStr[]="\r\n\t/* SEGMENT (section: ";

  /**                                                                                                                      */
  if( bfd_bwrite ((void *) HeadStr, sizeof(HeadStr)-1, abfd) != (unsigned)(sizeof(HeadStr)-1) )
    return FALSE;

  /**\t SEGMENT (section:                                                                                                */
  dst += sprintf(dst,"%s ) */\r\n",section_name);
  dst += sprintf(dst,"\t0x%08tx, /* address */\r\n", list->where);
  dst += sprintf(dst,"\t0x%08x, /* length  */\r\n\t", sec_size>>2);
  if( bfd_bwrite ((void *) start, dst-start, abfd) != (unsigned)(dst-start) )
    return FALSE;

  /** \t/ * SEGMENT (section: SECT_NAME ) * /                                                                              */

  /* write section content */
  dst=start;
  column=0;
  while( data < end)
  {
    dst += sprintf(dst,"0x%02x%02x%02x%02x,", data[0], data[1], data[2], data[3]);
		data += 4;

    column ++;
    if(column > 7)
    {
	dst += sprintf(dst,"\r\n\t");
	if( bfd_bwrite ((void *) start, dst-start, abfd) != (unsigned)(dst-start) )
	  return FALSE;

        dst=start;
	column = 0;
    }
  }

  if(dst!=start)
  {
    dst += sprintf(dst,"\r\n\t");
    if( bfd_bwrite ((void *) start, dst-start, abfd) != (unsigned)(dst-start) )
      return FALSE;
  }

  /* if section was not dword aligned */
  if(align)
  {
    unsigned int i;
    union {
            unsigned int  uiI;
            unsigned char uiCh[4];
    } un_dword;

    dst=start;

    for(i=0; i<4; i++)
      if(i<align)
        un_dword.uiCh[i] = *data++;
      else
        un_dword.uiCh[i] = 0;

    dst += sprintf(dst,"0x%08x,",un_dword.uiI);

    if( bfd_bwrite ((void *) start, dst-start, abfd) != (unsigned)(dst-start) )
      return FALSE;
  }

  /* return OK*/
  return TRUE;
}


static bfd_boolean
ldrxpic_write_finalize_c (bfd *abfd, tdata_type *tdata)
{
  char FinalMessageStr[] = "\
/* END SEGMENT */\r\n\
\t0, 0\r\n\
};\r\n";

	NOT_USED(tdata);

  if( bfd_bwrite ((void *) FinalMessageStr, sizeof(FinalMessageStr)-1, abfd) != (unsigned)(sizeof(FinalMessageStr)-1) )
    return FALSE;

   return TRUE;
}

///
///
///   L D R X P I C    ---    Scheduler
///
///
typedef bfd_boolean (*ldrxpic_write_header_func)  (bfd *abfd, tdata_type *tdata);
typedef bfd_boolean (*ldrxpic_write_section_func) (bfd *abfd, ldrxpic_data_list_type *list, const bfd_byte *data);
typedef bfd_boolean (*ldrxpic_write_finalize)     (bfd *abfd, tdata_type *tdata);

static bfd_boolean
internal_ldrxpic_write_object_contents (bfd *abfd, int symbols)
{
	NOT_USED(symbols);

  tdata_type *tdata = abfd->tdata.srec_data;
  ldrxpic_data_list_type *list;

  /* call functions prototypes */ 
  ldrxpic_write_header_func   ldrxpic_write_header   = NULL;
  ldrxpic_write_section_func  ldrxpic_write_section  = NULL;
  ldrxpic_write_finalize      ldrxpic_write_finalize = NULL;

  /* what backend type of the ldrxpic to call: *.S / *.c */
  /* see in the extension of the target name */
  int fname_len = strlen(abfd->filename);

  switch(abfd->filename[fname_len-1])
  {
    /* C backend type */
    case 'p':
    case 'P':
    case 'h':
    case 'H':
    case 'c':
    case 'C':
             ldrxpic_write_header   = ldrxpic_write_header_c;
             ldrxpic_write_section  = ldrxpic_write_section_c;
             ldrxpic_write_finalize = ldrxpic_write_finalize_c;
             break;
    /* arm-assembler backend type, default type */
    case 's':
    case 'S':
    default:
             ldrxpic_write_header   = ldrxpic_write_header_s;
             ldrxpic_write_section  = ldrxpic_write_section_s;
             break;
  }


  /* emit header */
  if(ldrxpic_write_header)
    if (! ldrxpic_write_header (abfd, tdata))
      return FALSE;


  if(ldrxpic_write_section)
  {
    /* Now wander though all the sections provided and output them.  */
    list = tdata->head;
    while (list != (ldrxpic_data_list_type *) NULL)
    {
      bfd_byte *location = list->data;
      //if (! ldrxpic_write_section (abfd, list, location, location + list->size, list->section))
      if (! ldrxpic_write_section (abfd, list, location))
        return FALSE;
      list = list->next;
    }
  }

  /* emit epilogue */
  if(ldrxpic_write_finalize)
    if (! ldrxpic_write_finalize (abfd, tdata))
      return FALSE;


  return TRUE;
}




///
///
///   E N D      O F      L D R X P I C
///
///













static bfd_boolean
ldrxpic_write_object_contents (bfd *abfd)
{
  return internal_ldrxpic_write_object_contents (abfd, 0);
}

static bfd_boolean
symbolldrxpic_write_object_contents (bfd *abfd)
{
  return internal_ldrxpic_write_object_contents (abfd, 1);
}

static int
ldrxpic_sizeof_headers (bfd *abfd ATTRIBUTE_UNUSED,
		     struct bfd_link_info *info ATTRIBUTE_UNUSED)
{
  return 0;
}

/* Return the amount of memory needed to read the symbol table.  */

static long
ldrxpic_get_symtab_upper_bound (bfd *abfd)
{
  return (bfd_get_symcount (abfd) + 1) * sizeof (asymbol *);
}

/* Return the symbol table.  */

static long
ldrxpic_canonicalize_symtab (bfd *abfd, asymbol **alocation)
{
  bfd_size_type symcount = bfd_get_symcount (abfd);
  asymbol *csymbols;
  unsigned int i;

  csymbols = abfd->tdata.srec_data->csymbols;
  if (csymbols == NULL && symcount != 0)
    {
      asymbol *c;
      struct ldrxpic_symbol *s;

      csymbols = (asymbol *) bfd_alloc (abfd, symcount * sizeof (asymbol));
      if (csymbols == NULL)
	return -1;
      abfd->tdata.srec_data->csymbols = csymbols;

      for (s = abfd->tdata.srec_data->symbols, c = csymbols;
	   s != NULL;
	   s = s->next, ++c)
	{
	  c->the_bfd = abfd;
	  c->name = s->name;
	  c->value = s->val;
	  c->flags = BSF_GLOBAL;
	  c->section = bfd_abs_section_ptr;
	  c->udata.p = NULL;
	}
    }

  for (i = 0; i < symcount; i++)
    *alocation++ = csymbols++;
  *alocation = NULL;

  return symcount;
}

static void
ldrxpic_get_symbol_info (bfd *ignore_abfd ATTRIBUTE_UNUSED,
		      asymbol *symbol,
		      symbol_info *ret)
{
  bfd_symbol_info (symbol, ret);
}

static void
ldrxpic_print_symbol (bfd *abfd,
		   void * afile,
		   asymbol *symbol,
		   bfd_print_symbol_type how)
{
  FILE *file = (FILE *) afile;

  switch (how)
    {
    case bfd_print_symbol_name:
      fprintf (file, "%s", symbol->name);
      break;
    default:
      bfd_print_symbol_vandf (abfd, (void *) file, symbol);
      fprintf (file, " %-5s %s",
	       symbol->section->name,
	       symbol->name);
    }
}

#define	ldrxpic_close_and_cleanup                    _bfd_generic_close_and_cleanup
#define ldrxpic_bfd_free_cached_info                 _bfd_generic_bfd_free_cached_info
#define ldrxpic_new_section_hook                     _bfd_generic_new_section_hook
#define ldrxpic_bfd_is_target_special_symbol         ((bfd_boolean (*) (bfd *, asymbol *)) bfd_false)
#define ldrxpic_bfd_is_local_label_name              bfd_generic_is_local_label_name
#define ldrxpic_get_lineno                           _bfd_nosymbols_get_lineno
#define ldrxpic_find_nearest_line                    _bfd_nosymbols_find_nearest_line
#define ldrxpic_find_inliner_info                    _bfd_nosymbols_find_inliner_info
#define ldrxpic_make_empty_symbol                    _bfd_generic_make_empty_symbol
#define ldrxpic_bfd_make_debug_symbol                _bfd_nosymbols_bfd_make_debug_symbol
#define ldrxpic_read_minisymbols                     _bfd_generic_read_minisymbols
#define ldrxpic_minisymbol_to_symbol                 _bfd_generic_minisymbol_to_symbol
#define ldrxpic_get_section_contents_in_window       _bfd_generic_get_section_contents_in_window
#define ldrxpic_bfd_get_relocated_section_contents   bfd_generic_get_relocated_section_contents
#define ldrxpic_bfd_relax_section                    bfd_generic_relax_section
#define ldrxpic_bfd_gc_sections                      bfd_generic_gc_sections
#define ldrxpic_bfd_merge_sections                   bfd_generic_merge_sections
#define ldrxpic_bfd_is_group_section                 bfd_generic_is_group_section
#define ldrxpic_bfd_discard_group                    bfd_generic_discard_group
#define ldrxpic_section_already_linked               _bfd_generic_section_already_linked
#define ldrxpic_bfd_define_common_symbol             bfd_generic_define_common_symbol
#define ldrxpic_bfd_link_hash_table_create           _bfd_generic_link_hash_table_create
#define ldrxpic_bfd_link_hash_table_free             _bfd_generic_link_hash_table_free
#define ldrxpic_bfd_link_add_symbols                 _bfd_generic_link_add_symbols
#define ldrxpic_bfd_link_just_syms                   _bfd_generic_link_just_syms
#define ldrxpic_bfd_final_link                       _bfd_generic_final_link
#define ldrxpic_bfd_link_split_section               _bfd_generic_link_split_section

const bfd_target ldrxpic_vec =
{
  "ldrxpic",			/* Name.  */
  bfd_target_srec_flavour,
  BFD_ENDIAN_UNKNOWN,		/* Target byte order.  */
  BFD_ENDIAN_UNKNOWN,		/* Target headers byte order.  */
  (HAS_RELOC | EXEC_P |		/* Object flags.  */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | WP_TEXT | D_PAGED),
  (SEC_CODE | SEC_DATA | SEC_ROM | SEC_HAS_CONTENTS
   | SEC_ALLOC | SEC_LOAD | SEC_RELOC),	/* Section flags.  */
  0,				/* Leading underscore.  */
  ' ',				/* AR_pad_char.  */
  16,				/* AR_max_namelen.  */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
  bfd_getb32, bfd_getb_signed_32, bfd_putb32,
  bfd_getb16, bfd_getb_signed_16, bfd_putb16,	/* Data.  */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
  bfd_getb32, bfd_getb_signed_32, bfd_putb32,
  bfd_getb16, bfd_getb_signed_16, bfd_putb16,	/* Hdrs.  */

  {
    _bfd_dummy_target,
    ldrxpic_object_p,		/* bfd_check_format.  */
    _bfd_dummy_target,
    _bfd_dummy_target,
  },
  {
    bfd_false,
    ldrxpic_mkobject,
    _bfd_generic_mkarchive,
    bfd_false,
  },
  {				/* bfd_write_contents.  */
    bfd_false,
    ldrxpic_write_object_contents,
    _bfd_write_archive_contents,
    bfd_false,
  },

  BFD_JUMP_TABLE_GENERIC (ldrxpic),
  BFD_JUMP_TABLE_COPY (_bfd_generic),
  BFD_JUMP_TABLE_CORE (_bfd_nocore),
  BFD_JUMP_TABLE_ARCHIVE (_bfd_noarchive),
  BFD_JUMP_TABLE_SYMBOLS (ldrxpic),
  BFD_JUMP_TABLE_RELOCS (_bfd_norelocs),
  BFD_JUMP_TABLE_WRITE (ldrxpic),
  BFD_JUMP_TABLE_LINK (ldrxpic),
  BFD_JUMP_TABLE_DYNAMIC (_bfd_nodynamic),

  NULL,

  NULL
};

const bfd_target symbolldrxpic_vec =
{
  "symbolldrxpic",			/* Name.  */
  bfd_target_srec_flavour,
  BFD_ENDIAN_UNKNOWN,		/* Target byte order.  */
  BFD_ENDIAN_UNKNOWN,		/* Target headers byte order.  */
  (HAS_RELOC | EXEC_P |		/* Object flags.  */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | WP_TEXT | D_PAGED),
  (SEC_CODE | SEC_DATA | SEC_ROM | SEC_HAS_CONTENTS
   | SEC_ALLOC | SEC_LOAD | SEC_RELOC),	/* Section flags.  */
  0,				/* Leading underscore.  */
  ' ',				/* AR_pad_char.  */
  16,				/* AR_max_namelen.  */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
  bfd_getb32, bfd_getb_signed_32, bfd_putb32,
  bfd_getb16, bfd_getb_signed_16, bfd_putb16,	/* Data.  */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
  bfd_getb32, bfd_getb_signed_32, bfd_putb32,
  bfd_getb16, bfd_getb_signed_16, bfd_putb16,	/* Headers.  */

  {
    _bfd_dummy_target,
    symbolldrxpic_object_p,	/* bfd_check_format.  */
    _bfd_dummy_target,
    _bfd_dummy_target,
  },
  {
    bfd_false,
    ldrxpic_mkobject,
    _bfd_generic_mkarchive,
    bfd_false,
  },
  {				/* bfd_write_contents.  */
    bfd_false,
    symbolldrxpic_write_object_contents,
    _bfd_write_archive_contents,
    bfd_false,
  },

  BFD_JUMP_TABLE_GENERIC (ldrxpic),
  BFD_JUMP_TABLE_COPY (_bfd_generic),
  BFD_JUMP_TABLE_CORE (_bfd_nocore),
  BFD_JUMP_TABLE_ARCHIVE (_bfd_noarchive),
  BFD_JUMP_TABLE_SYMBOLS (ldrxpic),
  BFD_JUMP_TABLE_RELOCS (_bfd_norelocs),
  BFD_JUMP_TABLE_WRITE (ldrxpic),
  BFD_JUMP_TABLE_LINK (ldrxpic),
  BFD_JUMP_TABLE_DYNAMIC (_bfd_nodynamic),

  NULL,

  NULL
};
