#ifndef __BSW_IPC_NETWORK_HPP__
#define __BSW_IPC_NETWORK_HPP__

#include <iosfwd>
#include <stdexcept>
#include <vector>
#include <string>

#include "bsw/bsw_api.hpp"
#include "thirdparty/my_stdint.h"
#include "bsw/err/throw.hpp"
#include "bsw/err/err.hpp"

namespace bsw
{
  namespace ipc
  {
    namespace network
    {

      BSW_API extern const uint32_t BSW_INADDR_ANY;
      BSW_API extern const uint32_t BSW_INADDR_NONE;
      BSW_API extern const uint32_t BSW_INADDR_LOOPBACK;
      BSW_API extern const uint32_t BSW_INADDR_BROADCAST;

      /** 
       * Initializes network infrastructure. 
       * 
       * initialisation happens only ones
       */
      BSW_API void init () THROW_SPEC (std::runtime_error);

      /** 
       * Deinitializes network infrastructure
       * 
       * Deinitialisation happens only once.
       * @return number of calls. If 1 returned, then all released
       */
      BSW_API void quit () NO_THROW_SPEC;
      
      typedef uint32_t ipv4_addr_t; 
      
      struct BSW_API ip_address_s
      {
	ip_address_s ();
	
	ipv4_addr_t host;
	uint16_t port;
      };
      
      BSW_API std::ostream& operator << (std::ostream& os, const ip_address_s& addr);
      
      /** 
       * resolves host to ipv4 address
       * 
       * @param host - hostname or dotted decimal
       * @param ip_addr - result
       * @param err_code - error code (if any)
       * 
       * @return false if error occured
       */
      BSW_API bool resolve_host (const char* host, ipv4_addr_t& ip_addr, err_code_t& err_code) NO_THROW_SPEC;
      BSW_API bool resolve_host (const std::string& host, ipv4_addr_t& ip_addr, err_code_t& err_code) NO_THROW_SPEC;
      
      /** 
       * returns hostname for the given ip
       * 
       * @param ip - v4 ip to resolve
       * @param host - result
       * @param err_code - error code (if any)
       * 
       * @return false if error occured
       */
      BSW_API bool resolve_ip (ipv4_addr_t ip, std::string& host, err_code_t& err_code) NO_THROW_SPEC;

      BSW_API bool get_local_addresses (std::vector <ip_address_s>& addresses, err_code_t& err_code);
      
      // tcp
      struct tcp_socket_s;

      BSW_API tcp_socket_s* tcp_open (const ip_address_s& addr);
      BSW_API tcp_socket_s* tcp_accept (tcp_socket_s* server);
      BSW_API ip_address_s  get_peer_address (const tcp_socket_s* sock);

      BSW_API int tcp_send (tcp_socket_s* sock, const void* data, int len);
      BSW_API int tcp_recv (tcp_socket_s* sock, void* data, int maxlen);

      BSW_API void tcp_close (tcp_socket_s* sock);

      // udp
      struct udp_socket_s;

      BSW_API udp_socket_s* udp_open ();

      BSW_API int udp_bind (udp_socket_s* socket, uint16_t port);
      BSW_API int udp_send (udp_socket_s* socket, const ip_address_s& addr, const void* data, int len);
      BSW_API int udp_recv (udp_socket_s* socket, ip_address_s& addr, void* data, int maxlen);
      BSW_API void udp_close (udp_socket_s* socket);
      //select
      struct socket_set_s;

      BSW_API socket_set_s* allocate_socket_set (int max_sockets);
      
      BSW_API int add_socket (const udp_socket_s* socket, socket_set_s* set);
      BSW_API int add_socket (const tcp_socket_s* socket, socket_set_s* set);
      BSW_API int rm_socket (const udp_socket_s* socket, socket_set_s* set);
      BSW_API int rm_socket (const tcp_socket_s* socket, socket_set_s* set);

      BSW_API int check_sockets (socket_set_s* sock_set, uint32_t timeout_ms);
      BSW_API bool is_ready (const udp_socket_s* socket);
      BSW_API bool is_ready (const tcp_socket_s* socket);
      
      BSW_API void free_socket_set (socket_set_s* sock_set);
    } // ns network
  } // ns ipc
} // ns bsw


#endif
