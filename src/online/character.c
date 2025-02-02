#include "character.h"

#include "sprite/npc/Goomba.h"
#include "sprite/npc/KoopaTroopa.h"
#include "sprite/npc/Bobomb.h"
#include "sprite/npc/ParaTroopa.h"
#include "sprite/npc/Boo.h"
#include "sprite/npc/WorldWatt.h"
#include "sprite/npc/CheepCheep.h"
#include "sprite/npc/Lakitu.h"
#include "sprite/npc/Magikoopa.h"

static struct {
    s32 idle;
    s32 run;
    s32 jump;
    b32 flying;
} characters[] = {
    [CHARACTER_GOOMBA] = {ANIM_Goomba_Idle, ANIM_Goomba_Run, ANIM_Goomba_Midair, FALSE},
    [CHARACTER_KOOPA] = {ANIM_KoopaTroopa_Idle, ANIM_KoopaTroopa_Run, ANIM_KoopaTroopa_Walk, FALSE},
    [CHARACTER_BOBOMB] = {ANIM_Bobomb_Idle, ANIM_Bobomb_Run, ANIM_Bobomb_Idle, FALSE},
    [CHARACTER_PARAKOOPA] = {ANIM_ParaTroopa_Idle, ANIM_ParaTroopa_Run, ANIM_ParaTroopa_Walk, TRUE},
    [CHARACTER_BOO] = {ANIM_Boo_Idle, ANIM_Boo_Run, ANIM_Boo_Walk, TRUE},
    [CHARACTER_LAKITU] = {ANIM_Lakitu_Anim00, ANIM_Lakitu_Anim02, ANIM_Lakitu_Anim01, TRUE},
    [CHARACTER_CHEEP_CHEEP] = {ANIM_CheepCheep_Idle, ANIM_CheepCheep_Run, ANIM_CheepCheep_Jump, FALSE},
    [CHARACTER_BANDIT] = {ANIM_WorldWatt_Idle, ANIM_WorldWatt_Run, ANIM_WorldWatt_Walk, TRUE},
    [CHARACTER_DRY_BONES] = {ANIM_Magikoopa_Anim00, ANIM_Magikoopa_Anim02, ANIM_Magikoopa_Anim01, FALSE},
    [CHARACTER_FUZZY] = {ANIM_Magikoopa_Anim00, ANIM_Magikoopa_Anim02, ANIM_Magikoopa_Anim01, FALSE},
};

s32 character_idle_anim(s8 character) {
    return characters[character].idle;
}

s32 character_run_anim(s8 character) {
    return characters[character].run;
}

s32 character_jump_anim(s8 character) {
    return characters[character].jump;
}

s32 character_hurt_anim(s8 character) {
    return characters[character].idle; // TODO
}

b32 character_is_flying(s8 character) {
    return characters[character].flying;
}
