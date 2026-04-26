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
void
write( const RegisterMessage & msg, const BytesOp & bytesOp ) {
   bytesOp.write( msg.msgType );
   bytesOp.write( msg.nameLen );
   bytesOp.writeBytes( msg.name.data(), msg.nameLen );
   if ( msg.existingClientId.has_value() ) {
      bytesOp.write( msg.existingClientId );
   }
}

template < typename BytesOp >
void
write( const JoinMessage msg, const BytesOp & bytesOp ) {
   bytesOp.write( msg.msgType );
   bytesOp.write( msg.roomId );
}

template < typename BytesOp >
void
write( const ChangeDirMessage msg, BytesOp bytesOp ) {
   bytesOp.write( msg.msgType );
   bytesOp.write( msg.newDir );
}

template < typename BytesOp >
void
write( const AckMessage msg, const BytesOp & bytesOp ) {
   bytesOp.write( msg.msgType );
   bytesOp.write( msg.id );
   bytesOp.write( msg.reason );
}

template < typename BytesOp >
void
write( const DisconnectMessage & msg, const BytesOp & bytesOp ) {
   bytesOp.write( msg.msgType );
   bytesOp.write( msg.reasonLen );
   bytesOp.writeBytes( msg.reason.data(), msg.reasonLen );
}

template < typename BytesOp >
void
write( const DeathMessage msg, const BytesOp & bytesOp ) {
   bytesOp.write( msg.msgType );
   bytesOp.write( msg.score );
}

template < typename BytesOp >
void
write( const SnapshotMessage & msg, const BytesOp & bytesOp ) {
   bytesOp.write( msg.msgType );
   bytesOp.write( msg.snapshotLen );
   bytesOp.write( msg.bytes.data(), msg.snapshotLen );
}

} // namespace Serdes
