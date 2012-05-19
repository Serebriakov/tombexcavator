#include <ostream>
#include "bsw/ipc/network/network.hpp"

#include "thirdparty/predef.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef _WIN32_WCE
#include <errno.h>
#endif

/* Include system network headers */
#if defined(PREDEF_OS_WINDOWS)
#define BSW_USE_W32_SOCKETS
#if defined(_WIN64) || defined(PREDEF_COMPILER_MINGW32) || (defined(_MSC_VER) && (_MSC_VER == 1600))
#if defined(_WIN32_WINNT)
#undef _WIN32_WINNT
#endif

#define _WIN32_WINNT 0x0501
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <winsock.h>
/* NOTE: windows socklen_t is signed
 * and is defined only for winsock2. */
typedef int socklen_t;
#endif /* W64 */
#include <iphlpapi.h>
#else /* UNIX */
#include <sys/types.h>
#include <signal.h>
#if defined(PREDEF_OS_FREEBSD)
#include <sys/socket.h>
#endif
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#if !defined(PREDEF_OS_BEOS)
#include <arpa/inet.h>
#endif
#if defined(PREDEF_OS_LINUX) /* FIXME: what other platforms have this? */
#include <netinet/tcp.h>
#endif
#include <sys/socket.h>
#include <net/if.h>
#include <netdb.h>
#endif /* WIN32 */

/* System-dependent definitions */
#ifndef BSW_USE_W32_SOCKETS
#if defined(PREDEF_OS_OS2)
#define closesocket     soclose
#else  /* !__OS2__ */
#define closesocket	close
#endif /* __OS2__ */
#define SOCKET	int
#define INVALID_SOCKET	-1
#define SOCKET_ERROR	-1
#endif /* BSW_USE_W32_SOCKETS */

#ifdef BSW_USE_W32_SOCKETS
#define os_get_last_error WSAGetLastError
#define os_set_last_error WSASetLastError
#ifndef EINTR
#define EINTR WSAEINTR
#endif
#else
int os_get_last_error (void);
void os_set_last_error (int err);
#endif

#if defined(BSW_USE_W32_SOCKETS)


#define IN6ADDRSZ       16
#define INADDRSZ         4
#define INT16SZ          2
#if defined(EAFNOSUPPORT)
#undef EAFNOSUPPORT
#endif

#define EAFNOSUPPORT     WSAEAFNOSUPPORT

static int      inet_pton4(const char *src, unsigned char *dst);
#ifdef ENABLE_IPV6
static int      inet_pton6(const char *src, unsigned char *dst);
#endif

/* int
 * inet_pton(af, src, dst)
 *      convert from presentation format (which usually means ASCII printable)
 *      to network format (which is usually some kind of binary format).
 * return:
 *      1 if the address was valid for the specified address family
 *      0 if the address wasn't valid (`dst' is untouched in this case)
 *      -1 if some other error occurred (`dst' is untouched in this case, too)
 * notice:
 *      On Windows we store the error in the thread errno, not
 *      in the winsock error code. This is to avoid losing the
 *      actual last winsock error. So use macro ERRNO to fetch the
 *      errno this function sets when returning (-1), not SOCKERRNO.
 * author:
 *      Paul Vixie, 1996.
 */
static int inet_pton(int af, const char *src, void *dst)
{
  switch (af) {
  case AF_INET:
    return (inet_pton4(src, (unsigned char *)dst));
#ifdef ENABLE_IPV6
  case AF_INET6:
    return (inet_pton6(src, (unsigned char *)dst));
#endif
  default:
    os_set_last_error (EAFNOSUPPORT);
    return (-1);
  }
  /* NOTREACHED */
}

/* int
 * inet_pton4(src, dst)
 *      like inet_aton() but without all the hexadecimal and shorthand.
 * return:
 *      1 if `src' is a valid dotted quad, else 0.
 * notice:
 *      does not touch `dst' unless it's returning 1.
 * author:
 *      Paul Vixie, 1996.
 */
