#include "common.h"
#include "dx/debug_menu.h"
#include "online.h"

namespace online::comms {

Message message;
volatile s32 changeMeToExchangeMessage = 0; // GDB will place a write breakpoint on this

struct Header {
    char magic[0x20];
    volatile s32* changeMeToExchangeMessage;
    Message* message;
};

// .text section is linked first, so header will be placed at the start of this segment
[[gnu::section(".text")]] Header header = {
    "PAPERMARIO-DX ONLINE",
    &changeMeToExchangeMessage,
    &message,
};

void begin_step() {
    static b32 needsInit = TRUE;
    if (needsInit) {
        // This would be better off as constructor initialization, but we don't support global constructors yet.
        // See: .init_array section being discarded in the linker script.
        message = Message();
        needsInit = FALSE;
    }

    debug_printf("connected? %d", message.is_connected_to_client());
}

void end_step() {
    message.encode();
}

Message& get_message() {
    return message;
}

Message::Message() {
    ASSERT_MSG((u32)&header == 0x80700000U, "online::comms::header placed at %p\nbut it should be at %p", &header, (void*)0x80700000);

    // Write nil message so initial decode() doesn't read garbage
    reset_writer();
    encode();
}

void Message::decode() {
    mpack_reader_t reader;
    mpack_reader_init_data(&reader, rawMessage, sizeof(rawMessage));

    // Read until nil
    while (mpack_peek_tag(&reader).type != mpack_type_nil) {
        online::rpc::read::mpack_read_rpc(&reader);
    }
    mpack_expect_nil(&reader);

    mpack_error_t error = mpack_reader_destroy(&reader);
    if (error != mpack_ok) {
        debug_printf("MPack read error: %s\n", mpack_error_to_string(error));
    }
}

void Message::reset_writer() {
    mpack_writer_init(&writer, rawMessage, sizeof(rawMessage));
    mpack_writer_set_error_handler(&writer, [](mpack_writer_t* writer, mpack_error_t error) {
        debug_printf("MPack write error: %s\n", mpack_error_to_string(error));
    });
    // During the frame, the user will write RPCs using online::rpc:: functions.
}

void Message::encode() {
    // End of message marker
    mpack_write_nil(&writer);

    length = mpack_writer_buffer_used(&writer);
    mpack_error_t error = mpack_writer_destroy(&writer);
    if (error != mpack_ok) {
        debug_printf("MPack write error: %s\n", mpack_error_to_string(error));

        // Write nil on its own instead
        reset_writer();
        mpack_write_nil(&writer);
        length = mpack_writer_buffer_used(&writer);
        error = mpack_writer_destroy(&writer);
        ASSERT(error != mpack_ok);
    }

    reset_writer();

    // If GDB client is connected, this will exchange messages with it.
    changeMeToExchangeMessage = rand_int(0x7FFFFFFF);

    osInvalICache((void*) &length, sizeof(length));
    osInvalICache(&rawMessage, sizeof(rawMessage));

    // GDB will set length to 0 after it reads the message.
    // Therefore if it is not 0, GDB is disconnected or misbehaving.
    isGdbConnected = length == 0;

    // If GDB is connected, we should have received a new message.
    if (isGdbConnected) {
        message.decode();
    }
};

} // namespace online::comms
