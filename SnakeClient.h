#pragma once
#include <algorithm>
#include <arpa/inet.h>
#include <chrono>
#include <cstdlib>
#include <format>
#include <functional>
#include <iostream>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <optional>
#include <queue>
#include <random>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>

#include "TSQueue.h"

class ServerMessage;
class ClientMessage;
class SnakeClient {
 public:
   SnakeClient( TSQueue< ServerMessage > & ingressQueue,
                TSQueue< ClientMessage > & egressQueue,
                std::function< bool() > exit )
       : ingressQueue_{ ingressQueue }, egressQueue_{ egressQueue },
         shouldExit{ exit } {}

   bool connect() {
      sock_ = socket( AF_INET, SOCK_STREAM, 0 );

      uint16_t serverPort = 8080;
      std::string serverAddr = "127.0.0.1";

      sockaddr_in server{};
      server.sin_family = AF_INET;
      inet_pton( AF_INET, serverAddr.c_str(), &server.sin_addr );
      server.sin_port = htons( 8080 );
      return ::connect(
          sock_, reinterpret_cast< sockaddr * >( &server ), sizeof( server ) );
   }
   void doNetworkLoop() {
      while ( !shouldExit() ) {
         // TODO: Implement this
      }
   }

 private:
   TSQueue< ServerMessage > & ingressQueue_;
   TSQueue< ClientMessage > & egressQueue_;

   std::function< bool() > shouldExit;
   int sock_;
};