static int
inet_pton4(const char *src, unsigned char *dst)
{
  static const char digits[] = "0123456789";
  int saw_digit, octets, ch;
  unsigned char tmp[INADDRSZ], *tp;

  saw_digit = 0;
  octets = 0;
  tp = tmp;
  *tp = 0;
  while((ch = *src++) != '\0') {
    const char *pch;

    if((pch = strchr(digits, ch)) != NULL) {
      unsigned int val = *tp * 10 + (unsigned int)(pch - digits);

      if(saw_digit && *tp == 0)
        return (0);
      if(val > 255)
        return (0);
      *tp = (unsigned char)val;
      if(! saw_digit) {
        if(++octets > 4)
          return (0);
        saw_digit = 1;
      }
    }
    else if(ch == '.' && saw_digit) {
      if(octets == 4)
        return (0);
      *++tp = 0;
      saw_digit = 0;
    }
    else
      return (0);
  }
  if(octets < 4)
    return (0);
  memcpy(dst, tmp, INADDRSZ);
  return (1);
}

#ifdef ENABLE_IPV6
/* int
 * inet_pton6(src, dst)
 *      convert presentation level address to network order binary form.
 * return:
 *      1 if `src' is a valid [RFC1884 2.2] address, else 0.
 * notice:
 *      (1) does not touch `dst' unless it's returning 1.
 *      (2) :: in a full address is silently ignored.
 * credit:
 *      inspired by Mark Andrews.
 * author:
 *      Paul Vixie, 1996.
 */
static int
inet_pton6(const char *src, unsigned char *dst)
{
  static const char xdigits_l[] = "0123456789abcdef",
    xdigits_u[] = "0123456789ABCDEF";
  unsigned char tmp[IN6ADDRSZ], *tp, *endp, *colonp;
  const char *xdigits, *curtok;
  int ch, saw_xdigit;
  size_t val;

  memset((tp = tmp), 0, IN6ADDRSZ);
  endp = tp + IN6ADDRSZ;
  colonp = NULL;
  /* Leading :: requires some special handling. */
  if(*src == ':')
    if(*++src != ':')
      return (0);
  curtok = src;
  saw_xdigit = 0;
  val = 0;
  while((ch = *src++) != '\0') {
    const char *pch;

    if((pch = strchr((xdigits = xdigits_l), ch)) == NULL)
      pch = strchr((xdigits = xdigits_u), ch);
    if(pch != NULL) {
      val <<= 4;
      val |= (pch - xdigits);
      if(++saw_xdigit > 4)
        return (0);
      continue;
    }
    if(ch == ':') {
      curtok = src;
      if(!saw_xdigit) {
        if(colonp)
          return (0);
        colonp = tp;
        continue;
      }
      if(tp + INT16SZ > endp)
        return (0);
      *tp++ = (unsigned char) (val >> 8) & 0xff;
      *tp++ = (unsigned char) val & 0xff;
      saw_xdigit = 0;
      val = 0;
      continue;
    }
    if(ch == '.' && ((tp + INADDRSZ) <= endp) &&
        inet_pton4(curtok, tp) > 0) {
      tp += INADDRSZ;
      saw_xdigit = 0;
      break;    /* '\0' was seen by inet_pton4(). */
    }
    return (0);
  }
  if(saw_xdigit) {
    if(tp + INT16SZ > endp)
      return (0);
    *tp++ = (unsigned char) (val >> 8) & 0xff;
    *tp++ = (unsigned char) val & 0xff;
  }
  if(colonp != NULL) {
    /*
     * Since some memmove()'s erroneously fail to handle
     * overlapping regions, we'll do the shift by hand.
     */
    const ssize_t n = tp - colonp;
    ssize_t i;

    if(tp == endp)
      return (0);
    for(i = 1; i <= n; i++) {
      *(endp - i) = *(colonp + n - i);
      *(colonp + n - i) = 0;
    }
    tp = endp;
  }
  if(tp != endp)
    return (0);
  memcpy(dst, tmp, IN6ADDRSZ);
  return (1);
}
#endif /* ENABLE_IPV6 */

