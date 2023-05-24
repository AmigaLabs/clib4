/*
 * $Id: socket_headers.h,v 1.14 2006-04-05 07:53:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#define _SOCKET_HEADERS_H

#ifndef _NETINET_IN_H
#include <netinet/in.h>
#endif /* _NETINET_IN_H */

#ifndef _SYS_SOCKET_H
#include <sys/socket.h>
#endif /* _SYS_SOCKET_H */

#ifndef _SYS_FILIO_H
#include <sys/filio.h>
#endif /* _SYS_FILIO_H */

#ifndef _ARPA_INET_H
#include <arpa/inet.h>
#endif /* _ARPA_INET_H */

#ifndef _NETDB_H
#include <netdb.h>
#endif /* _NETDB_H */

#ifndef _ARPA_NAMESER_H
#include <arpa/nameser.h>
#endif /* _ARPA_NAMESER_H */

#ifndef _RESOLV_H
#include <resolv.h>
#endif /* _RESOLV_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _SYSLOG_H
#include <syslog.h>
#endif

#ifndef _UN_H
#include <sys/un.h>
#endif

/* Code value. */
#define SBTB_CODE 1
#define SBTS_CODE 0x3FFF

/* Argument passed by reference or by value. */
#define SBTF_VAL 0x0000
#define SBTF_REF 0x8000

/* Get/Set (read/write) selection. */
#define SBTF_GET 0
#define SBTF_SET 1

/* Set a parameter, passing it by value. */
#define SBTM_SETVAL(code) (TAG_USER | SBTF_VAL | (((code) & SBTS_CODE) << SBTB_CODE) | SBTF_SET)

/* Get a parameter, passing it by reference. */
#define SBTM_GETREF(code) (TAG_USER | SBTF_REF | (((code) & SBTS_CODE) << SBTB_CODE) | SBTF_GET)

#define SBTC_BREAKMASK					1	/* Interrupt signal mask */
#define SBTC_LOGTAGPTR					11	/* Under which name log entries are filed */
#define SBTC_ERRNOLONGPTR				24	/* Pointer to errno, length == sizeof(errno) */
#define SBTC_HERRNOLONGPTR				25	/* 'h_errno' pointer (with sizeof(h_errno) == sizeof(long)) */
#define SBTC_CAN_SHARE_LIBRARY_BASES	51	/* Enable library base sharing among Processes */
#define SBTC_HAVE_SERVER_API			63	/* Whether or not the server API is supported. */
#define SBTC_ERROR_HOOK					68	/* Error hook pointer */

#define DECLARE_SOCKETBASE() \
	struct Library   UNUSED	*SocketBase  = __CLIB2->__SocketBase; \
	struct SocketIFace 		*ISocket	 = __CLIB2->__ISocket

extern int h_errno;

extern struct fd * __get_file_descriptor_socket(int socket_descriptor);
extern int64_t __socket_hook_entry(struct _clib2 *__clib2, struct fd * fd,struct file_action_message * fam);
extern void __set_h_errno(int new_h_errno);
extern int __select(int num_fds,fd_set *read_fds,fd_set *write_fds,fd_set *except_fds,struct timeval *timeout,ULONG * signal_mask_ptr);

/* Syslog stuff */
extern int syslog_openlog_flags;
extern int syslog_mask;
extern int syslog_facility;
extern char syslog_ident[35];
extern FILE *syslog_fd;

#include <interfaces/bsdsocket.h>

