#pragma once

#include "common.h"
#include "mpack/mpack.h"

EXTERN_C void online_begin_step_game_loop();
EXTERN_C void online_end_step_game_loop();
EXTERN_C void online_on_clear_npcs();

#ifdef __cplusplus

namespace online {
    /// A PeerId, which is a UUID.
    /// Represented as a u64 pair: https://docs.rs/uuid/latest/uuid/struct.Uuid.html#method.as_u64_pair
    class PeerId {
        u64 mostSignificant;
        u64 leastSignificant;

    public:
        PeerId() : mostSignificant(0), leastSignificant(0) {}
        PeerId(u64 most, u64 least) : mostSignificant(most), leastSignificant(least) {}
        PeerId(mpack_reader_t* reader) {
            mostSignificant = mpack_expect_u64(reader);
            leastSignificant = mpack_expect_u64(reader);
        }
        bool operator==(PeerId const& other) const {
            return mostSignificant == other.mostSignificant && leastSignificant == other.leastSignificant;
        }
        bool operator!=(PeerId const& other) const {
            return !(*this == other);
        }
        void print() const {
            debug_printf("PeerId(%016llx-%016llx)", mostSignificant, leastSignificant);
        }
        void lld_pair(u64& most, u64& least) const {
            most = mostSignificant;
            least = leastSignificant;
        }
        void mpack_write(mpack_writer_t* writer) const {
            mpack_write_u64(writer, mostSignificant);
            mpack_write_u64(writer, leastSignificant);
        }
    };

    namespace comms {
        /// Communication happens via a GDB client that connects to the emulator.
        ///
        /// Terminology:
        /// - "client" refers to `../../client/`, the GUI program that interfaces between the network and the emulator.
        /// - "room" refers to the room of the signaling server that the client is connected to.
        /// - "peer" refers to another person in the room, and also ourselves.
        class Message {
            u32 framesSinceLastRecv;

            /// This client's peer ID.
            PeerId peerId;
        public:
            mpack_writer_t writer;

            char outMessage[128]; // actual message being sent
            char outMessageSwap[128]; // swap buffer for writing
            volatile char inMessage[128];
            volatile u32 outSignal; // size
            volatile b32 inSignal;

            Message();

            /// Read incoming message from the buffer.
            void decode();

            /// Write outgoing message for this frame to the buffer.
            void encode();

            void reset_writer();

            inline PeerId const& peer_id() const { return peerId; };
            inline void set_peer_id(PeerId id) { peerId = id; };
            inline b32 has_peer_id() const { return peerId != PeerId(); }

            inline b32 is_connected_to_client() const { return framesSinceLastRecv < 60; }
        };

        void begin_step();
        void end_step();
        Message& get_message();
    };

    namespace rpc {
        void set_player_state(Vec3f const& pos, f32 yaw, AnimID anim);
        void set_map(s16 mapID, s16 areaID);

        namespace read {
            void mpack_read_rpc(mpack_reader_t* reader);
            void peer_connected(mpack_reader_t* reader);
            void peer_disconnected(mpack_reader_t* reader);
            void set_peer_id(mpack_reader_t* reader);
            void dbg_print(mpack_reader_t* reader);
            void set_player_state(mpack_reader_t* reader);
            void set_map(mpack_reader_t* reader);
        }
    };

    namespace peers {
        constexpr size_t MAX_PEERS = 16;

        class Peer {
            bool connected = false;
            Npc* npc = nullptr;
            void connect(PeerId const& id);
        public:
            PeerId id;
            void disconnect();
            void update();
            void move(Vec3f const& pos, f32 yaw, AnimID anim);
            void set_map(s16 mapID, s16 areaID);
            bool is_connected() const { return connected; }

            static Peer* get_ptr(PeerId const& id);
            static Peer& get_mut(PeerId const& id);
            static Peer const& get(PeerId const& id);
            static Peer& upsert_mut(PeerId const& id);
            static void disconnect_all();
            static void clear_npcs();
        };

        extern Peer peers[MAX_PEERS];
        void update();
    }
};

#endif