#endif



static bool network_started = false;

#ifndef BSW_USE_W32_SOCKETS
int os_get_last_error (void)
{
  return errno;
}
// --------------------------------------------------------
void os_set_last_error (int err)
{
  errno = err;
}
#endif

namespace bsw
{
  namespace ipc
  {
    namespace network
    {
      const uint32_t BSW_INADDR_ANY = 0x00000000;
      const uint32_t BSW_INADDR_NONE = 0xFFFFFFFF;
      const uint32_t BSW_INADDR_LOOPBACK = 0x7f000001;
      const uint32_t BSW_INADDR_BROADCAST = 0xFFFFFFFF;

      void init () THROW_SPEC (std::runtime_error)
      {
	if (!network_started)
	  {
#ifdef BSW_USE_W32_SOCKETS
	    /* Start up the windows networking */
	    WORD version_wanted = MAKEWORD(1,1);
	    WSADATA wsaData;

	    if ( WSAStartup(version_wanted, &wsaData) != 0 ) 
	      {
		throw std::runtime_error ("Couldn't initialize Winsock 1.1");
	      }
#else
	    /* SIGPIPE is generated when a remote socket is closed */
	    void (*handler)(int);
	    handler = signal(SIGPIPE, SIG_IGN);
	    if (handler != SIG_DFL ) 
	      {
		signal(SIGPIPE, handler);
	      }
#endif
	  }
	network_started = true;
      }
      // -----------------------------------------------------------
      void quit () NO_THROW_SPEC
      {
	if (!network_started)
	  {
	    return;
	  }
#ifdef BSW_USE_W32_SOCKETS
	/* Clean up windows networking */
	if (WSACleanup() == SOCKET_ERROR) 
	  {
	    if (WSAGetLastError() == WSAEINPROGRESS) 
	      {
#ifndef _WIN32_WCE
		WSACancelBlockingCall ();
#endif
		WSACleanup();
	      }
	  }
#else
	/* Restore the SIGPIPE handler */
	void (*handler)(int);
	handler = signal (SIGPIPE, SIG_DFL);
	if (handler != SIG_IGN) 
	  {
	    signal(SIGPIPE, handler);
	  }
#endif
      }
      // -----------------------------------------------------------
      bool resolve_host (const std::string& host, ipv4_addr_t& ip_addr, err_code_t& err_code) NO_THROW_SPEC
      {
	return resolve_host (host.c_str (), ip_addr, err_code);
      }
      // -----------------------------------------------------------
      bool resolve_host (const char* host, ipv4_addr_t& ip_addr, err_code_t& err_code) NO_THROW_SPEC
      {
	assert (host);
	err_code = 0;
	struct in_addr in;
	int s = inet_pton (AF_INET, host, &in);

	if (s <= 0)
	  {
	    if (s < 0)
	      {
		err_code = os_get_last_error ();
		return false;
	      }
	    struct hostent *hp;
	    ipv4_addr_t h;
	    hp = gethostbyname(host);
	    if (hp) 
	      {
		assert (hp->h_length == sizeof (h));
		memcpy(&h, hp->h_addr,hp->h_length);
		ip_addr = ntohl (h);
		return true;
	      } 
	    else
	      {
		err_code = os_get_last_error ();
		return false;
	      }
	  }
	ip_addr = ntohl (in.s_addr);
	return true;
      }
      // -----------------------------------------------------------
      bool resolve_ip (ipv4_addr_t ip, std::string& host, err_code_t& err_code) NO_THROW_SPEC
      {
	struct sockaddr_in sock_in;
	char hostname[NI_MAXHOST];
	char servInfo[NI_MAXSERV];

	sock_in.sin_family = AF_INET;
	sock_in.sin_addr.s_addr = htonl (ip);
	sock_in.sin_port = 0;

	int rc = getnameinfo((struct sockaddr *) &sock_in,
			     sizeof (struct sockaddr),
			     hostname,
			     NI_MAXHOST, servInfo, NI_MAXSERV, NI_NUMERICSERV);
	if (rc)
	  {
	    err_code = os_get_last_error ();
	    return false;
	  }
	err_code = 0;
	host.assign (hostname);
	return true;
      }
      // -----------------------------------------------------------
      bool get_local_addresses (std::vector <ip_address_s>& addresses, err_code_t& err_code)
      {
	err_code = 0;
#ifdef SIOCGIFCONF
	/* Defined on Mac OS X */
#ifndef _SIZEOF_ADDR_IFREQ
#define _SIZEOF_ADDR_IFREQ sizeof
#endif
	SOCKET sock;
	struct ifconf conf;
	char data[4096];
	struct ifreq *ifr;
	struct sockaddr_in *sock_addr;
	
	
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) 
	  {
	    err_code = os_get_last_error ();
	    return 0;
	  }

