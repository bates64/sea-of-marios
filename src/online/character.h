#pragma once

#include "common.h"

enum PlayerCharacter {
    CHARACTER_GOOMBA = 1,
    CHARACTER_KOOPA,
    CHARACTER_BOBOMB,
    CHARACTER_PARAKOOPA,
    CHARACTER_BOO,
    CHARACTER_LIL_SPARKY,
    CHARACTER_CHEEP_CHEEP,
    CHARACTER_LAKITU,
    CHARACTER_MAGIKOOPA,
};

s32 character_idle_anim(s8 character);
s32 character_run_anim(s8 character);
s32 character_jump_anim(s8 character);
s32 character_hurt_anim(s8 character);
b32 character_is_flying(s8 character);
