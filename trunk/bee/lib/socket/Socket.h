#if !defined SOCKET_H
#define SOCKET_H


#ifdef WIN32
    #include <winsock.h>
#else
    #include <netdb.h>
    #include <sys/socket.h>  // SO_REUSEADDR
    #include <sys/types.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h> // SOL_TCP
    #include <netinet/udp.h> // SOL_UDP
    #include <arpa/inet.h>
#endif

#include <errno.h>
#include <fcntl.h>

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

using std::string ;




class Socket
{
    public:
        class Exception :public std::runtime_error {
            public:
                Exception() : std::runtime_error("Socket Error" )
                {
                };

                Exception(const char* ex) : std::runtime_error(ex)
                {
                };
        };

        //! Socket families
        struct Family {
            typedef int  Flag ;
            static const Flag Unix      = PF_UNIX ;
            static const Flag Inet      = PF_INET ;
            static const Flag Inet6     = PF_INET6 ;
            static const Flag Ipx       = PF_IPX ;
            static const Flag Netlink   = PF_NETLINK ;
            static const Flag X25       = PF_X25 ;
            static const Flag Ax25      = PF_AX25 ;
            static const Flag Atmpvc    = PF_ATMPVC ;
            static const Flag Appletalk = PF_APPLETALK ;
            static const Flag Packet    = PF_PACKET ;
        } ;

        //! Socket types
        struct Type {
            typedef int  Flag ;
            static const Flag Stream    = SOCK_STREAM ;
            static const Flag Datagram  = SOCK_DGRAM ;
            static const Flag Raw       = SOCK_RAW ;
            static const Flag Rdm       = SOCK_RDM ;
            static const Flag Seqpacket = SOCK_SEQPACKET ;
            static const Flag Packet    = SOCK_PACKET ;
            //static const Flag Max       = MAX ;
            //static const Flag Unknown   = UNKNOWN ;
        } ;
        //! /usr/include/linux/socket.h
        struct Level{
            typedef int  Flag ;
            static const Flag Ip        = SOL_IP ;
            static const Flag Tcp       = SOL_TCP ;
            static const Flag Udp       = SOL_UDP ;
            static const Flag Ipv6      = SOL_IPV6 ;
            static const Flag Icpmv6    = SOL_ICMPV6 ;
            static const Flag Socket    = SOL_SOCKET ;
            static const Flag Raw       = SOL_RAW ;
        } ;


        struct Option{
            typedef int  Flag ;
            static const Flag ReuseAddress =SO_REUSEADDR ;
            static const Flag Type       = SO_TYPE ;
            static const Flag Error      = SO_ERROR ;
            static const Flag DontRoute  = SO_DONTROUTE ;
            static const Flag Broadcast  = SO_BROADCAST ;
            static const Flag SendBuf    = SO_SNDBUF ;
            static const Flag KeepAlive  = SO_KEEPALIVE ;
            static const Flag Linger     = SO_LINGER ;
            //static const Flag RecvBuf    = SO_RECVBUF ;
        } ;//! the whole list is in /usr/include/asm-i486/socket.h

    public:
        int     sock_ ;     //!< Socket descriptor.
        int     tout_ ;     //!< Socket timeout.
        bool    is_open_ ;  //!< Tells if the socket was created.
        bool    is_conn_ ;  //!< Tells if the socket was connected.
        bool    is_sync_ ;//!< Is the socket sync or async ?
        string  resp_ ;     //!< Place to hold responses.
        Type::Flag      type_ ;
        Family::Flag    family_ ;

    public:
        Socket( ) ;
        ~Socket( ) ;

        void open   ( Socket::Family::Flag family ,
                      Socket::Type::Flag type,
                      const int protocol_i ) ;
        void write  ( const char* msg, const int& size ) ;
        void write  ( const string& msg ) ;
        void read   ( char* s, unsigned int n) ;
        void connect( const char* hostName, const int port ) ;
        void timeout( int const& tout ) ;   //!< Set timeout
        unsigned int  timeout() ;                    //!< Get timeout
        void close  ( ) ;                   //!< Close the connection
        void close  ( const int how ) ;           //!< Close the connection
        void shutdown( const int how ) ;
        inline bool is_open( )      { return is_open_ ; } ;
        inline bool is_connected()  { return is_conn_ ; } ;
        inline string response( )   { return resp_ ; } ;
} ;//class Socket

#endif





#if 0  //TODO
    Comunication------------
recv()          //! diferentierea intre recv/TCP si recvfrom/UDP, se face in interiorul clasei.
    // extern, este folosit numai recv(), folosindu-se dupa caz, recv sau recvfrom
    operator >>
    recvfrom()
send()
    sendfile(int out_fd, int in_fd, off_t *offset, size_t count);
    send   ( FILE* f, int64 offset, unsigned long count )  sendfile( sock_, fileno(f), offset, count ) ;//! use fgetpos
    send   ( fstream, offset, count ) ;
    operator <<
    sendmsg()
sendto()

    Connection Control------
    accept
    bind
    connect
    is_connected
    listen
    shutdown
    socket
    socketpair
    Socket Controls---------
    setsockopt
    getsockopt
    Naming Calls------------
    get_hostbyaddr
    get_hostbyname
    get_hostname
    get_peername
    get_protobyname
    get_servbyname
    set_servent
    Conversion Tools-------- Folosite numai intern ???
    htons       - hostToNetworkShort
    ntohs       - networkToHostShort
    inet_addr   - convert dotted IP to binary address
    inet_aton   - convert dotted IP to binary address
    inet_ntoa
    inet_ntop
    inet_pton


#endif
