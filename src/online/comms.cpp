#include "common.h"
#include "dx/debug_menu.h"
#include "online.h"

namespace online::comms {

Message message;
struct Header {
    char magic[0x20];
    char* outMessage; // Outgoing message
    volatile char* inMessage; // Incoming message
    volatile u32* outSignal; // size of outgoing message, 0 if not ready
    volatile b32* inSignal; // 1 if incoming message is ready to be read
};

// .text section is linked first, so header will be placed at the start of this segment
[[gnu::section(".text")]] Header header = {
    "PAPERMARIO-DX ONLINE",
    message.outMessage,
    message.inMessage,
    &message.outSignal,
    &message.inSignal,
};

void begin_step() {
    static b32 needsInit = TRUE;
    if (needsInit) {
        // This would be better off as constructor initialization, but we don't support global constructors yet.
        // See: .init_array section being discarded in the linker script.
        message = Message();
        needsInit = FALSE;
    }

    message.decode();
    message.reset_writer();
}

void end_step() {
    message.encode();

    // Forget peer ID if we've disconnected
    /*if (!message.is_connected_to_client()) {
        message.set_peer_id(PeerId());
    }*/
}

Message& get_message() {
    return message;
}

Message::Message() {
    ASSERT_MSG((u32)&header == 0x80700000U, "online::comms::header placed at %p\nbut it should be at %p", &header, (void*)0x80700000);
    framesSinceLastRecv = 0xFFFFFFFFU;
    bzero(outMessage, sizeof(outMessage));
    bzero(outMessageSwap, sizeof(outMessageSwap));
    bzero((void*)inMessage, sizeof(inMessage));
    outSignal = 0;
    inSignal = 0;
}

void Message::decode() {
    // Only read if we have a signal
    if (!inSignal) {
        if (framesSinceLastRecv < 0xFFFFFFFFU) framesSinceLastRecv += 1;
        return;
    }
    framesSinceLastRecv = 0;

    //osInvalDCache((void*)inMessage, sizeof(inMessage));
    mpack_reader_t reader;
    mpack_reader_init_data(&reader, (const char*)inMessage, sizeof(inMessage));

    // Read until nil
    while (mpack_peek_tag(&reader).type != mpack_type_nil) {
        online::rpc::read::mpack_read_rpc(&reader);
        if (mpack_reader_error(&reader) != mpack_ok) break;
    }
    mpack_expect_nil(&reader);

    mpack_error_t error = mpack_reader_destroy(&reader);
    if (error != mpack_ok) {
        debug_printf("MPack read error: %s\n", mpack_error_to_string(error));
    }

    // Reset signal so we don't read the same message again
    inSignal = FALSE;
}

void Message::reset_writer() {
    mpack_writer_init(&writer, outMessageSwap, sizeof(outMessageSwap));
    mpack_writer_set_error_handler(&writer, [](mpack_writer_t* writer, mpack_error_t error) {
        debug_printf("MPack write error: %s\n", mpack_error_to_string(error));
    });
    // During the frame, the user will write RPCs using online::rpc:: functions.
}

void Message::encode() {
    // End of message marker
    mpack_write_nil(&writer);

    size_t messageSize = mpack_writer_buffer_used(&writer);

    mpack_error_t error = mpack_writer_destroy(&writer);
    if (error != mpack_ok) {
        debug_printf("MPack write error: %s\n", mpack_error_to_string(error));
        return;
    }

    // Signal, if there is a message (ie it isn't just nil)
    if (messageSize > 1) {
        if (outSignal != 0) {
            // TODO: better to concat messages?
            //debug_printf("discarding previous message");
        }
        bcopy(outMessageSwap, outMessage, sizeof(outMessage));
        outSignal = messageSize;
    }
};

} // namespace online::comms
