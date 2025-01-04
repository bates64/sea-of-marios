#include "common.h"
#include "mpack.h"

void mpack_assert_fail(const char* message) {
    debug_printf("MPack: %s\n", message);

    PANIC_MSG("MPack: %s\n", message);
}

// Break = bad, but not fatal
void mpack_break_hit(const char* message) {
    debug_printf("MPack break: %s\n", message);
}
