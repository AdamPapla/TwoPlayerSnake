#pragma once
#include <algorithm>
#include <arpa/inet.h>
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <format>
#include <functional>
#include <iostream>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <optional>
#include <queue>
#include <random>
#include <span>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>

#include "Messages.h"
#include "Serdes.h"
#include "Serializers.h"
#include "TSQueue.h"

class SnakeClient {
 public:
   SnakeClient( TSQueue< Message::ServerMessage > & ingressQueue,
                TSQueue< Message::ClientMessage > & egressQueue,
                std::stop_token stop )
       : ingressQueue_{ ingressQueue }, egressQueue_{ egressQueue }, stop_{ stop } {}

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
      while ( !stop_.stop_requested() ) {
         auto readBytes =
             ::recv( sock_, ingressBuff_.data(), ingressBuff_.size(), 0 );
         // TODO: Add proper error handling here
         assert( readBytes != -1 && "recv call failed" );
         if ( readBytes > 0 ) {
            accumulator_.insert( accumulator_.end(),
                                 ingressBuff_.begin(),
                                 ingressBuff_.begin() + readBytes );
            drainIngressQueue();
            maybeCompactAccumulator();
         }
         queueOutgoing();
         sendFromBuffer();
      }
   }

 private:
   void drainIngressQueue() {
      using namespace Message;
      // TODO: Good opportunity to practice circular buffers here, for now just
      // going with simple read offset + occasional resize.
      std::span< std::uint8_t > toRead{ accumulator_.begin() + readOffset_,
                                        accumulator_.end() };
      Serdes::BytesReader reader( toRead );
      while ( !stop_.stop_requested() ) {
         auto msgType = reader.peek< Message::ServerMessageType >();
         if ( !msgType.has_value() )
            return;
         switch ( msgType.value() ) {
         case ServerMessageType::REGISTER_ACK:
         case ServerMessageType::JOIN_ACK:
         case ServerMessageType::CHANGE_DIR_ACK:
         case ServerMessageType::LEAVE_ACK: {
            auto ackOpt = reader.read< AckMessage >();
            if ( !ackOpt.has_value() )
               return;
            ingressQueue_.push( ackOpt.value() );
            break;
         }
         case ServerMessageType::SNAPSHOT: {
            using MsgLen = decltype( SnapshotMessage::snapshotLen );
            auto lenOpt = reader.read< MsgLen >();
            if ( !lenOpt.has_value() )
               return;
            std::vector< std::uint8_t > bytes;
            bytes.resize( lenOpt.value() );
            if ( !reader.readBytes< std::uint8_t >( bytes ) )
               return;
            ingressQueue_.push( SnapshotMessage{ .snapshotLen = lenOpt.value(),
                                                 .bytes = std::move( bytes ) } );
         }
         case ServerMessageType::DISCONNECT: {
            using MsgLen = decltype( DisconnectMessage::reasonLen );
            auto lenOpt = reader.read< MsgLen >();
            if ( !lenOpt.has_value() )
               return;
            std::string reason;
            reason.resize( lenOpt.value() );
            if ( !reader.readBytes( std::span< char >( reason ) ) )
               return;
            ingressQueue_.push( DisconnectMessage{ .reasonLen = lenOpt.value(),
                                                   .reason = std::move( reason ) } );
            break;
         }
         case ServerMessageType::DEATH: {
            auto deathOpt = reader.read< DeathMessage >();
            if ( !deathOpt )
               return;
            ingressQueue_.push( deathOpt.value() );
            break;
         }
         }
      }
   }

   void queueOutgoing() {
      // We queue to buffer to make use of TCP's partial sends and prevent blocking
      // network thread for too long Ensure we have enough space, then write the
      // message.
      Serdes::BytesWriter writer{ std::span< uint8_t >(
          egressBuff_.begin() + writeOffset_, egressBuff_.end() ) };
      bool bufferFull = writeOffset_ != buffSize_;
      while ( !stop_.stop_requested() && !bufferFull ) {
         auto outbound = egressQueue_.try_front();
         if ( !outbound.has_value() )
            break;
         Serdes::BytesCounter counter;
         std::visit( [ & ]( const auto & msg ) { Serdes::write( msg, counter ); },
                     outbound.value() );
         bufferFull = counter.count > writer.remainingBytes();
         if ( bufferFull )
            break;
         std::visit( [ & ]( const auto & msg ) { Serdes::write( msg, writer ); },
                     outbound.value() );
         egressQueue_.pop();
         writeOffset_ = buffSize_ - writer.remainingBytes();
      }
   }

   void sendFromBuffer() {
      auto *sendStart = &( egressBuff_[ sendOffset_ ] );
      auto sendLen = writeOffset_ - sendOffset_;
      auto sentBytes = ::send( sock_, sendStart, sendLen, 0 );
      assert( sentBytes != -1 && "send call failed" );
      sendOffset_ += sentBytes;
      // TODO: Again - ring buffers make this clean. This is just a stop-gap
      if ( sendOffset_ == writeOffset_ ) {
         sendOffset_ = 0;
         writeOffset_ = 0;
      }
   }

   // TODO: Rung buffers will remove the need for this shrinking
   void maybeCompactAccumulator() {
      if ( accumulator_.size() > buffSize_ / 2 ) {
         // TODO: This may lead to wasted allocations once the buffer fills again
         accumulator_.erase( accumulator_.begin(),
                             accumulator_.begin() + readOffset_ );
      }
   }

   constinit static const std::size_t buffSize_{ 8192 };

   TSQueue< Message::ServerMessage > & ingressQueue_;
   TSQueue< Message::ClientMessage > & egressQueue_;

   std::array< std::uint8_t, buffSize_ > ingressBuff_;
   std::vector< std::uint8_t > accumulator_;
   std::size_t readOffset_{ 0 };

   std::array< std::uint8_t, buffSize_ > egressBuff_;
   std::size_t writeOffset_{ 0 };
   std::size_t sendOffset_{ 0 };

   std::condition_variable cv_;
   std::stop_token stop_;
   int sock_;
};
