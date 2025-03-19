#ifndef _WHIRLWIND_H_
#define _WHIRLWIND_H_

#include "common_structs.h"

#ifdef _LANGUAGE_C_PLUS_PLUS
extern "C" {
#endif

// args: variation
API_CALLABLE(action_command_whirlwind_init);

// args: prep time, duration, difficulty
API_CALLABLE(action_command_whirlwind_start);

void action_command_whirlwind_update(void);
void action_command_whirlwind_draw(void);
void action_command_whirlwind_free(void);

enum {
    ACV_WHIRLWIND_HUFF      = 0,
    ACV_WHIRLWIND_BUZZAR    = 1,
};

#ifdef _LANGUAGE_C_PLUS_PLUS
}
#endif

#endif
