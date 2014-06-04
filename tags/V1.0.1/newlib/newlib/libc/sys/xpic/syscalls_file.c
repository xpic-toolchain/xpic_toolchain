
/******************************************************************************/
/*  Includes                                                                  */
/******************************************************************************/

#if !defined( _CONSOLE_32_ )
#include <sys/types.h>
#include <sys/reent.h>
#include <sys/unistd.h>
#endif
#include <sys/stat.h>

#include <stdio.h>

/**
 * Opens a file (reentrant)
 * @param ptReent    Reentrancy structure 
 * @param pszFile    Name and path of the file to open 
 * @param iFlags     Flags for file open (see O_RDONLY, O_WRONLY, O_RDWR, ...
 * @param iMode      Sharing mode (see S_IRWXU, ...)
 * @return -1 on error (errno holds last error)
 */
int _open_r(struct _reent* ptReent, const char* pszFile, int iFlags, int iMode)
{
  return -1;
}


/**
 * Closes a file (non-reentrant)
 * @param ptReent  Reentrancy structure
 * @param iFile    File Handle 
 * @return -1 on error (errno holds last error)
 */
int _close_r(struct _reent* ptReent, int iFile)
{
  return -1;
}


/**
 * Queries state of a an open file (reentrant)
 * @param ptReent  Reentrancy structure
 * @param iFile    File Handle 
 * @param ptState  Returned state
 * @return -1 on error (errno holds last error)                   
 */
int _fstat_r(struct _reent* ptReent, int iFile, struct stat* ptState)
{
  return -1;
}

/**
 * Queries state of a file (reentrant)
 * @param ptReent  Reentrancy structure
 * @param szName   Name of the file to get status for 
 * @param ptState  Returned state
 * @return -1 on error (errno holds last error)                   
 */
int _stat_r(struct _reent* ptReent, const char* szName, struct stat* ptState)
{
  return -1;
}


/**
 * Queries if the file is a terminal (stdout)
 * @param iFile    File Handle 
 * @return !=0 if device is a terminal/tty device                 
 */
int _isatty_r(struct _reent* ptReent, int iFile)
{
  return 0 ;
}


/**
 * Establish a new name for an exisiting file (reentrant)
 * @param ptReent    Reentrancy structure 
 * @param szOldName  Old file name/path 
 * @param szNewName  New file name/path
 * @return -1 on error (errno holds error)                        
 */
int _link_r(struct _reent* ptReent, const char* szOldName, const char* szNewName)
{
  return -1;
}


/**
 * Removes a file's directory entry (reentrant)
 * @param ptReent    Reentrancy structure 
 * @param szName   Name of the file to delete 
 * @return -1 on error (errno holds error)                        
 */
int _unlink_r(struct _reent* ptReent, const char* szName)
{
  return -1;
}


/**
 * Reads from a file (reentrant)
 * @param ptReent    Reentrancy structure 
 * @param iFile      File handle 
 * @param pvBuffer   Data to write to file
 * @param iBytes     number of bytes to write
 * @return number of bytes successfully read (-1 on error)        
 */
_ssize_t _read_r(struct _reent* ptReent, int iFile, void* pvBuffer, size_t iBytes)
{
  return -1;
}


/**
 * Writes to a file (reentrant)
 * @param ptReent    Reentrancy structure 
 * @param iFile      File handle 
 * @param pvBuffer   Data to write to file
 * @param iBytes     number of bytes to write
 * @return number of bytes successfully written                   
 */
_ssize_t _write_r(struct _reent* ptReent, int iFile, const void* pvBuffer, size_t iBytes)
{
  // pretend to be successfull
  return iBytes;
}


/**
 * Set position in a file (reentrant)
 * @param ptReent    Reentrancy structure 
 * @param iFile      File handle 
 * @param iOffset    Offset to adjust
 * @param iDirection Direction of iOffset (SEEK_SET, SEEK_CUR, SEEK_END)
 * @return new file offset
 */
_off_t _lseek_r(struct _reent* ptReent, int iFile, _off_t iOffset, int iDirection)
{
  return -1;
}
