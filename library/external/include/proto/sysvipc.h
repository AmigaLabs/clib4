/* $VER: proto/sysvipc.h 51.4 (25.02.107) By Peter Bengtsson
 */
#ifndef PROTO_SYSVIPC_H
#define PROTO_SYSVIPC_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef LIBRARIES_SYSVIPC_H
#include <libraries/sysvipc.h>
#endif

/****************************************************************************/

#ifndef __NOLIBBASE__
 #ifndef __USE_BASETYPE__
  extern struct Library * SysVIPCBase;
 #else
  extern struct SysVIPCLibBase * SysVIPCBase;
 #endif /* __USE_BASETYPE__ */
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/sysvipc.h>
 #ifdef __USE_INLINE__
  #include <inline4/sysvipc.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_SYSVIPC_PROTOS_H
  #define CLIB_SYSVIPC_PROTOS_H 1
 #endif /* CLIB_SYSVIPC_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct SYSVIFace *ISysVIPC;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_SYSVIPC_PROTOS_H
  #include <clib/sysvipc_protos.h>
 #endif /* CLIB_SYSVIPC_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/sysvipc.h>
  #else
   #include <ppcinline/sysvipc.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/sysvipc_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/sysvipc_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_SYSVIPC_H */
