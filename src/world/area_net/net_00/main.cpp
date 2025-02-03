#include "net_00.h"
#include "online/character.h"
#include "effects.h"
#include "dx/debug_menu.h"

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

PlayerTrait highest_trait() {
    s8 max = -1;
    PlayerTrait trait = OPPORTUNISTIC;

    for (int i = 0; i < NUM_TRAITS; i++) {
        if (quizWeights[i] > max) {
            max = quizWeights[i];
            trait = (PlayerTrait) i;
        }
    }
    return trait;
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
    gGameStatus.bestTrait = best_non_character_trait(character);
    change_player_character(character);
    return ApiStatus_DONE2;
}

#include "world/common/npc/StarSpirit.inc.c"

EvtScript EVS_Muskular_Appear = {
    Call(SetNpcPos, NPC_SELF, GEN_MUSKULAR_VEC)
    Call(PlaySoundAtNpc, NPC_SELF, SOUND_STAR_SPIRIT_APPEAR_A, SOUND_SPACE_DEFAULT)
    PlayEffect(EFFECT_SPARKLES, 0, GEN_MUSKULAR_VEC, 45, 0)
    Thread
        Call(MakeLerp, 0, 255, 80 * DT, EASING_LINEAR)
        Label(0)
            Call(UpdateLerp)
            Call(SetNpcImgFXParams, NPC_SELF, IMGFX_SET_ALPHA, LVar0, 0, 0, 0)
            Wait(1)
            IfEq(LVar1, 1)
                Goto(0)
            EndIf
    EndThread
    Thread
        Call(MakeLerp, 0, 2880, 80 * DT, EASING_QUADRATIC_OUT)
        Label(1)
            Call(UpdateLerp)
            Call(SetNpcRotation, NPC_SELF, 0, LVar0, 0)
            Wait(1)
            IfEq(LVar1, 1)
                Goto(1)
            EndIf
    EndThread
    Thread
        Call(MakeLerp, GEN_MUSKULAR_Y + 80, GEN_MUSKULAR_Y, 80 * DT, EASING_QUADRATIC_OUT)
        Label(2)
            Call(UpdateLerp)
            Call(SetNpcPos, NPC_SELF, GEN_MUSKULAR_X, LVar0, GEN_MUSKULAR_Z)
            Wait(1)
            IfEq(LVar1, 1)
                Goto(2)
            EndIf
    EndThread
    Wait(100 * DT)
    Return
    End
};

EvtScript EVS_Muskular_Depart = {
    Thread
        Loop(25)
            Call(GetNpcPos, NPC_SELF, LVar0, LVar1, LVar2)
            PlayEffect(EFFECT_SPARKLES, 4, LVar0, LVar1, LVar2, 20, 0, 0, 0, 0, 0, 0, 0, 0)
            Wait(4 * DT)
        EndLoop
    EndThread
    Thread
        Set(LVar2, 0)
        Set(LVar3, 1800)
        Call(MakeLerp, LVar2, LVar3, 100, EASING_CUBIC_IN)
        Loop(0)
            Call(UpdateLerp)
            Call(SetNpcRotation, NPC_SELF, 0, LVar0, 0)
            Wait(1)
            IfEq(LVar1, 0)
                BreakLoop
            EndIf
        EndLoop
    EndThread
    Thread
        Call(GetNpcPos, NPC_SELF, LVar2, LVar3, LVar4)
        Set(LVar5, LVar3)
        Add(LVar5, 180)
        Call(MakeLerp, LVar3, LVar5, 100 * DT, EASING_CUBIC_IN)
        Loop(0)
            Call(UpdateLerp)
            Call(SetNpcPos, NPC_SELF, LVar2, LVar0, LVar4)
            Wait(1)
            IfEq(LVar1, 0)
                BreakLoop
            EndIf
        EndLoop
        Call(SetNpcPos, NPC_SELF, NPC_DISPOSE_LOCATION)
    EndThread
    Thread
        Wait(15 * DT)
        Call(PlaySoundAtNpc, NPC_SELF, SOUND_STAR_SPIRIT_DEPART_1, SOUND_SPACE_DEFAULT)
    EndThread
    Wait(10 * DT)
    Return
    End
};