	conf.ifc_len = sizeof(data);
	conf.ifc_buf = (caddr_t) data;
	if (ioctl(sock, SIOCGIFCONF, &conf) < 0) 
	  {
	    err_code = os_get_last_error ();
	    closesocket(sock);
	    return false;
	  }

	ifr = (struct ifreq*)data;
	while ((char*)ifr < data+conf.ifc_len) 
	  {
	    if (ifr->ifr_addr.sa_family == AF_INET) 
	      {
		sock_addr = (struct sockaddr_in*)&ifr->ifr_addr;
		ip_address_s a;
		a.host = ntohl (sock_addr->sin_addr.s_addr);
		a.port = ntohs (sock_addr->sin_port);
		addresses.push_back (a);
	      }
	    ifr = (struct ifreq*)((char*)ifr + _SIZEOF_ADDR_IFREQ(*ifr));
	  }
	closesocket(sock);
#elif defined(PREDEF_OS_WINDOWS)
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter;
	PIP_ADDR_STRING pAddress;
	DWORD dwRetVal = 0;
	ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);

	pAdapterInfo = (IP_ADAPTER_INFO *) malloc(sizeof (IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL) 
	  {
	    err_code = os_get_last_error ();
	    return false;
	  }

	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == ERROR_BUFFER_OVERFLOW) 
	  {
	    pAdapterInfo = (IP_ADAPTER_INFO *) realloc(pAdapterInfo, ulOutBufLen);
	    if (pAdapterInfo == NULL) 
	      {
		err_code = os_get_last_error ();
		return false;
	      }
	    dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
	  }

	if (dwRetVal == NO_ERROR) 
	  {
	    for (pAdapter = pAdapterInfo; pAdapter; pAdapter = pAdapter->Next) 
	      {
		for (pAddress = &pAdapterInfo->IpAddressList; pAddress; pAddress = pAddress->Next) 
		  {
		    ip_address_s a;
		    a.host = inet_addr(pAddress->IpAddress.String);
		    a.port = 0;
		    addresses.push_back (a);
		  }
	      }
	  }
	free(pAdapterInfo);
#endif
	return true;
      }
      // =====================================================================
      // tcp
      struct tcp_socket_s
      {
	int ready;
	SOCKET channel;
	ip_address_s remoteAddress;
	ip_address_s localAddress;
	int sflag;
      };
      
