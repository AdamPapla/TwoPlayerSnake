#pragma once
#include <cassert>
#include <cstdint>
#include <cstring>
#include <optional>
#include <vector>

#include "Utility.h"

namespace Message {

enum class ClientMessageType : std::uint16_t { REGISTER, JOIN, CHANGE_DIR, LEAVE };

struct ClientMessage {
   ClientMessageType msgType;
};

struct RegisterMessage : ClientMessage {
   std::string name;
   std::optional< ClientId > existingClientId_;
};

struct JoinMessage : ClientMessage {
   std::string roomId;
};

struct ChangeDirMessage : ClientMessage {
   Move newDir;
};

struct LeaveMessage : ClientMessage {};

enum class ServerMessageType : std::uint16_t {
   // Ack/Nack client messages
   REGISTER_ACK,
   REGISTER_NACK,
   JOIN_ACK,
   JOIN_NACK,
   CHANGE_DIR_ACK,
   CHANGE_DIR_NACK,
   LEAVE_ACK,
   LEAVE_NACK,

   // Server response messages
   DISCONNECT,
   DEATH,
   SNAPSHOT
};

struct ServerMessage {
   ServerMessageType msgType;
};

struct RegisterAckMessage : ServerMessage {
   ClientId id;
};

struct DisconnectMessage : ServerMessage {
   std::string reason;
};

struct DeathMessage : ServerMessage {
   uint32_t score;
};

struct Snapshot : ServerMessage {
   std::vector< std::uint8_t > bytes;
};

} // namespace Message
