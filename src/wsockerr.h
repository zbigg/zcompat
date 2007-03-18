#ifdef WSAEINTR
case WSAEINTR:		strerr = "WSA: Interrupted system call";break;
#endif
#ifdef WSAEBADF
case WSAEBADF:		strerr = "WSA: Bad file number";break;
#endif
#ifdef WSAEACCES
case WSAEACCES:		strerr = "WSA: Permission denied";break;
#endif
#ifdef WSAEFAULT
case WSAEFAULT:		strerr = "WSA: Bad address passed";break;
#endif
#ifdef WSAEINVAL
case WSAEINVAL:		strerr = "WSA: Invalid parameter passed";break;
#endif
#ifdef WSAEMFILE
case WSAEMFILE:		strerr = "WSA: Too many open files";break;
#endif
#ifdef WSAEWOULDBLOCK
case WSAEWOULDBLOCK:	strerr = "WSA: Operation would block";break;
#endif
#ifdef WSAEINPROGRESS
case WSAEINPROGRESS:	strerr = "WSA: Operation is now in progress";break;
#endif
#ifdef WSAEALREADY
case WSAEALREADY:	strerr = "WSA: Operation is already in progress";break;
#endif
#ifdef WSAENOTSOCK
case WSAENOTSOCK:	strerr = "WSA: Socket operation on non-socket";break;
#endif
#ifdef WSAEDESTADDRREQ
case WSAEDESTADDRREQ:	strerr = "WSA: Destination address required";break;
#endif
#ifdef WSAEMSGSIZE
case WSAEMSGSIZE:	strerr = "WSA: Message is too long";break;
#endif
#ifdef WSAEPROTOTYPE
case WSAEPROTOTYPE:	strerr = "WSA: The protocol is of the wrong type for the socket";break;
#endif
#ifdef WSAENOPROTOOPT
case WSAENOPROTOOPT:	strerr = "WSA: The requested protocol is not available";break;
#endif
#ifdef WSAEPROTONOSUPPORT
case WSAEPROTONOSUPPORT:strerr = "WSA: The requested protocol is not supported";break;
#endif
#ifdef WSAESOCKTNOSUPPORT
case WSAESOCKTNOSUPPORT:strerr = "WSA: The specified socket type is not supported";break;
#endif
#ifdef WSAEOPNOTSUPP
case WSAEOPNOTSUPP:	strerr = "WSA: The specified operation is not supported";break;
#endif
#ifdef WSAEPFNOSUPPORT
case WSAEPFNOSUPPORT:	strerr = "WSA: The specified protocol family is not supported";break;
#endif
#ifdef WSAEAFNOSUPPORT
case WSAEAFNOSUPPORT:	strerr = "WSA: The specified address family is not supported";break;
#endif
#ifdef WSAEADDRINUSE
case WSAEADDRINUSE:	strerr = "WSA: The specified address is already in use";break;
#endif
#ifdef WSAEADDRNOTAVAIL
case WSAEADDRNOTAVAIL:	strerr = "WSA: The requested address is unassignable";break;
#endif
#ifdef WSAENETDOWN
case WSAENETDOWN:	strerr = "WSA: The network appears to be down";break;
#endif
#ifdef WSAENETUNREACH
case WSAENETUNREACH:	strerr = "WSA: The network is unreachable";break;
#endif
#ifdef WSAENETRESET
case WSAENETRESET:	strerr = "WSA: The network dropped the connection on reset";break;
#endif
#ifdef WSAECONNABORTED
case WSAECONNABORTED:	strerr = "WSA: Software caused a connection abort";break;
#endif
#ifdef WSAECONNRESET
case WSAECONNRESET:	strerr = "WSA: Connection was reset by peer";break;
#endif
#ifdef WSAENOBUFS
case WSAENOBUFS:	strerr = "WSA: Out of buffer space";break;
#endif
#ifdef WSAEISCONN
case WSAEISCONN:	strerr = "WSA: Socket is already connected";break;
#endif
#ifdef WSAENOTCONN
case WSAENOTCONN:	strerr = "WSA: Socket is not presently connected";break;
#endif
#ifdef WSAESHUTDOWN
case WSAESHUTDOWN:	strerr = "WSA: Can't send data because socket is shut down";break;
#endif
#ifdef WSAETOOMANYREFS
case WSAETOOMANYREFS:	strerr = "WSA: Too many references;break; unable to splice";break;
#endif
#ifdef WSAETIMEDOUT
case WSAETIMEDOUT:	strerr = "WSA: The connection timed out";break;
#endif
#ifdef WSAECONNREFUSED
case WSAECONNREFUSED:	strerr = "WSA: The connection was refused";break;
#endif
#ifdef WSAELOOP
case WSAELOOP:		strerr = "WSA: Too many symbolic link levels";break;
#endif
#ifdef WSAENAMETOOLONG
case WSAENAMETOOLONG:	strerr = "WSA: File name is too long";break;
#endif
#ifdef WSAEHOSTDOWN
case WSAEHOSTDOWN:	strerr = "WSA: The host appears to be down";break;
#endif
#ifdef WSAEHOSTUNREACH
case WSAEHOSTUNREACH:	strerr = "WSA: The host is unreachable";break;
#endif
#ifdef WSAENOTEMPTY
case WSAENOTEMPTY:	strerr = "WSA: The directory is not empty";break;
#endif
#ifdef WSAEPROCLIM
case WSAEPROCLIM:	strerr = "WSA: There are too many processes";break;
#endif
#ifdef WSAEUSERS
case WSAEUSERS:		strerr = "WSA: There are too many users";break;
#endif
#ifdef WSAEDQUOT
case WSAEDQUOT:		strerr = "WSA: The disk quota is exceeded";break;
#endif
#ifdef WSAESTALE
case WSAESTALE:		strerr = "WSA: Bad NFS file handle";break;
#endif
#ifdef WSAEREMOTE
case WSAEREMOTE:	strerr = "WSA: There are too many levels of remote in the path";break;
#endif
#ifdef WSAEDISCO
/*case WSAEDISCO:	strerr = "WSA: Disconnect";break;  UNDEFINED */
#endif
#ifdef WSASYSNOTREADY
case WSASYSNOTREADY:	strerr = "WSA: Network sub-system is not ready or unusable";break;
#endif
#ifdef WSAVERNOTSUPPORTED
case WSAVERNOTSUPPORTED:strerr = "WSA: The requested version is not supported";break;
#endif
#ifdef WSANOTINITIALISED
case WSANOTINITIALISED: strerr = "WSA: Socket system is not initialized";break;
#endif
#ifdef WSAHOST_NOT_FOUND
case WSAHOST_NOT_FOUND: strerr = "WSA: The host was not found";break;
#endif
#ifdef WSATRY_AGAIN
case WSATRY_AGAIN:	strerr = "WSA: Non-authoritative host not found";break;
#endif
#ifdef WSANO_RECOVERY
case WSANO_RECOVERY:	strerr = "WSA: Non-recoverable error";break;
#endif
#ifdef WSANO_DATA
case WSANO_DATA:	strerr = "WSA: No information of the requested type was available";break;
#endif
default:		strerr = "WSA: Unknown error";break;
