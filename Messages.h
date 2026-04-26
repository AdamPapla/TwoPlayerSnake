#pragma once
#include <cassert>
#include <cstdint>
#include <cstring>
#include <optional>
#include <variant>
#include <vector>

#include "Utility.h"

namespace Message {

enum class ClientMessageType : std::uint16_t { REGISTER, JOIN, CHANGE_DIR, LEAVE };

struct ClientMessageBase {
   ClientMessageType msgType;
};

struct RegisterMessage : ClientMessageBase {
   std::uint8_t nameLen;
   std::string name;
   std::optional< ClientId > existingClientId;
};

struct JoinMessage : ClientMessageBase {
   std::uint32_t roomId;
};

struct ChangeDirMessage : ClientMessageBase {
   Move newDir;
};

struct LeaveMessage : ClientMessageBase {};

enum class ServerMessageType : std::uint16_t {
   // Ack/Nack client messages
   REGISTER_ACK,
   JOIN_ACK,
   CHANGE_DIR_ACK,
   LEAVE_ACK,

   // Server response messages
   DISCONNECT,
   DEATH,
   SNAPSHOT
};

struct ServerMessageBase {
   ServerMessageType msgType;
};

enum class NackReason : std::uint16_t {
   // TODO: This will be expanded as needed, placeholder for now
   UNSET
};

struct AckMessage : ServerMessageBase {
   ClientId id;
   NackReason reason;
};

struct DisconnectMessage : ServerMessageBase {
   std::uint16_t reasonLen;
   std::string reason;
};

struct DeathMessage : ServerMessageBase {
   uint32_t score;
};

struct SnapshotMessage : ServerMessageBase {
   uint32_t snapshotLen;
   std::vector< std::uint8_t > bytes;
};

using ClientMessage =
    std::variant< RegisterMessage, JoinMessage, ChangeDirMessage, LeaveMessage >;
using ServerMessage =
    std::variant< AckMessage, DisconnectMessage, DeathMessage, SnapshotMessage >;

} // namespace Message
