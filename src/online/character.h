#pragma once

#include "common.h"

enum PlayerCharacter {
    CHARACTER_GOOMBA,
    CHARACTER_KOOPA,
    CHARACTER_BOBOMB,
    CHARACTER_PARAKOOPA,
    CHARACTER_BOO,
    CHARACTER_LAKITU,
    CHARACTER_CHEEP_CHEEP,
    CHARACTER_BANDIT,
    CHARACTER_DRY_BONES,
    CHARACTER_FUZZY,

    NUM_CHARACTERS
};

enum PlayerTrait {
    OPPORTUNISTIC,
    TENACIOUS,
    IMPULSIVE,
    PRECISE,
    RESILIENT,
    METHODICAL,
    STRATEGIC,
    CREATIVE,
    MISCHIEVOUS,
    GREEDY,

    NUM_TRAITS
};

EXTERN_C void change_player_character(enum PlayerCharacter character);
EXTERN_C s32 character_idle_anim(s8 character);
EXTERN_C s32 character_run_anim(s8 character);
EXTERN_C s32 character_jump_anim(s8 character);
EXTERN_C s32 character_hurt_anim(s8 character);
EXTERN_C b32 character_is_flying(s8 character);
