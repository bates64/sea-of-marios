#pragma once

#include "common.h"
#include "mpack/mpack.h"

EXTERN_C void online_begin_step_game_loop();
EXTERN_C void online_end_step_game_loop();

#ifdef __cplusplus

EXTERN_C HeapNode heap_generalHead;

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
            debug_printf("PeerId(%lld, %lld)", mostSignificant, leastSignificant);
        }
        void mpack_write(mpack_writer_t* writer) const {
            mpack_write_u64(writer, mostSignificant);
            mpack_write_u64(writer, leastSignificant);
        }
    };

    namespace comms {


        /// Communication happens via a GDB client that connects to the emulator.
        /// They talk over @p rawMessage, which is a buffer that is read and written to by both.
        ///
        /// Terminology:
        /// - "client" refers to `../../client/`, the GUI program that interfaces between the network and the emulator.
        /// - "room" refers to the room of the signaling server that the client is connected to.
        /// - "peer" refers to another person in the room, and also ourselves.
        class Message {
            /// Outgoing length of rawMessage that the GDB client should read. The GDB client will watch this value.
            volatile u32 length = 0;

            /// The buffer used **both** for reading and writing messages.
            /// GDB will write this at the start of each frame with the most recent message from the server.
            /// At the end of each frame, the client will write its message and send it to the server.
            char rawMessage[0x200];

            /// Note: does not neccessarily mean we are in a room.
            b32 isGdbConnected = FALSE;

            /// This client's peer ID.
            PeerId peerId;

            void reset_writer();
        public:
            mpack_writer_t writer;

            Message();

            /// Read incoming message from the buffer.
            void decode();

            /// Write outgoing message for this frame to the buffer.
            void encode();

            inline PeerId const& peer_id() const { return peerId; };
            inline void set_peer_id(PeerId id) { peerId = id; };

            inline b32 is_connected_to_client() const { return isGdbConnected; }
        };

        void begin_step();
        void end_step();
        Message& get_message();
    };

    namespace rpc {
        void set_player_state(Vec3f const& pos, f32 yaw, AnimID anim);

        namespace read {
            void mpack_read_rpc(mpack_reader_t* reader);
            void peer_connected(mpack_reader_t* reader);
            void peer_disconnected(mpack_reader_t* reader);
            void set_peer_id(mpack_reader_t* reader);
            void set_player_state(mpack_reader_t* reader);
        }
    };

    namespace peers {
        constexpr size_t MAX_PEERS = 16;

        class Peer {
            PeerId id;
            Npc* npc;
            u32 timeout = 0;
        public:
            Peer(PeerId id);
            void free();
            void update();
            void move(Vec3f const& pos, f32 yaw, AnimID anim);
            static Peer& get_by_id(PeerId id);

            // move ctor
            Peer(Peer&& other) : id(other.id), npc(other.npc), timeout(other.timeout) {
                other.npc = nullptr;
            }

            // move assignment
            Peer& operator=(Peer&& other) {
                id = other.id;
                npc = other.npc;
                timeout = other.timeout;
                other.npc = nullptr;
                return *this;
            }
        };

        template <typename T>
        constexpr T&& move(T& t) noexcept
        {
            return static_cast<T&&>(t);
        }

        template <typename T>
        class Vec {
            T* data;
            size_t capacity;
            size_t size;
        public:
            Vec() : data(nullptr), capacity(0), size(0) {}
            Vec(size_t capacity) : capacity(capacity), size(0) {
                data = (T*)general_heap_malloc(capacity * sizeof(T));
            }

            template <size_t N>
            Vec(T const (&arr)[N]) : capacity(N), size(N) {
                data = general_heap_malloc(N * sizeof(T));
                memcpy(data, arr, N * sizeof(T));
            }

            ~Vec() {
                general_heap_free(data);
            }

            T& push(T&& value) {
                if (size >= capacity) {
                    if (capacity == 0) {
                        capacity = 1;
                        data = (T*)general_heap_malloc(capacity * sizeof(T));
                    } else {
                        capacity *= 2;
                        data = (T*)_heap_realloc(&heap_generalHead, &data, capacity * sizeof(T));
                    }
                }

                data[size++] = move(value);
                return data[size - 1];
            }

            void remove(T const& value) {
                for (size_t i = 0; i < size; i++) {
                    if (&data[i] == &value) {
                        for (size_t j = i; j < size - 1; j++) {
                            bcopy(&data[j], &data[j + 1], sizeof(T)); // probably evil
                        }
                        size--;
                        return;
                    }
                }
            }

            T& operator[](size_t index) {
                return data[index];
            }

            T const& operator[](size_t index) const {
                return data[index];
            }

            size_t len() const {
                return size;
            }

            T* begin() {
                return data;
            }

            T* end() {
                return data + size;
            }

            T const* begin() const {
                return data;
            }

            T const* end() const {
                return data + size;
            }
        };

        extern Vec<Peer> peers;

        static inline void remove(Peer const& value) {
            peers.remove(value);
        }

        void update();
    }
};

#endif