/// Set LVar0 to next question index, or -1 if all questions have been asked
API_CALLABLE(GetNextQuestion) {
    static bool questionAsked[NUM_TRAITS] = {0};

    // Check that there is an unasked question
    s32 i;
    for (i = 0; i < NUM_TRAITS; i++) {
        if (!questionAsked[i]) {
            break;
        }
    }
    if (i == NUM_TRAITS) {
        script->varTable[0] = -1;
        return ApiStatus_DONE2;
    }

    // Pick random question that hasn't been asked yet
    s32 question = rand_int(NUM_TRAITS - 1);
    while (questionAsked[question]) {
        question = rand_int(NUM_TRAITS - 1);
    }
    questionAsked[question] = TRUE;
    script->varTable[0] = question;
    return ApiStatus_DONE2;
}

EvtScript EVS_Muskular_Quiz = {
    Call(SpeakToPlayer, NPC_SELF, ANIM_WorldMuskular_Talk, ANIM_WorldMuskular_Still, 0, MSG_Personality_Start)
    Loop(0)
        Call(GetNextQuestion)
        Switch(LVar0)
            CaseEq(-1)
                BreakLoop
            CaseEq(OPPORTUNISTIC)
                Call(ContinueSpeech, NPC_SELF, ANIM_WorldMuskular_Talk, ANIM_WorldMuskular_Still, 0, MSG_Personality_Question_Opportunistic)
                Call(ShowChoice, MSG_Personality_Choice_Opportunistic)
                Switch(LVar0)
                    CaseEq(0) // better offer
                        Call(ChangeTraitWeight, OPPORTUNISTIC, 3)
                        Call(ChangeTraitWeight, IMPULSIVE, 1)
                    CaseEq(1) // convince them it's junk
                        Call(ChangeTraitWeight, STRATEGIC, 1)
                        Call(ChangeTraitWeight, MISCHIEVOUS, 1)
                    CaseEq(2) // walk away
                        Call(ChangeTraitWeight, OPPORTUNISTIC, -1)
                        Call(ChangeTraitWeight, GREEDY, -1)
                EndSwitch
            CaseEq(TENACIOUS)
                Call(ContinueSpeech, NPC_SELF, ANIM_WorldMuskular_Talk, ANIM_WorldMuskular_Still, 0, MSG_Personality_Question_Tenacious)
                Call(ShowChoice, MSG_Personality_Choice_Tenacious)
                Switch(LVar0)
                    CaseEq(0) // scared them off
                        Call(ChangeTraitWeight, STRATEGIC, 1)
                        Call(ChangeTraitWeight, TENACIOUS, 1)
                    CaseEq(1) // they ran out of supplies
                        Call(ChangeTraitWeight, TENACIOUS, 3)
                        Call(ChangeTraitWeight, RESILIENT, 1)
                    CaseEq(2) // could be a trap
                        Call(ChangeTraitWeight, CREATIVE, 1)
                        Call(ChangeTraitWeight, STRATEGIC, 1)
                        Call(ChangeTraitWeight, IMPULSIVE, -2)
                EndSwitch
            // TODO: IMPULSIVE
            CaseEq(PRECISE)
                Call(ContinueSpeech, NPC_SELF, ANIM_WorldMuskular_Talk, ANIM_WorldMuskular_Still, 0, MSG_Personality_Question_Precise)
                Call(ShowChoice, MSG_Personality_Choice_Precise)
                Switch(LVar0)
                    CaseEq(0) // yes
                        Call(ChangeTraitWeight, PRECISE, 3)
                        Call(ChangeTraitWeight, STRATEGIC, 1)
                    CaseEq(1) // no
                        Call(ChangeTraitWeight, OPPORTUNISTIC, 1)
                        Call(ChangeTraitWeight, METHODICAL, 1)
                    CaseEq(2) // whatever
                        Call(ChangeTraitWeight, TENACIOUS, -1)
                EndSwitch
            CaseEq(RESILIENT)
                Call(ContinueSpeech, NPC_SELF, ANIM_WorldMuskular_Talk, ANIM_WorldMuskular_Still, 0, MSG_Personality_Question_Resilient)
                Call(ShowChoice, MSG_Personality_Choice_Resilient)
                Switch(LVar0)
                    CaseEq(0) // patch it quickly
                        Call(ChangeTraitWeight, RESILIENT, 3)
                        Call(ChangeTraitWeight, OPPORTUNISTIC, 1)
                        Call(ChangeTraitWeight, PRECISE, -1)
                    CaseEq(1) // take time to repair it
                        Call(ChangeTraitWeight, METHODICAL, 2)
                        Call(ChangeTraitWeight, PRECISE, 1)
                        Call(ChangeTraitWeight, IMPULSIVE, -1)
                    CaseEq(2) // convince someone else
                        Call(ChangeTraitWeight, MISCHIEVOUS, 2)
                        Call(ChangeTraitWeight, STRATEGIC, 1)
                        Call(ChangeTraitWeight, TENACIOUS, -1)
                    CaseEq(3) // do other tasks first
                        Call(ChangeTraitWeight, STRATEGIC, 2)
                        Call(ChangeTraitWeight, METHODICAL, -1)
                        Call(ChangeTraitWeight, RESILIENT, -1)
                EndSwitch
            CaseEq(METHODICAL)
                Call(ContinueSpeech, NPC_SELF, ANIM_WorldMuskular_Talk, ANIM_WorldMuskular_Still, 0, MSG_Personality_Question_Methodical)
                Call(ShowChoice, MSG_Personality_Choice_Methodical)
                Switch(LVar0)
                    CaseEq(0) // good
                        Call(ChangeTraitWeight, PRECISE, -1)
                    CaseEq(1) // amazing
                        Call(ChangeTraitWeight, METHODICAL, 3)
                        Call(ChangeTraitWeight, RESILIENT, 1)
                        Call(ChangeTraitWeight, MISCHIEVOUS, 1) // joke answer
                EndSwitch
            CaseEq(STRATEGIC)
                Call(ContinueSpeech, NPC_SELF, ANIM_WorldMuskular_Talk, ANIM_WorldMuskular_Still, 0, MSG_Personality_Question_Strategic)
                Call(ShowChoice, MSG_Personality_Choice_Strategic)
                Switch(LVar0)
                    CaseEq(0) // ignore it
                        Call(ChangeTraitWeight, OPPORTUNISTIC, -1)
                        Call(ChangeTraitWeight, IMPULSIVE, -1)
                        Call(ChangeTraitWeight, RESILIENT, 1)
                    CaseEq(1) // think carefully
                        Call(ChangeTraitWeight, STRATEGIC, 3)
                        Call(ChangeTraitWeight, PRECISE, 1)
                    CaseEq(2) // monsters!
                        Call(ChangeTraitWeight, IMPULSIVE, 1)
                        Call(ChangeTraitWeight, TENACIOUS, 1)
                EndSwitch
            CaseEq(CREATIVE)
                Call(ContinueSpeech, NPC_SELF, ANIM_WorldMuskular_Talk, ANIM_WorldMuskular_Still, 0, MSG_Personality_Question_Creative)
                Call(ShowChoice, MSG_Personality_Choice_Creative)
                Switch(LVar0)
                    CaseEq(0) // exact
                        Call(ChangeTraitWeight, CREATIVE, -1)
                        Call(ChangeTraitWeight, METHODICAL, 3)
                    CaseEq(1) // tweak
                        Call(ChangeTraitWeight, STRATEGIC, 2)
                    CaseEq(2) // different
                        Call(ChangeTraitWeight, CREATIVE, 3)
                        Call(ChangeTraitWeight, MISCHIEVOUS, 1)
                EndSwitch
            CaseEq(MISCHIEVOUS)
                Call(ContinueSpeech, NPC_SELF, ANIM_WorldMuskular_Talk, ANIM_WorldMuskular_Still, 0, MSG_Personality_Question_Mischievous)
                Call(ShowChoice, MSG_Personality_Choice_Mischievous)
                Switch(LVar0)
                    CaseEq(0) // haha
                        Call(ChangeTraitWeight, MISCHIEVOUS, 4)
                    CaseEq(1) // save them
                        Call(ChangeTraitWeight, OPPORTUNISTIC, 1)
                    CaseEq(2) // let them die
                        Call(ChangeTraitWeight, STRATEGIC, 1)
                        Call(ChangeTraitWeight, MISCHIEVOUS, 1)
                EndSwitch
            CaseEq(GREEDY)
                Call(ContinueSpeech, NPC_SELF, ANIM_WorldMuskular_Talk, ANIM_WorldMuskular_Still, 0, MSG_Personality_Question_Greedy)
                Call(ShowChoice, MSG_Personality_Choice_Greedy)
                Switch(LVar0)
                    CaseEq(0) // yes
                        Call(ChangeTraitWeight, GREEDY, 3)
                    CaseEq(1) // never
                        Call(ChangeTraitWeight, GREEDY, -2)
                    CaseEq(2) // gave it back
                        Call(ChangeTraitWeight, IMPULSIVE, 1)
                EndSwitch
            CaseDefault
                DebugPrintf("Unknown question: %d", LVar0)
        EndSwitch
    EndLoop
    Return
    End
};

