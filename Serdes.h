#pragma once
#include <cassert>
#include <concepts>
#include <cstdint>
#include <cstring>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "GameState.h"

namespace Serdes {

using namespace GameState;

template < typename T >
concept TriviallySerializable =
    std::is_trivially_copyable_v< T > && std::is_standard_layout_v< T >;

struct BytesCounter {
   std::size_t count{ 0 };

   template < TriviallySerializable T >
   void write( const T & ) {
      count += sizeof( T );
   }
   template < TriviallySerializable T >
   void writeBytes( const T *const, const std::size_t size ) {
      count += size * sizeof( T );
   }
};

class BytesWriter {
 public:
   BytesWriter( std::span< std::uint8_t > bytes )
       : bytes_{ bytes }, remBytes_{ bytes_.size() } {}

   template < TriviallySerializable T >
   void write( const T & val ) {
      assert( remBytes_ >= sizeof( T ) &&
              "Didn't allocate enough space in serialization buffer" );
      std::size_t currentIdx = bytes_.size() - remBytes_;
      std::memcpy( bytes_.data() + currentIdx, &val, sizeof( T ) );
      remBytes_ -= sizeof( T );
   }
   template < TriviallySerializable T >
   void writeBytes( const T *const data, const std::size_t size ) {
      std::size_t currentIdx = bytes_.size() - remBytes_;
      std::size_t writeSize = size * sizeof( T );
      assert( remBytes_ >= writeSize &&
              "Didn't allocate enough space in serialization buffer" );
      std::memcpy( bytes_.data() + currentIdx, data, writeSize );
      remBytes_ -= writeSize;
   }

   std::size_t remainingBytes() { return remBytes_; }

 private:
   std::span< uint8_t > bytes_;
   std::size_t remBytes_;
};

struct BytesReader {
 public:
   BytesReader( std::span< std::uint8_t > bytes )
       : bytes_{ bytes }, remBytes_{ bytes.size() } {}

   template < TriviallySerializable T >
   T read() {
      T val;
      if ( remBytes_ < sizeof( T ) ) {
         throw std::runtime_error( "Not enough bytes left in buffer to read" );
      }
      std::size_t currentIdx = bytes_.size() - remBytes_;
      std::memcpy( &val, bytes_.data() + currentIdx, sizeof( T ) );
      remBytes_ -= sizeof( T );
      return val;
   }
   template < TriviallySerializable T >
   void readBytes( std::span< T > dest ) {
      std::size_t writeSize = dest.size_bytes();
      if ( remBytes_ < writeSize ) {
         throw std::runtime_error( "Not enough bytes left in buffer to read" );
      }
      std::size_t currentIdx = bytes_.size() - remBytes_;
      std::memcpy( dest.data(), bytes_.data() + currentIdx, writeSize );
      remBytes_ -= writeSize;
   }

   std::size_t remainingBytes() { return remBytes_; }

 private:
   std::span< uint8_t > bytes_;
   std::size_t remBytes_;
};

std::vector< uint8_t > write( const Snapshot & snapshot );
Snapshot read( std::span< uint8_t > bytes );

} // namespace Serdes