      tcp_socket_s* tcp_open (const ip_address_s& addr)
      {
	tcp_socket_s* sock;
	struct sockaddr_in sock_addr;


	sock = new tcp_socket_s;
	sock->channel = socket(AF_INET, SOCK_STREAM, 0);
	if (sock->channel == INVALID_SOCKET) 
	  {
	    throw std::runtime_error ("Couldn't create socket");
	  }
	/* Connect to remote, or bind locally, as appropriate */
	if ( (addr.host != INADDR_NONE) && (addr.host != INADDR_ANY) ) 
	  {
	    // #########  Connecting to remote
	    memset(&sock_addr, 0, sizeof(sock_addr));
	    sock_addr.sin_family = AF_INET;
	    sock_addr.sin_addr.s_addr = htonl (addr.host);
	    sock_addr.sin_port = htons (addr.port);
	    
	    /* Connect to the remote host */
	    if (connect(sock->channel, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) == SOCKET_ERROR ) 
	      {
		throw std::runtime_error ("Couldn't connect to remote host");
	      }
	    sock->sflag = 0;
	  } 
	else 
	  {
	    // ##########  Binding locally
	    memset (&sock_addr, 0, sizeof(sock_addr));
	    sock_addr.sin_family = AF_INET;
	    sock_addr.sin_addr.s_addr = INADDR_ANY;
	    sock_addr.sin_port = addr.port;
#if !defined(PREDEF_OS_WINDOWS)
	    /* allow local address reuse */
	    { 
	      int yes = 1;
	      setsockopt(sock->channel, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes));
	    }
#endif
	    /* Bind the socket for listening */
	    if (bind(sock->channel, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) == SOCKET_ERROR) 
	      {
		throw std::runtime_error ("Couldn't bind to local port");
	      }
	    if (listen (sock->channel, 5) == SOCKET_ERROR) 
	      {
		throw std::runtime_error ("Couldn't listen to local port");
	      }
#if defined(PREDEF_OS_BEOS) && defined(SO_NONBLOCK)
	    /* On BeOS r5 there is O_NONBLOCK but it's for files only */
	    {
	      long b = 1;
	      setsockopt(sock->channel, SOL_SOCKET, SO_NONBLOCK, &b, sizeof(b));
	    }
#elif defined(O_NONBLOCK)
	    {
	      fcntl(sock->channel, F_SETFL, O_NONBLOCK);
	    }
#elif defined(PREDEF_OS_WINDOWS)
	    {
	      unsigned long mode = 1;
	      ioctlsocket (sock->channel, FIONBIO, &mode);
	    }
#elif defined(PREDEF_OS_OS2)
	    {
	      int dontblock = 1;
	      ioctl(sock->channel, FIONBIO, &dontblock);
	    }
#else
#warning How do we set non-blocking mode on other operating systems?
#endif
	    sock->sflag = 1;
	  }
	sock->ready = 0;

#ifdef TCP_NODELAY
	// Set the nodelay TCP option for real-time games 
	{ 
	  int yes = 1;
	  setsockopt(sock->channel, IPPROTO_TCP, TCP_NODELAY, (char*)&yes, sizeof(yes));
	}
#endif /* TCP_NODELAY */
	/* Fill in the channel host address */
	sock->remoteAddress.host = sock_addr.sin_addr.s_addr;
	sock->remoteAddress.port = sock_addr.sin_port;