API_CALLABLE(GetPersonalityResultMessage) {
    s32 messages[] = {
        MSG_Personality_Result_Opportunistic,
        MSG_Personality_Result_Tenacious,
        MSG_Personality_Result_Impulsive,
        MSG_Personality_Result_Precise,
        MSG_Personality_Result_Resilient,
        MSG_Personality_Result_Methodical,
        MSG_Personality_Result_Strategic,
        MSG_Personality_Result_Creative,
        MSG_Personality_Result_Mischievous,
        MSG_Personality_Result_Greedy,
    };
    script->varTable[0] = messages[highest_trait()];
    return ApiStatus_DONE2;
}

API_CALLABLE(GetPersonalityCharacterResultMessage) {
    s32 messages[] = {
        MSG_Personality_CharacterResult_Goomba,
        MSG_Personality_CharacterResult_Koopa,
        MSG_Personality_CharacterResult_BobOmb,
        MSG_Personality_CharacterResult_Parakoopa,
        MSG_Personality_CharacterResult_Boo,
        MSG_Personality_CharacterResult_Lakitu,
        MSG_Personality_CharacterResult_CheepCheep,
        MSG_Personality_CharacterResult_Bandit,
        MSG_Personality_CharacterResult_DryBones,
        MSG_Personality_CharacterResult_Fuzzy,
    };
    script->varTable[0] = messages[find_closest_character()];
    return ApiStatus_DONE2;
}

