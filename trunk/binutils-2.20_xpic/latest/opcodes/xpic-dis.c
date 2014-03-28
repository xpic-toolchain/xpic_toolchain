/* Disassemble AVR instructions.
   Copyright 1999, 2000, 2002, 2004, 2005, 2006, 2007, 2008
   Free Software Foundation, Inc.

   Contributed by Denis Chertykov <denisc@overta.ru>

   This file is part of libopcodes.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include <assert.h>
#include "sysdep.h"
#include "dis-asm.h"
#include "opintl.h"
#include "libiberty.h"
#include "elf/xpic.h"
// define this just only for objdump utility, not for GDB
#define FOR_OBJDUMP

struct xpic_opcodes_s
{
  char *name;
  char *constraints;
  int insn_size;		/* In words.  */
  int isa;
  unsigned int bin_opcode;
  char *opcode;
};

#define XPIC_INSN(NAME, CONSTR, OPCODE, SIZE, ISA, BIN) \
{#NAME, CONSTR, SIZE, ISA, BIN, OPCODE},

const struct xpic_opcodes_s xpic_opcodes[] =
{
  #include "opcode/xpic.h"
  {NULL, NULL,  0, 0, 0, NULL}
};

//static const char * comment_start = "0x";

static char *CondCode[] ={
"always","z","c","s","v","e","gu","gs","geu","ges","r0z","r1z","r2z","r3z",
"r0123z","reserved","never","nz","nc","ns","nv","ne","leu","les","lu","ls","r0nz","r1nz","r2nz","r3nz","r0123nz", "reserved", NULL };

static char *Registers[] ={
"r0\0\0","r1\0\0","r2\0\0","r3\0\0","r4\0\0","r5\0\0","r6\0\0","r7\0\0","u0\0\0","u1\0\0","u2\0\0","u3\0\0","u4\0\0","pc","st\0\0","z0\0\0",NULL};

static void xpic_get_reg_srtr(int reg, char *str)
{
 if(*str=='D')
 {
   if(reg==REG_ZERO)
    {char r[]="u10"; memcpy(str,r,6);return ;}
   if(reg==REG_PC)
    {char r[]="u32"; memcpy(str,r,6);return ;}
 }
 memcpy(str,Registers[reg&0x0f],5);
}

static int xpic_get_const_srtr(unsigned int con, char *str)
{
  int C=0;
  switch(*str)
  {
   case 'I':
   case 'M':
   case 'T':
   case 'U': C = con;						break; 

   case 'X': C = (con & 0x4000 ? (con|0xffff8000) : con)<<2;	break;
   case 'W': C = con<<2;					break;
   case 'V': C = (con & 0x4000 ? (con|0xffff8000) : con);	break;
   case 'u': C = con+16;					break; 
   case 'Z': C = (con & 0x40000 ? (con|0xfff80000) : con);	break;
   //case 'Y': 	break;
   default: break;
  };

  sprintf(str,"#%d",C);
  return 1;
}

static unsigned int xpic_dis_opcode (bfd_vma addr, disassemble_info *info)
{
  bfd_byte buffer[4];
  int status;

  status = info->read_memory_func (addr, buffer, 4, info);

  if (status == 0)
    return bfd_getl32 (buffer);

  info->memory_error_func (status, addr, info);
  return -1;
}

static int xpic_get_operand(char pattern, const struct xpic_opcodes_s *opcode,int bin)
{
  int i,j,shift=-1,mask=0; 
  char *strOpcode = opcode->opcode;

  for(i=0,j=31; j>=0; j--,i++)
  {
    if( (shift==-1) && (pattern==strOpcode[j]) )
      shift=i;

    mask <<=1;
    mask |= (strOpcode[i] == pattern);
  }

  if( (mask==0) || (shift == -1) )
  {
    if(pattern == 'e') return -1;
    return xpic_get_operand('e',opcode,bin);
  }
  if(pattern == 'e')return ( (bin & mask)>>shift ) | 0x0080;
  return (bin & mask)>>shift;
}

static int xpic_get_ext(int ext,int *iSign,int *iPost, int *iMas)
{
  int N=-1;
  ext &= ~0x00000080;
  if( (ext>= 0)&&(ext<= 4) )	N=ext+ 1;
  if( (ext>= 5)&&(ext<= 9) )	N=ext+ 4;
  if( (ext>=10)&&(ext<=15) )	N=ext+ 7;
  if(N==-1) {*iSign=-1;*iPost=-1;*iMas=-1;return -1;}//error
  *iPost = (N>>3) & 3;
  *iMas  = 2 - ((N>>1) & 3);
  *iSign = 1 - (N & 1);
  return 1;
}

#ifdef FOR_OBJDUMP
// original is on "objdump.c", line 134
struct copy_objdump_disasm_info 
{
	bfd *              abfd;
	asection *         sec;
	bfd_boolean        require_sec;
	arelent **         dynrelbuf;
	long               dynrelcount;
	disassembler_ftype disassemble_fn;
	arelent *          reloc;
};

// Function "find_symbol_for_address" is really static, but I want it to use for my needs
// atribute "static" is commented in "objdump.c", line 724
// AND in function "find_symbol_for_address" were added additional line "&& (sec->flags & SEC_CODE)!=0" on 792
extern asymbol *find_symbol_for_address (bfd_vma vma,struct disassemble_info *info,long *place);
extern int XPIC_SEC_FLAG;
#endif
int print_insn_xpic (bfd_vma addr, disassemble_info *info)
{
  unsigned int insn;
  const struct xpic_opcodes_s *opcode;
  void *stream = info->stream;// here print
  fprintf_ftype prin = info->fprintf_func;
  int cmd_len = 4;
  unsigned int nopcodes;
  static unsigned int *maskptr;
  static unsigned int *xpic_bin_masks=NULL;
  static unsigned int last_opcode=0;
  static unsigned int last_but_one_opcode=0;
  char Str[12];

/// number of opcodes in table:
  nopcodes = sizeof (xpic_opcodes) / sizeof (struct xpic_opcodes_s);
/// get current instruction:
  insn = xpic_dis_opcode(addr, info);
/// build masks:
  if(xpic_bin_masks==NULL)
  {
   xpic_bin_masks = xmalloc (nopcodes * sizeof (unsigned int));
   for(opcode = xpic_opcodes, maskptr=xpic_bin_masks; opcode->name; opcode++, maskptr++)
   {
    char * s;
    unsigned int mask = 0;
    for (s = opcode->opcode; *s; ++s)
    {
      mask <<=1;
      mask |= (*s == '1' || *s == '0');

    }
    *maskptr = mask;
   }
  }
/// serch for an opcode:
  for(opcode = xpic_opcodes, maskptr=xpic_bin_masks; opcode->name; opcode++, maskptr++)
  {
   if( (insn & *maskptr) == opcode->bin_opcode)
   break;
  }
/// found?
 if(opcode->name==NULL)
 {///no:  Unknown opcode, or invalid combination of operands
   (*prin) (stream, "%s\t0x%08x\t// data or unknown instruction",".long",insn);
 }
 else
 {
  int i,iSign=-1,iPost=-1,iMas=-1,regR=-1;
/// delete last character in name (number) and print name at first
  char *constrain = opcode->constraints;
  char OpName[20];

  struct 
  {
    unsigned int uiInfoAddr[3];
    int iNumInfo;
  } tInfo;
  tInfo.iNumInfo = 0;

#ifdef FOR_OBJDUMP
/// disassemble just executable sections
  if(info->section!=NULL && (info->section->flags & SEC_CODE)==0)
  {
    goto xpic_disass_info;
  }
#endif

  // condition execution
  if(*constrain=='C')
  {
    int cc = xpic_get_operand(*constrain,opcode,insn);
    if(cc > 0 && cc < 31)
     (*prin) (stream, " [%s]", CondCode[cc]);
    constrain++;
  }
  memcpy(OpName,opcode->name,strlen(opcode->name));
  OpName[strlen(opcode->name)-1]=0;
  (*prin) (stream, " %s  ", OpName);
/// then evaluate instruction operands +
  while(*constrain)
  {
    switch(*constrain)
    {
// registers:
      case 'D': 
      case 'd':
      case 'r': 
      case 'w': 
      case 'q':{int reg = xpic_get_operand(*constrain,opcode,insn);	
		if(reg!=-1)
		{
		 regR=reg;
		 Str[0]=*constrain;
		 xpic_get_reg_srtr(reg, Str);
		 (*prin) (stream, "%s", Str);
		}
		else (*prin) (stream, SYMB_UNDEF);///error code
		break;}
// constants:
      case 'Y':{int mas,con;
                con = xpic_get_operand(*constrain,opcode,insn);
		if(con!=-1){
                 con = (con & (int)0x10000 ? (con|(int)0xfffe0000) : con);
                 mas = (insn>>18) & 3;
//                 if(mas == MAS_BYTE ) (*prin) (stream, "#%d", con   );
                 if(mas == MAS_WORD )con<<=1;
                 if(mas == MAS_DWORD)con<<=2;
 		 (*prin) (stream, "#%d", con);
		// Info:
		 if(regR == REG_ZERO)
		   tInfo.uiInfoAddr[tInfo.iNumInfo++]=(unsigned int)con;
		 if(regR == REG_PC)
		   tInfo.uiInfoAddr[tInfo.iNumInfo++]=(unsigned int)(con+addr);
		}
		else (*prin) (stream, SYMB_UNDEF);///error code
               } break;
      case 'I':
      case 'M':
      case 'T': 
      case 'U': 
      case 'u':
      case 'V': 
      case 'W': 
      case 'X': 
      case 'Z':{int con = xpic_get_operand(*constrain,opcode,insn);	
		if(con!=-1)
		{
		 Str[0]=*constrain;
		 xpic_get_const_srtr(con, Str);
		 (*prin) (stream, "%s", Str);
		// Info:
		 if(regR == REG_PC  &&  *constrain == 'Z')
		   tInfo.uiInfoAddr[tInfo.iNumInfo++]=(unsigned int)con;
		}
		else (*prin) (stream, SYMB_UNDEF);///error code
	       }break;
// flags:
      case 's': iSign = xpic_get_operand(*constrain,opcode,insn);
                if(iSign & 0x80) xpic_get_ext(iSign,&iSign,&iPost,&iMas);
                if(iSign==1)(*prin) (stream, "s");
		break;
      case 'P':
      case 'p':if(iPost==-1){
                iPost = xpic_get_operand(*constrain,opcode,insn);
                if(iPost & 0x80) xpic_get_ext(iSign,&iSign,&iPost,&iMas);
                            }
                if((*constrain=='p')&&(iPost==PP_PREDECREMENT )) (*prin) (stream, "--");
                if((*constrain=='P')&&(iPost==PP_POSTINCREMENT)) (*prin) (stream, "++");
		break;
      case 'm':if(iMas==-1){
		iMas = xpic_get_operand(*constrain,opcode,insn);
                if(iMas & 0x80) xpic_get_ext(iMas,&iSign,&iPost,&iMas);
			   }
                if(iMas==MAS_BYTE) (*prin) (stream, "b");
                if(iMas==MAS_WORD) (*prin) (stream, "w");
                if(iMas==MAS_DWORD)(*prin) (stream, "d");
		break;
      case 'S': {int bs = xpic_get_operand(*constrain,opcode,insn);
                if(bs==BS_BYTE) (*prin) (stream, "_bs");
                if(bs==BS_WORD) (*prin) (stream, "_ws");
		break;}
      case 'c': {int condc = xpic_get_operand(*constrain,opcode,insn);
		 if(condc>=0)
		 (*prin) (stream, "%s", CondCode[condc]);
		 else (*prin) (stream, SYMB_UNDEF);///error code
		 break;}
      case 'j': 
	{
	int rel_addr = ((((insn>>5) & 0x00ffffff) ^ 0x00800000) - 0x00800000) <<2;
//	sprintf (buf, ".%+-8d", rel_addr);
	(*prin) (stream, "#%d",rel_addr);

  tInfo.uiInfoAddr[tInfo.iNumInfo++]=(unsigned int)(addr+rel_addr);


		 break;
	}
      case 'v': {int condc = xpic_get_operand(*constrain,opcode,insn);
		 if((condc>=0) && (condc<=3))
		      (*prin) (stream, "%s", CondCode[condc+26]);
		 else (*prin) (stream, SYMB_UNDEF);///error code
		 break;}
// symbols:
      case 'x':	  (*prin) (stream, "pc ");break;
      case '+':	  (*prin) (stream, " + ");break;
      case ']':	  (*prin) (stream, " ]");break;
      case '[':	  (*prin) (stream, "[ ");break;
      case ',':	  (*prin) (stream, ", ");break;
      default: (*prin) (stream, " ? "); break;
    }// switch(*opcode)
    constrain++;
  };

#ifdef FOR_OBJDUMP
xpic_disass_info:
// print info in code section
//printf ("\nSection %s, FLAGS=0x%x \n",info->section->name, info->section->flags );
  if(info->section!=NULL)
  {
   if(info->section->flags & SEC_CODE)
   {// this info is accessible just in programm section:
    if(tInfo.iNumInfo>0 && tInfo.iNumInfo<3)
    {
     (*prin) (stream, "\t\t\t");
     for(i=0;i<tInfo.iNumInfo;i++)
     {
      XPIC_SEC_FLAG=SEC_CODE;
      info->print_address_func (tInfo.uiInfoAddr[i], info,0);
      XPIC_SEC_FLAG=SEC_DATA;
      info->print_address_func (tInfo.uiInfoAddr[i], info,1);
     }
    }
    // for longjump, that is not from INT_PRAM, for address:
    if(addr>XPIC_MEM_INTPRAM_LEN && last_opcode==XPIC_INS_STORE_dSP_R6 &&((last_but_one_opcode&XPIC_INS_LOAD_PC_MPC_OFF_)==XPIC_INS_LOAD_PC_MPC_OFF_)){
      (*prin) (stream, "\t\t");
      info->print_address_func (insn, info,0);
      (*prin) (stream, " (Call address)");
    }
   }else
   {
     // this part of info is aviable just for "elf", not for objects
//     if(((struct copy_objdump_disasm_info *)info->application_data)->reloc == NULL)
     {
       asymbol *sym = find_symbol_for_address (insn, info, NULL);
       if(sym!=0)
       {
        if( insn == (sym->section->vma + sym->value))
        { 
          XPIC_SEC_FLAG=SEC_CODE;
          info->print_address_func (insn, info,0);
        }
       }
     }
   }

  }
#endif
  //save opcode of last 2 instructions
  last_but_one_opcode=last_opcode;
  last_opcode=insn;
 }// else

  return cmd_len;
}

///  end