	return(sock);
      }
      // ---------------------------------------------------------------------
      tcp_socket_s* tcp_accept (tcp_socket_s* server)
      {
	tcp_socket_s* sock;
	struct sockaddr_in sock_addr;
	socklen_t sock_alen;

	/* Only server sockets can accept */
	if (!server->sflag ) 
	  {
	    throw std::runtime_error ("Only server sockets can accept()");
	  }
	server->ready = 0;

	sock = new tcp_socket_s;

	// Accept a new TCP connection on a server socket
	sock_alen = sizeof(sock_addr);
	sock->channel = accept(server->channel, (struct sockaddr *)&sock_addr,
			       &sock_alen);
	if (sock->channel == INVALID_SOCKET) 
	  {
	    throw std::runtime_error ("accept() failed");
	  }
#if defined(PREDEF_OS_WIN32)
	{
	  /* passing a zero value, socket mode set to block on */
	  unsigned long mode = 0;
	  ioctlsocket (sock->channel, FIONBIO, &mode);
	}
#elif defined(O_NONBLOCK)
	{
	  int flags = fcntl(sock->channel, F_GETFL, 0);
	  fcntl(sock->channel, F_SETFL, flags & ~O_NONBLOCK);
	}
#endif 
	sock->remoteAddress.host = ntohl (sock_addr.sin_addr.s_addr);
	sock->remoteAddress.port = ntohs (sock_addr.sin_port);
	
	sock->sflag = 0;
	sock->ready = 0;

	return(sock);
      }
      // ---------------------------------------------------------------------
      ip_address_s  get_peer_address (const tcp_socket_s* sock)
      {
	if (sock->sflag)
	  {
	    return ip_address_s ();
	  }
	return(sock->remoteAddress);
      }
      // ---------------------------------------------------------------------
      int tcp_send (tcp_socket_s* sock, const void* datap, int len)
      {
	const uint8_t *data = (const uint8_t *)datap;	// For pointer arithmetic
	int sent, left;

	// Server sockets are for accepting connections only
	if ( sock->sflag ) 
	  {
	    throw std::runtime_error ("Server sockets cannot send");
	  }

	// Keep sending data until it's sent or an error occurs 
	left = len;
	sent = 0;
	os_set_last_error (0);
	do 
	  {
	    len = send(sock->channel, (const char *) data, left, 0);
	    if (len > 0) 
	      {
		sent += len;
		left -= len;
		data += len;
	      }
	  } while ( (left > 0) && ((len > 0) || (os_get_last_error () == EINTR)) );

	return(sent);
      }
      // ---------------------------------------------------------------------
      int tcp_recv (tcp_socket_s* sock, void* data, int maxlen)
      {
	int len;

	/* Server sockets are for accepting connections only */
	if (sock->sflag) 
	  {
	    throw std::runtime_error ("Server sockets cannot receive");
	  }

	os_set_last_error (0);
	do 
	  {
	    len = recv (sock->channel, (char *) data, maxlen, 0);
	  } while (os_get_last_error () == EINTR);

	sock->ready = 0;
	return(len);
      }
      // ---------------------------------------------------------------------
      void tcp_close (tcp_socket_s* sock)
      {
	if ( sock != NULL ) 
	  {
	    if ( sock->channel != INVALID_SOCKET ) 
	      {
		closesocket(sock->channel);
	      }
	    delete sock;
	  }
      }
      // ---------------------------------------------------------------------
      // udp
      struct udp_socket_s
      {
	int ready;
	SOCKET channel;
	ip_address_s address;
      };
      // ------------------------------------------------------------------------------------
      udp_socket_s* udp_open ()
      {
	udp_socket_s* sock;
	struct sockaddr_in sock_addr;

	sock = new udp_socket_s;
	memset(sock, 0, sizeof(*sock));
	memset(&sock_addr, 0, sizeof(sock_addr));
	sock->channel = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock->channel == INVALID_SOCKET) 
	  {
	    throw std::runtime_error ("Couldn't create socket");
	  }
	
#ifdef SO_BROADCAST
	// Allow LAN broadcasts with the socket
	{ 
	  int yes = 1;
	  setsockopt(sock->channel, SOL_SOCKET, SO_BROADCAST, (char*)&yes, sizeof(yes));
	}
#endif
#ifdef IP_ADD_MEMBERSHIP
	/* Receive LAN multicast packets on 224.0.0.1
	   This automatically works on Mac OS X, Linux and BSD, but needs
	   this code on Windows.
	*/
	/* A good description of multicast can be found here:
	   http://www.docs.hp.com/en/B2355-90136/ch05s05.html
	*/
	/* FIXME: Add support for joining arbitrary groups to the API */
	{
	  struct ip_mreq	g;

	  g.imr_multiaddr.s_addr = inet_addr("224.0.0.1");
	  g.imr_interface.s_addr = INADDR_ANY;
	  setsockopt(sock->channel, IPPROTO_IP, IP_ADD_MEMBERSHIP,
		     (char*)&g, sizeof(g));
	}
