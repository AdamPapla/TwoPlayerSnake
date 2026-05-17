#pragma once
#include <cassert>
#include <cstdint>
#include <cstring>
#include <vector>

#include "Messages.h"
#include "Serdes.h"

namespace Serdes {

using namespace Message;

// Keeping these generic and instantiating with BytesCounter or BytesWriter prevents
// our size calculation and serialization going out of sync
template < typename BytesOp >
bool
write( const RegisterMessage & msg, BytesOp & bytesOp ) {
   bool success = bytesOp.write( msg.msgType );
   success &= bytesOp.write( msg.existingClientId );
   success &= bytesOp.write( msg.nameLen );
   success &= bytesOp.writeBytes( msg.name.data(), msg.nameLen );
   return success;
}

inline bool
read( RegisterMessage & msg, BytesReader & reader ) {
   auto msgType = reader.read< ClientMessageType >();
   auto id = reader.read< ClientId >();
   using NameLen = decltype( RegisterMessage::nameLen );
   auto nameLen = reader.read< NameLen >();
   if ( msgType && id && nameLen ) {
      msg.msgType = msgType.value();
      msg.existingClientId = id.value();
      msg.nameLen = nameLen.value();
      msg.name.resize( msg.nameLen );
      return reader.readBytes< char >( msg.name );
   }
   return false;
}

template < typename BytesOp >
bool
write( const JoinMessage msg, BytesOp & bytesOp ) {
   bool success = bytesOp.write( msg.msgType );
   success &= bytesOp.write( msg.roomId );
   return success;
}

inline bool
read( JoinMessage & msg, BytesReader & reader ) {
   auto msgType = reader.read< ClientMessageType >();
   auto roomId = reader.read< std::uint32_t >();
   if ( msgType && roomId ) {
      msg = { { msgType.value() }, roomId.value() };
      return true;
   }
   return false;
}

template < typename BytesOp >
bool
write( const ChangeDirMessage msg, BytesOp & bytesOp ) {
   bool success = bytesOp.write( msg.msgType );
   success &= bytesOp.write( msg.newDir );
   return success;
}

inline bool
read( ChangeDirMessage & msg, BytesReader & reader ) {
   auto msgType = reader.read< ClientMessageType >();
   auto newDir = reader.read< Move >();
   if ( msgType && newDir ) {
      msg = { { msgType.value() }, newDir.value() };
      return true;
   }
   return false;
}

template < typename BytesOp >
bool
write( const LeaveMessage msg, BytesOp & bytesOp ) {
   bool success = bytesOp.write( msg.msgType );
   return success;
}

inline bool
read( LeaveMessage & msg, BytesReader & reader ) {
   auto msgType = reader.read< ClientMessageType >();
   if ( msgType ) {
      msg = { { msgType.value() } };
      return true;
   }
   return false;
}

template < typename BytesOp >
bool
write( const AckMessage msg, BytesOp & bytesOp ) {
   bool success = bytesOp.write( msg.msgType );
   success &= bytesOp.write( msg.id );
   success &= bytesOp.write( msg.reason );
   return success;
}

inline bool
read( AckMessage & ack, BytesReader & reader ) {
   auto msgType = reader.read< ServerMessageType >();
   auto id = reader.read< ClientId >();
   auto nackReason = reader.read< NackReason >();
   if ( id && nackReason ) {
      ack = { { .msgType = msgType.value() }, id.value(), nackReason.value() };
      return true;
   }
   return false;
}

template < typename BytesOp >
bool
write( const DisconnectMessage & msg, BytesOp & bytesOp ) {
   bool success = bytesOp.write( msg.msgType );
   success &= bytesOp.write( msg.reasonLen );
   success &= bytesOp.writeBytes( msg.reason.data(), msg.reasonLen );
   return success;
}

inline bool
read( DisconnectMessage & msg, BytesReader & reader ) {
   using MsgLen = decltype( DisconnectMessage::reasonLen );
   auto msgType = reader.read< ServerMessageType >();
   auto len = reader.read< MsgLen >();
   if ( len && msgType ) {
      msg.msgType = msgType.value();
      msg.reasonLen = len.value();
      msg.reason.resize( msg.reasonLen );
      return reader.readBytes< char >( msg.reason );
   }
   return false;
}

template < typename BytesOp >
bool
write( const DeathMessage msg, BytesOp & bytesOp ) {
   bool success = bytesOp.write( msg.msgType );
   success &= bytesOp.write( msg.score );
   return success;
}

inline bool
read( DeathMessage & msg, BytesReader & reader ) {
   auto msgType = reader.read< ServerMessageType >();
   auto score = reader.read< uint32_t >();
   if ( msgType && score ) {
      msg.msgType = msgType.value();
      msg.score = score.value();
      return true;
   }
   return false;
}

template < typename BytesOp >
bool
write( const SnapshotMessage & msg, BytesOp & bytesOp ) {
   bool success = bytesOp.write( msg.msgType );
   success &= bytesOp.write( msg.snapshotLen );
   success &= bytesOp.writeBytes( msg.bytes.data(), msg.snapshotLen );
   return success;
}

inline bool
read( SnapshotMessage & msg, BytesReader & reader ) {
   using MsgLen = decltype( SnapshotMessage::snapshotLen );
   auto msgType = reader.read< ServerMessageType >();
   auto len = reader.read< MsgLen >();
   if ( msgType && len ) {
      msg.msgType = msgType.value();
      msg.snapshotLen = len.value();
      msg.bytes.resize( msg.snapshotLen );
      return reader.readBytes< std::uint8_t >( msg.bytes );
   }
   return false;
}

} // namespace Serdes
