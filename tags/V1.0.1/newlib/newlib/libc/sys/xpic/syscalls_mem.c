/**************************************************************************************
 
   Copyright (c) Hilscher GmbH. All Rights Reserved.
 
 **************************************************************************************
 
   Filename:
    $Workfile: libc_mem_support.c $
   Last Modification:
    $Author: Burkhard $
    $Modtime: 12.06.07 9:30 $
    $Revision: 1 $
   
   Targets:
     OS Independent : yes
 
   Description:
   libc_mem_support.c  Operating system specific functions for libc memory support (malloc, ...)
   This function assumes we are using newlib V1.14
       
   Changes:
 
     Version   Date        Author   Description
     ----------------------------------------------------------------------------------
      1        07.12.2006  MT       initial version
 
**************************************************************************************/

/*****************************************************************************/
/*! \file libc_mem_support.c                                                    
*   Operating system specific functions for libc memory support (malloc, ...)*/
/*****************************************************************************/
#include <sys/types.h>
#include <sys/reent.h>
#include <string.h> /* memset, etc */

#include <malloc.h>

/*****************************************************************************/
/* External variables from malloc implementation, to allow soft resets, where
*  static variables are not reset on rcX                                     */
/*****************************************************************************/
struct malloc_chunk
{
  size_t prev_size; /* Size of previous chunk (if free). */
  size_t size;      /* Size in bytes, including overhead. */
  struct malloc_chunk* fd;   /* double links -- used only if free. */
  struct malloc_chunk* bk;
}; 

#define NAV             128   /* number of bins */

#define DEFAULT_TRIM_THRESHOLD (128L * 1024L) 
#define DEFAULT_TOP_PAD        (0) 

typedef struct malloc_chunk* mbinptr;
#define SIZE_SZ        sizeof(size_t)
#define bin_at(i)      ((mbinptr)((char*)&(av_[2*(i) + 2]) - 2*SIZE_SZ))  
#define IAV(i)         bin_at(i), bin_at(i)

extern char*            __malloc_sbrk_base;
extern unsigned long    __malloc_max_sbrked_mem;
extern unsigned long    __malloc_max_total_mem;
extern struct mallinfo  __malloc_current_mallinfo;
extern unsigned long    __malloc_trim_threshold;
extern unsigned long    __malloc_top_pad;
extern mbinptr          __malloc_av_[NAV * 2 + 2];

/*****************************************************************************/

extern char  _heap_start; /* Defined by the linker */
extern char  _heap_end;   /* Defined by the linker */
static char* s_pchHeapEnd;
static char* s_pchHeapMax;

/*****************************************************************************/
/*! Initialize memory libC support                                           */
/*****************************************************************************/
void libc_mem_init()
{
  int iIdx;
  
  __malloc_sbrk_base      = (char*)-1;
  __malloc_max_sbrked_mem = 0;
  __malloc_max_total_mem  = 0;
  memset(&__malloc_current_mallinfo, 0, sizeof(__malloc_current_mallinfo));
  
  __malloc_trim_threshold = DEFAULT_TRIM_THRESHOLD;
  __malloc_top_pad        = DEFAULT_TOP_PAD; 

  /* first two elements always NULL */
  __malloc_av_[0] = NULL;
  __malloc_av_[1] = NULL;

  for(iIdx = 2; iIdx < sizeof(__malloc_av_) / sizeof(__malloc_av_[0]); iIdx += 2)
  {
    __malloc_av_[iIdx]     = (mbinptr)&__malloc_av_[iIdx - 2];
    __malloc_av_[iIdx + 1] = (mbinptr)&__malloc_av_[iIdx - 2];     
  }
 
  s_pchHeapEnd = &_heap_start;
  s_pchHeapMax = &_heap_end;
}

/*****************************************************************************/
/*! Non-Reentrant version of increase program data space
*   \param iIncr     Size to increase heap pointer 
*   \return previous heap pointer (NULL on error)                            */
/*****************************************************************************/
caddr_t sbrk(size_t iIncr) 
{  
  caddr_t pvBase = (caddr_t)-1;

  if( (s_pchHeapEnd + iIncr) <= s_pchHeapMax)
  {
    pvBase = s_pchHeapEnd;
    s_pchHeapEnd += iIncr;
  }

  return pvBase;
}

/*****************************************************************************/
/*! Reentrant version of increase program data space
*   \param ptReent   Pointer to global data block which holds errno  
*   \param iIncr     Size to de-/increase heap pointer 
*   \return previous heap pointer (-1 on error)                            */
/*****************************************************************************/
caddr_t _sbrk_r(struct _reent* ptReent, size_t iIncr) 
{
  caddr_t pvBase = (caddr_t)-1;

  //TODO: use interlocked increment here to be reentrant
  if( (s_pchHeapEnd + iIncr) <= s_pchHeapMax)
  {
    pvBase = s_pchHeapEnd;
    s_pchHeapEnd += iIncr;
  }

  return pvBase;
}

/****************************************************************************/
/*! Multithreading lock for malloc routines
 *   \param ptReent      Pointer to global data block which holds errno      */
/*****************************************************************************/
 void __malloc_lock(struct _reent* ptReent)
{
  //TODO: implement locking (Lock IRQ/mutex/spinlock etc.)
  
}
 
/****************************************************************************/
/*! Multithreading unlock for malloc routines
 *   \param ptReent      Pointer to global data block which holds errno      */
/*****************************************************************************/
 void __malloc_unlock(struct _reent* ptReent)
{
  //TODO: implement unlocking (Lock IRQ/mutex/spinlock etc.)
  
}