#endif
	return sock;
      }
      // -------------------------------------------------------------------------
      int udp_bind (udp_socket_s* sock, uint16_t port)
      {
	struct sockaddr_in sock_addr;
	socklen_t sock_len;

	memset(&sock_addr, 0, sizeof(sock_addr));

	// Bind locally, if appropriate
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = htonl (INADDR_ANY);
	sock_addr.sin_port = htons (port);

	// Bind the socket for listening 
	if (bind (sock->channel, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) == SOCKET_ERROR ) 
	  {
	    throw std::runtime_error ("Couldn't bind to local port");
	  }
	// Get the bound address and port
	sock_len = sizeof(sock_addr);
	if (getsockname (sock->channel, (struct sockaddr *)&sock_addr, &sock_len) < 0 ) 
	  {
	    throw std::runtime_error ("couldnt get socket address");
	  }

	// Fill in the channel host address
	sock->address.host = ntohl (sock_addr.sin_addr.s_addr);
	sock->address.port = ntohs (sock_addr.sin_port);
	return 1;
      }
      // -------------------------------------------------------------------------
      int udp_send (udp_socket_s* sock, const ip_address_s& addr, const void* data, int len)
      {
	
	struct sockaddr_in sock_addr;
	int sock_len = sizeof (sock_addr);

	memset(&sock_addr, 0, sizeof(sock_addr));
	
	sock_addr.sin_addr.s_addr = htonl (addr.host);
	sock_addr.sin_port = htons (addr.port);
	sock_addr.sin_family = AF_INET;
	int status = sendto(sock->channel, 
			    (char*)data, len, 0,
			    (struct sockaddr *)&sock_addr,
			    sock_len);
	if (status >= 0)
	  {
	    return 1;
	  }
	return 0;
      }
      // -------------------------------------------------------------------------
      int udp_recv (udp_socket_s* sock, ip_address_s& addr, void* data, int maxlen)
      {
	socklen_t sock_len;
	struct sockaddr_in sock_addr;

	sock_len = sizeof(sock_addr);
	int status = recvfrom(sock->channel,
			      (char*)data, maxlen, 0,
			      (struct sockaddr *)&sock_addr,
			      &sock_len);
	addr.host = ntohl (sock_addr.sin_addr.s_addr);
	addr.port = ntohs (sock_addr.sin_port);
	if (status >= 0)
	  {
	    return status;
	  }
	return 0;
      }
      // -------------------------------------------------------------------------
      void udp_close (udp_socket_s* socket)
      {
	if (socket)
	  {
	    closesocket (socket->channel);
	  }
	delete socket;
      }
      // -------------------------------------------------------------------------
      //select

      struct net_socket_s
      {
	int ready;
	SOCKET channel;
      };

      struct socket_set_s
      {
	int numsockets;
	int maxsockets;
	net_socket_s** sockets;
      };
      // -------------------------------------------------------------
      socket_set_s* allocate_socket_set (int maxsockets)
      {
	int i;

	socket_set_s* set = (socket_set_s*)malloc(sizeof(*set));
	if ( set != NULL ) 
	  {
	    set->numsockets = 0;
	    set->maxsockets = maxsockets;
	    set->sockets = (struct net_socket_s **)malloc (maxsockets*sizeof(*set->sockets));
	    if ( set->sockets != NULL ) 
	      {
		for ( i=0; i<maxsockets; ++i ) 
		  {
		    set->sockets[i] = NULL;
		  }
	      } 
	    else 
	      {
		free(set);
		set = NULL;
	      }
	  }
	return set;
      }
      // ----------------------------------------------------------
      void free_socket_set (socket_set_s* set)
      {
	if (set) 
	  {
	    free(set->sockets);
	    free(set);
	  }
      }
      // ----------------------------------------------------------
      static int add_socket (net_socket_s* s, socket_set_s* set)
      {
	if (s != NULL) 
	  {
	    if (set->numsockets == set->maxsockets) 
	      {
		return -1;
	      }
	    set->sockets[set->numsockets++] = s;
	  }
	return set->numsockets;
      }
      // ----------------------------------------------------------
      int add_socket (const udp_socket_s* socket, socket_set_s* set)
      {
	return add_socket ((net_socket_s*) (socket), set);
      }
      // ----------------------------------------------------------
      int add_socket (const tcp_socket_s* socket, socket_set_s* set)
      {
	return add_socket ((net_socket_s*) (socket), set);
      }
      // ----------------------------------------------------------
      static int rm_socket (net_socket_s* sock, socket_set_s* set)
      {
	int i;

	if (sock != NULL) 
	  {
	    for (i=0; i<set->numsockets; ++i) 
	      {
		if (set->sockets[i] == sock) 
		  {
		    break;
		  }
	      }
	    if (i == set->numsockets) 
	      {
		return -1;
	      }
	    --set->numsockets;
	    for ( ; i<set->numsockets; ++i) 
	      {
		set->sockets[i] = set->sockets[i+1];
	      }
	  }
	return set->numsockets;
      }
      // ----------------------------------------------------------
      int rm_socket (const udp_socket_s* socket, socket_set_s* set)
      {
	return rm_socket ((net_socket_s*) (socket), set);
      }
      // ----------------------------------------------------------
      int rm_socket (const tcp_socket_s* socket, socket_set_s* set)
      {
	return rm_socket ((net_socket_s*) (socket), set);
      }
      // ----------------------------------------------------------
      int check_sockets (socket_set_s* set, uint32_t timeout_ms)
      {
	int i;
	SOCKET maxfd;
	int retval;
	struct timeval tv;
	fd_set mask;

	/* Find the largest file descriptor */
	maxfd = 0;
	for (i=set->numsockets-1; i>=0; --i) 
	  {
	    if (set->sockets[i]->channel > maxfd) 
	      {
		     maxfd = set->sockets[i]->channel;
	      }
	  }

	/* Check the file descriptors for available data */
	do {
	  os_set_last_error (0);
		
	  /* Set up the mask of file descriptors */
	  FD_ZERO(&mask);
	  for (i=set->numsockets-1; i>=0; --i) 
	    {
	      FD_SET(set->sockets[i]->channel, &mask);
	    }

	  /* Set up the timeout */

	  tv.tv_usec = (timeout_ms%1000)*1000;
		
	  /* Look! */
	  retval = select(maxfd+1, &mask, NULL, NULL, &tv);
	} while (os_get_last_error () == EINTR);

	/* Mark all file descriptors ready that have data available */
	if ( retval > 0 ) 
	  {
	    for (i=set->numsockets-1; i>=0; --i) 
	      {
		if ( FD_ISSET(set->sockets[i]->channel, &mask) ) 
		  {
		    set->sockets[i]->ready = 1;
		  }
	      }
	  }
	return(retval);
      }
      // ----------------------------------------------------------
      bool is_ready (const udp_socket_s* socket)
      {
	return (socket && socket->ready);
      }
      // ----------------------------------------------------------
      bool is_ready (const tcp_socket_s* socket)
      {
	return (socket && socket->ready);
      }
      // ==========================================================
      ip_address_s::ip_address_s ()
	: host (0),
	  port (0)
      {
	
      }
      
      #define IPCOMP(addr, n) ((addr >> (24 - 8 * n)) & 0xFF)

      std::ostream& operator << (std::ostream& os, const ip_address_s& addr)
      {
	os  << IPCOMP (addr.host, 0) << "."
	    << IPCOMP (addr.host, 1) << "."
	    << IPCOMP (addr.host, 2) << "."
	    << IPCOMP (addr.host, 3);

	if (addr.port)
	  {
	    os << ":" << addr.port;
	  }
	return os;
      }
    } // ns network
  } // ns ipc
} // ns bsw
