#pragma once
#include <unistd.h>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <chrono>
#include <thread>
#include <queue>
#include <random>
#include <unordered_set>
#include <unordered_map>
#include <optional>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <format>
#include <functional>

#include "TSQueue.h"

struct ServerTcpConfig {
   int family;
};

using Tick = std::uint32_t;

// Making this a struct for now in case it needs expanded later
struct ClientConnection {
   ClientConnection( int sock ) : clientSock { sock } {}
   int clientSock;
};

class SnakeServer {
 public:
   SnakeServer( ServerTcpConfig config, std::function<bool()> exitFunc ) : config_ { config }, shouldExit { exitFunc } {}
   ~SnakeServer() { close( listenSock_ ); }
   void listen() {
      listenSock_ = socket( AF_INET, SOCK_STREAM, 0 );
      assert( listenSock_ != -1 );
      sockaddr_in addr{};
      addr.sin_family = config_.family;
      addr.sin_addr.s_addr = INADDR_ANY;
      uint16_t port = 8080;
      addr.sin_port = htons(port);
      if ( bind( listenSock_, reinterpret_cast< sockaddr * >( &addr ), sizeof(addr) ) == -1 ) {
         throw std::runtime_error( std::format( "Could not find port at {}", port ) );
      }
      if ( ::listen( listenSock_, 10 ) == -1 ){ 
         throw std::runtime_error( std::format( "Failed to listen on socket {}, port {}", listenSock_, port ) );
      }
      std::cout << "Listening for incoming connections..." << std::endl;
      while ( !shouldExit() ) {
         int clientSock = accept( listenSock_, nullptr, nullptr );
         if ( clientSock == -1 ) {
            continue;
         }
         clientQueue_.push( ClientConnection( clientSock ) );
      }

   }
 private:
   TSQueue< ClientConnection > clientQueue_;
   std::function<bool()> shouldExit;
   ServerTcpConfig config_;
   int listenSock_;
};

