#include "net_00.h"
#include "online/character.h"

namespace net_00 {

EntryList Entrances = { GEN_ENTRY_LIST };

#include "world/common/atomic/TexturePan.inc.c"

EvtScript EVS_TexPan = {
    SetGroup(EVT_GROUP_NEVER_PAUSE)
    Call(SetTexPanner, MODEL_bg, TEX_PANNER_C)
    Thread
        TEX_PAN_PARAMS_ID(TEX_PANNER_C)
        TEX_PAN_PARAMS_STEP(  -64,  -60,   32,   16)
        TEX_PAN_PARAMS_FREQ(    1,    1,    1,    1)
        TEX_PAN_PARAMS_INIT(    0,    0,    0,    0)
        Exec(EVS_UpdateTexturePan)
    EndThread
    Return
    End
};

typedef s8 Weights[NUM_TRAITS];

Weights characterToWeights[] = {
    [CHARACTER_GOOMBA] = {2, 0, 1, -1, 0, 0, 0, 0, 0, 0},
    [CHARACTER_KOOPA] = {0, 0, 0, -1, 2, 1, 0, 0, 0, 0},
    [CHARACTER_BOBOMB] = {0, 1, 2, 0, 0, 0, 0, -1, 0, 0},
    [CHARACTER_PARAKOOPA] = {-1, 0, 0, 0, 0, 2, 1, 0, 0, 0},
    [CHARACTER_BOO] = {0, 0, 0, 0, -1, 0, 2, 0, 1, 0},
    [CHARACTER_LAKITU] = {0, 1, 0, 0, 0, 0, 0, 2, 0, -1},
    [CHARACTER_CHEEP_CHEEP] = {0, 0, 0, 2, 0, 1, 0, -1, 0, 0},
    [CHARACTER_BANDIT] = {0, 0, 0, 0, 0, 0, -1, 0, 1, 2},
    [CHARACTER_DRY_BONES] = {0, 2, -1, 0, 1, 0, 0, 0, 0, 0},
    [CHARACTER_FUZZY] = {1, 0, 0, 0, 0, 0, 0, 0, 2, -1},
};

Weights quizWeights = {0};

u32 manhattan_distance(s8 a[], s8 b[]) {
    u32 sum = 0;
    for (int i = 0; i < NUM_TRAITS; i++) {
        if (a[i] != 0 || b[i] != 0) { // Ignore traits that are 0 in both
            sum += abs((s32)a[i] - (s32)b[i]);
        }
    }
    return sum;
}

PlayerCharacter find_closest_character() {
    u32 minDist = 0xFFFFFFFF;
    PlayerCharacter best = CHARACTER_GOOMBA;

    for (int i = 0; i < NUM_CHARACTERS; i++) {
        u32 dist = manhattan_distance(quizWeights, characterToWeights[i]);
        if (dist < minDist) {
            minDist = dist;
            best = (PlayerCharacter) i;
        }
    }
    return best;
}

/// Finds the highest quizWeights trait, ignoring the trait that is dominant in the character.
/// e.g. will never return OPPORTUNISTIC for Goomba
PlayerTrait best_non_character_trait(PlayerCharacter character) {
    s8* characterWeights = characterToWeights[character];
    PlayerTrait bestTrait = OPPORTUNISTIC;
    s8 bestWeight = -1;

    for (int i = 0; i < NUM_TRAITS; i++) {
        if (characterWeights[i] == 0 && quizWeights[i] > bestWeight) {
            bestWeight = quizWeights[i];
            bestTrait = (PlayerTrait) i;
        }
    }
    return bestTrait;
}

API_CALLABLE(ChangeTraitWeight) {
    Bytecode* args = script->ptrReadPos;
    PlayerTrait trait = (PlayerTrait) evt_get_variable(script, *args++);
    s32 value = evt_get_variable(script, *args++);
    quizWeights[trait] += value;
    return ApiStatus_DONE2;
}

API_CALLABLE(EndQuiz) {
    PlayerCharacter character = find_closest_character();
    gGameStatus.character = character;
    gGameStatus.bestTrait = best_non_character_trait(character);
    return ApiStatus_DONE2;
}

#include "world/common/npc/StarSpirit.inc.c"

EvtScript EVS_NpcInit_Muskular = {
    // TODO: actual quiz
    Call(ChangeTraitWeight, CREATIVE, 10)
    Call(EndQuiz)
    Return
    End
};

NpcData NpcData_Muskular = {
    .id = NPC_Muskular,
    .settings = &NpcSettings_StarSpirit,
    .pos = { GEN_MUSKULAR_VEC },
    .flags = COMMON_PASSIVE_FLAGS,
    .init = &EVS_NpcInit_Muskular,
    .yaw = 0,
    .drops = NO_DROPS,
    .animations = MUSKULAR_ANIMS,
};

NpcGroupList DefaultNPCs = {
    NPC_GROUP(N(NpcData_Muskular)),
    {},
};

EvtScript EVS_Main = {
    Call(SetSpriteShading, SHADING_NONE)
    EVT_SETUP_CAMERA_NO_LEAD(0, 0, 0)
    Call(SetMusicTrack, 0, SONG_SHOOTING_STAR_SUMMIT, 0, 8)
    Exec(EVS_TexPan)
    Call(DisablePlayerInput, TRUE)
    Call(MakeNpcs, TRUE, Ref(DefaultNPCs))
    Return
    End
};

}; // namespace net_00

MapSettings net_00_settings = {
    .main = &net_00::EVS_Main,
    .entryList = &net_00::Entrances,
    .entryCount = ENTRY_COUNT(net_00::Entrances),
    .background = &gBackgroundImage,
};

 s32 net_00_map_init(void) {
    sprintf(wMapTexName, "hos_tex");
    return FALSE;
 }