#define __socket(domain, type, protocol) __CLIB2->__ISocket->socket(domain , type , protocol)
#define __bind(sock, name, namelen) __CLIB2->__ISocket->bind(sock , name , namelen)
#define __listen(sock, backlog) __CLIB2->__ISocket->listen(sock , backlog)
#define __accept(sock, addr, addrlen) __CLIB2->__ISocket->accept(sock , addr , addrlen)
#define __connect(sock, name, namelen) __CLIB2->__ISocket->connect(sock , name , namelen)
#define __sendto(sock, buf, len, flags, to, tolen) __CLIB2->__ISocket->sendto(sock , buf , len , flags , to , tolen)
#define __send(sock, buf, len, flags) __CLIB2->__ISocket->send(sock , buf , len , flags)
#define __recvfrom(sock, buf, len, flags, addr, addrlen) __CLIB2->__ISocket->recvfrom(sock , buf , len , flags , addr , addrlen)
#define __recv(sock, buf, len, flags) __CLIB2->__ISocket->recv(sock , buf , len , flags)
#define __shutdown(sock, how) __CLIB2->__ISocket->shutdown(sock , how)
#define __setsockopt(sock, level, optname, optval, optlen) __CLIB2->__ISocket->setsockopt(sock , level , optname , optval , optlen)
#define __getsockopt(sock, level, optname, optval, optlen) __CLIB2->__ISocket->getsockopt(sock , level , optname , optval , optlen)
#define __getsockname(sock, name, namelen) __CLIB2->__ISocket->getsockname(sock , name , namelen)
#define __getpeername(sock, name, namelen) __CLIB2->__ISocket->getpeername(sock , name , namelen)
#define __IoctlSocket(sock, req, argp) __CLIB2->__ISocket->IoctlSocket(sock , req , argp)
#define __CloseSocket(sock) __CLIB2->__ISocket->CloseSocket(sock)
#define __WaitSelect(nfds, read_fds, write_fds, except_fds, timeout, signals) __CLIB2->__ISocket->WaitSelect(nfds , read_fds , write_fds , except_fds , timeout , signals)
#define __SetSocketSignals(int_mask, io_mask, urgent_mask) __CLIB2->__ISocket->SetSocketSignals(int_mask , io_mask , urgent_mask)
#define __getdtablesize() __CLIB2->__ISocket->getdtablesize()
#define __ObtainSocket(id, domain, type, protocol) __CLIB2->__ISocket->ObtainSocket(id , domain , type , protocol)
#define __ReleaseSocket(sock, id) __CLIB2->__ISocket->ReleaseSocket(sock , id)
#define __ReleaseCopyOfSocket(sock, id) __CLIB2->__ISocket->ReleaseCopyOfSocket(sock , id)
#define __Errno() __CLIB2->__ISocket->Errno()
#define __SetErrnoPtr(errno_ptr, size) __CLIB2->__ISocket->SetErrnoPtr(errno_ptr , size)
//#define __Inet_NtoA(ip) __CLIB2->__ISocket->Inet_NtoA(ip)
//#define __Inet_AtoN(cp, ip) __CLIB2->__ISocket->inet_aton(cp, ip)
//#define __inet_addr(cp) __CLIB2->__ISocket->inet_addr(cp)
//#define __Inet_LnaOf(in) __CLIB2->__ISocket->Inet_LnaOf(in)
//#define __Inet_NetOf(in) __CLIB2->__ISocket->Inet_NetOf(in)
//#define __Inet_MakeAddr(net, host) __CLIB2->__ISocket->Inet_MakeAddr(net , host)
//#define __inet_network(cp) __CLIB2->__ISocket->inet_network(cp)
//#define __gethostbyname(name) __CLIB2->__ISocket->gethostbyname(name)
//#define __gethostbyaddr(addr, len, type) __CLIB2->__ISocket->gethostbyaddr(addr , len , type)
#define __getnetbyname(name) __CLIB2->__ISocket->getnetbyname(name)
#define __getnetbyaddr(net, type) __CLIB2->__ISocket->getnetbyaddr(net , type)
//#define __getservbyname(name, proto) __CLIB2->__ISocket->getservbyname(name , proto)
//#define __getservbyport(port, proto) __CLIB2->__ISocket->getservbyport(port , proto)
#define __getprotobyname(name) __CLIB2->__ISocket->getprotobyname(name)
#define __getprotobynumber(proto) __CLIB2->__ISocket->getprotobynumber(proto)
#define __vsyslog(pri, msg, args) __CLIB2->__ISocket->vsyslog(pri , msg , args)
#define __syslog(pri, msg, tag1...) __CLIB2->__ISocket->syslog(pri , msg , ## tag1)
#define __Dup2Socket(old_socket, new_socket) __CLIB2->__ISocket->Dup2Socket(old_socket , new_socket)
#define __sendmsg(sock, msg, flags) __CLIB2->__ISocket->sendmsg(sock , msg , flags)
#define __recvmsg(sock, msg, flags) __CLIB2->__ISocket->recvmsg(sock , msg , flags)
#define __gethostname(name, namelen) __CLIB2->__ISocket->gethostname(name , namelen)
#define __gethostid() __CLIB2->__ISocket->gethostid()
#define __SocketBaseTagList(tags) __CLIB2->__ISocket->SocketBaseTagList(tags)
#define __SocketBaseTags(tag1...) __CLIB2->__ISocket->SocketBaseTags(## tag1)
#define __ProcessIsServer(pr) __CLIB2->__ISocket->ProcessIsServer(pr)
#define __getdomainname(buffer, size) __CLIB2->__ISocket->GetDefaultDomainName(buffer, size)
#define __setdomainname(buffer) __CLIB2->__ISocket->SetDefaultDomainName(buffer)

#endif /* _SOCKET_HEADERS_H */