EvtScript EVS_NpcInit_Muskular = {
    Thread
        ExecWait(EVS_Muskular_Appear)
        ExecWait(EVS_Muskular_Quiz)

        WaitSecs(1)

        Call(EndQuiz)

        Call(FadeOutMusic, 0, 5000)

        // "A $trait person like you would be..."
        Call(GetPersonalityResultMessage)
        Call(ContinueSpeech, NPC_SELF, ANIM_WorldMuskular_Talk, ANIM_WorldMuskular_Still, 0, LVar0)

        Call(PlaySound, SOUND_STAR_SPIRIT_CAST_A)
        PlayEffect(EFFECT_SPARKLES, 0, 0, -50, 200, 10)
        PlayEffect(EFFECT_STARS_SHIMMER, 3, 0, -50, 200, 42, 48, 15, 60, 0)
        PlayEffect(EFFECT_RADIAL_SHIMMER, 9, 0, -50, 200, Float(1.0), 100)
        Call(SetPlayerPos, 0, -50, 200)

        Call(SetMusicTrack, 0, SONG_KOOPA_BROS_THEME, 0, 8)

        // "A $character!"
        Call(GetPersonalityCharacterResultMessage)
        Call(ContinueSpeech, NPC_SELF, ANIM_WorldMuskular_Talk, ANIM_WorldMuskular_Still, 0, LVar0)

        // TODO: naming

        /*
        Call(ContinueSpeech, NPC_SELF, ANIM_WorldMuskular_Talk, ANIM_WorldMuskular_Still, 0, MSG_Personality_End)
        // TODO: name prompt
        Call(EndSpeech, NPC_SELF)*/

        WaitSecs(2)
        ExecWait(EVS_Muskular_Depart)

        WaitSecs(3)

        Call(GotoMapSpecial, Ref("mac_05"), 0, TRANSITION_SLOW_FADE_TO_WHITE)
    EndThread
    Return
    End
};

NpcData NpcData_Muskular = {
    .id = NPC_Muskular,
    .settings = &NpcSettings_StarSpirit,
    .pos = { NPC_DISPOSE_LOCATION },
    .flags = COMMON_PASSIVE_FLAGS | NPC_FLAG_IGNORE_CAMERA_FOR_YAW,
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
    Call(DisablePlayerPhysics, TRUE)
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
