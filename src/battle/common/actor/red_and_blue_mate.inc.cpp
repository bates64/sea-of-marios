#include "battle/battle.h"
#include "script_api/battle.h"
#include "effects.h"
#include "sprite/npc/Goomates.h"

namespace battle::actor {

namespace red_mate {

extern s32 DefaultAnims[];
extern EvtScript EVS_Init;
extern EvtScript EVS_Init_Inner;
extern EvtScript EVS_Idle;
extern EvtScript EVS_TakeTurn;
extern EvtScript EVS_HandleEvent;
extern EvtScript EVS_Attack_Headbonk;
extern EvtScript EVS_Attack_Firebonk;
extern EvtScript EVS_Attack_FireBurst;

enum ActorPartIDs {
    PRT_MAIN        = 1,
};

// Red Mate Actor Stats
constexpr s32 hp = 3;
constexpr s32 dmgHeadbonkWeak = 1;
constexpr s32 dmgHeadbonk = 2;
constexpr s32 dmgFirebonkWeak = 1;
constexpr s32 dmgFirebonk = 2;
constexpr s32 dmgBackFirebonkWeak = 1;
constexpr s32 dmgBackFirebonk = 2;
constexpr s32 dmgFireBurst = 2;

enum ActorVars {
    AVAR_RedDead,
    AVAR_HitBlue,
    AVAR_BlueDead,
    AVAR_Weakened,
};

// enum ActorParams {
//     DMG_HEADBONK    = 1,
// };

s32 DefenseTable[] = {
    ELEMENT_NORMAL,   1,
    ELEMENT_END,
};

s32 StatusTable[] = {
    STATUS_KEY_NORMAL,              0,
    STATUS_KEY_DEFAULT,             0,
    STATUS_KEY_SLEEP,             100,
    STATUS_KEY_POISON,              0,
    STATUS_KEY_FROZEN,              0,
    STATUS_KEY_DIZZY,             100,
    STATUS_KEY_FEAR,                0,
    STATUS_KEY_STATIC,              0,
    STATUS_KEY_PARALYZE,          100,
    STATUS_KEY_SHRINK,            100,
    STATUS_KEY_STOP,              100,
    STATUS_TURN_MOD_DEFAULT,        0,
    STATUS_TURN_MOD_SLEEP,          0,
    STATUS_TURN_MOD_POISON,         0,
    STATUS_TURN_MOD_FROZEN,         0,
    STATUS_TURN_MOD_DIZZY,          0,
    STATUS_TURN_MOD_FEAR,           0,
    STATUS_TURN_MOD_STATIC,         0,
    STATUS_TURN_MOD_PARALYZE,       0,
    STATUS_TURN_MOD_SHRINK,         0,
    STATUS_TURN_MOD_STOP,           0,
    STATUS_END,
};

ActorPartBlueprint ActorParts[] = {
    {
        .flags = ACTOR_PART_FLAG_PRIMARY_TARGET,
        .index = PRT_MAIN,
        .posOffset = { 0, 0, 0 },
        .targetOffset = { 0, 24 },
        .opacity = 255,
        .idleAnimations = DefaultAnims,
        .defenseTable = DefenseTable,
        .eventFlags = 0,
        .elementImmunityFlags = ELEMENT_FIRE,
        .projectileTargetOffset = { 0, -3 },
    },
};

s32 DefaultAnims[] = {
    STATUS_KEY_NORMAL,    ANIM_Goomates_Red_Idle,
    STATUS_KEY_STONE,     ANIM_Goomates_Red_Still,
    STATUS_KEY_SLEEP,     ANIM_Goomates_Red_Sleep,
    STATUS_KEY_POISON,    ANIM_Goomates_Red_Idle,
    STATUS_KEY_STOP,      ANIM_Goomates_Red_Still,
    STATUS_KEY_STATIC,    ANIM_Goomates_Red_Idle,
    STATUS_KEY_PARALYZE,  ANIM_Goomates_Red_Still,
    STATUS_KEY_DIZZY,     ANIM_Goomates_Red_Dizzy,
    STATUS_KEY_FEAR,      ANIM_Goomates_Red_Dizzy,
    STATUS_END,
};

// while shuffling around during idle
s32 ShuffleAnims[] = {
    STATUS_KEY_NORMAL,    ANIM_Goomates_Red_Run,
    STATUS_KEY_STONE,     ANIM_Goomates_Red_Still,
    STATUS_KEY_SLEEP,     ANIM_Goomates_Red_Sleep,
    STATUS_KEY_POISON,    ANIM_Goomates_Red_Idle,
    STATUS_KEY_STOP,      ANIM_Goomates_Red_Still,
    STATUS_KEY_STATIC,    ANIM_Goomates_Red_Run,
    STATUS_KEY_PARALYZE,  ANIM_Goomates_Red_Still,
    STATUS_KEY_DIZZY,     ANIM_Goomates_Red_Dizzy,
    STATUS_KEY_FEAR,      ANIM_Goomates_Red_Dizzy,
    STATUS_END,
};

s32 FireAnims[] = {
    STATUS_KEY_NORMAL,    ANIM_Goomates_Red_Fire_Idle,
    STATUS_END,
};

EvtScript EVS_Init = {
    ExecWait(EVS_Init_Inner)
    Return
    End
};

EvtScript EVS_Init_Inner = {
    Call(BindTakeTurn, ACTOR_SELF, Ref(EVS_TakeTurn))
    Call(BindIdle, ACTOR_SELF, Ref(EVS_Idle))
    Call(BindHandleEvent, ACTOR_SELF, Ref(EVS_HandleEvent))
    // Call(SetActorVar, ACTOR_SELF, AVAR_RedDead, FALSE)
    Call(SetActorVar, ACTOR_SELF, AVAR_HitBlue, FALSE)
    Call(SetActorVar, ACTOR_SELF, AVAR_BlueDead, FALSE)
    Call(SetActorVar, ACTOR_SELF, AVAR_Weakened, FALSE)
    Return
    End
};

EvtScript EVS_Idle = {
    // Call(GetActorVar, ACTOR_SELF, AVAR_BlueDead, LVar0)
    // IfEq(LVar0, TRUE)
    //     Call(SetIdleAnimations, ACTOR_SELF, PRT_MAIN, Ref(FireAnims))
    // Else
        Label(10)
            Call(RandInt, 80, LVar0)
            Add(LVar0, 80)
            Loop(LVar0)
                Label(0)
                    Call(GetStatusFlags, ACTOR_SELF, LVar1)
                    IfFlag(LVar1, STATUS_FLAGS_IMMOBILIZED)
                        Wait(1)
                        Goto(0)
                    EndIf
                Wait(1)
            EndLoop
            Call(GetActorPos, ACTOR_SELF, LVar0, LVar1, LVar2)
            Add(LVar0, 5)
            Call(SetActorIdleSpeed, ACTOR_SELF, Float(1.0))
            Call(SetIdleAnimations, ACTOR_SELF, PRT_MAIN, Ref(ShuffleAnims))
            Call(SetIdleGoal, ACTOR_SELF, LVar0, LVar1, LVar2)
            Call(IdleRunToGoal, ACTOR_SELF, 0)
            Call(SetIdleAnimations, ACTOR_SELF, PRT_MAIN, Ref(DefaultAnims))
            Loop(20)
                Label(1)
                    Call(GetStatusFlags, ACTOR_SELF, LVar1)
                    IfFlag(LVar1, STATUS_FLAGS_IMMOBILIZED)
                        Wait(1)
                        Goto(1)
                    EndIf
                Wait(1)
            EndLoop
            Call(GetActorPos, ACTOR_SELF, LVar0, LVar1, LVar2)
            Sub(LVar0, 5)
            Call(SetActorIdleSpeed, ACTOR_SELF, Float(1.0))
            Call(SetIdleAnimations, ACTOR_SELF, PRT_MAIN, Ref(ShuffleAnims))
            Call(SetIdleGoal, ACTOR_SELF, LVar0, LVar1, LVar2)
            Call(IdleRunToGoal, ACTOR_SELF, 0)
            Call(SetIdleAnimations, ACTOR_SELF, PRT_MAIN, Ref(DefaultAnims))
            Loop(80)
                Label(2)
                    Call(GetStatusFlags, ACTOR_SELF, LVar1)
                    IfFlag(LVar1, STATUS_FLAGS_IMMOBILIZED)
                        Wait(1)
                        Goto(2)
                    EndIf
                Wait(1)
            EndLoop
            Goto(10)
    // EndIf
    Return
    End
};

EvtScript EVS_HandleEvent = {
    Call(UseIdleAnimation, ACTOR_SELF, FALSE)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)
    Call(SetActorScale, ACTOR_SELF, Float(1.0), Float(1.0), Float(1.0))
    Call(GetLastEvent, ACTOR_SELF, LVar0)
    Switch(LVar0)
        CaseOrEq(EVENT_HIT_COMBO)
        CaseOrEq(EVENT_HIT)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Red_Hurt)
            ExecWait(EVS_Enemy_Hit)
        EndCaseGroup
        CaseEq(EVENT_SPIN_SMASH_HIT)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Red_Hurt)
            ExecWait(EVS_Enemy_SpinSmashHit)
        CaseEq(EVENT_SPIN_SMASH_DEATH)
            Call(SetActorVar, ACTOR_ENEMY0, 1, TRUE)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Red_Hurt)
            ExecWait(EVS_Enemy_SpinSmashHit)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Red_Hurt)
            ExecWait(EVS_Enemy_Death)
            Return
        CaseEq(EVENT_SHOCK_HIT)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Red_Hurt)
            ExecWait(EVS_Enemy_ShockHit)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Red_Hurt)
            ExecWait(EVS_Enemy_Knockback)
            Call(JumpToGoal, ACTOR_SELF, 5, FALSE, TRUE, FALSE)
            Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(2.0))
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Dizzy)
            Call(SetGoalToHome, ACTOR_SELF)
            Call(SetActorSpeed, ACTOR_SELF, Float(8.0))
            Call(RunToGoal, ACTOR_SELF, 0, FALSE)
            Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(1.0))
            Wait(5)
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Idle)
            Call(SetActorJumpGravity, ACTOR_SELF, Float(1.6))
            Call(JumpToGoal, ACTOR_SELF, 5, FALSE, TRUE, FALSE)
        CaseEq(EVENT_SHOCK_DEATH)
            Call(SetActorVar, ACTOR_ENEMY0, 1, TRUE)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Red_Hurt)
            ExecWait(EVS_Enemy_ShockHit)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Red_Hurt)
            ExecWait(EVS_Enemy_Death)
            Return
        CaseEq(EVENT_STAR_BEAM)
            // do nothing
        CaseOrEq(EVENT_ZERO_DAMAGE)
        CaseOrEq(EVENT_IMMUNE)
        CaseOrEq(EVENT_AIR_LIFT_FAILED)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Red_Idle)
            ExecWait(EVS_Enemy_NoDamageHit)
        EndCaseGroup
        CaseEq(EVENT_DEATH)
            Call(SetActorVar, ACTOR_ENEMY0, 1, TRUE)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Red_Hurt)
            ExecWait(EVS_Enemy_Hit)
            Wait(10)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Red_Hurt)
            ExecWait(EVS_Enemy_Death)
            Return
        CaseEq(EVENT_END_FIRST_STRIKE)
            Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(2.0))
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Run)
            Call(SetGoalToHome, ACTOR_SELF)
            Call(SetActorSpeed, ACTOR_SELF, Float(4.0))
            Call(RunToGoal, ACTOR_SELF, 0, FALSE)
            Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(1.0))
            Call(HPBarToHome, ACTOR_SELF)
        CaseEq(EVENT_RECOVER_STATUS)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Red_Idle)
            ExecWait(EVS_Enemy_Recover)
        CaseEq(EVENT_SCARE_AWAY)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Red_Run)
            SetConst(LVar2, ANIM_Goomates_Red_Hurt)
            ExecWait(EVS_Enemy_ScareAway)
            Return
        CaseEq(EVENT_BEGIN_AIR_LIFT)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Red_Run)
            ExecWait(EVS_Enemy_AirLift)
        CaseEq(EVENT_BLOW_AWAY)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Red_Hurt)
            ExecWait(EVS_Enemy_BlowAway)
            Return
        CaseDefault
    EndSwitch
    Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Idle)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, TRUE)
    Return
    End
};

EvtScript EVS_TakeTurn = {
    Call(UseIdleAnimation, ACTOR_SELF, FALSE)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)
    Call(GetActorVar, ACTOR_SELF, AVAR_BlueDead, LVar3)
    IfEq(LVar3, FALSE)
        Call(GetActorVar, ACTOR_SELF, AVAR_HitBlue, LVar4)
        IfEq(LVar4, FALSE)
            ExecWait(EVS_Attack_Headbonk)
        Else
            ExecWait(EVS_Attack_Firebonk)
        EndIf
    Else
        Call(UseBattleCamPreset, BTL_CAM_ACTOR)
        Call(BattleCamTargetActor, ACTOR_SELF)
        Call(MoveBattleCamOver, 30)
        Wait(30)
        // Call(SetIdleAnimations, ACTOR_SELF, PRT_MAIN, Ref(FireAnims))
        // Call(AddActorDecoration, ACTOR_SELF, PRT_MAIN, 0, ACTOR_DECORATION_RED_FLAMES)
        // Call(ModifyActorDecoration, ACTOR_SELF, PRT_MAIN, 0, 100, 100, 0, 0)
        ExecWait(EVS_Attack_FireBurst)
    EndIf
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, TRUE)
    Return
    End
};

#include "common/CalculateArcsinDeg.inc.c"

EvtScript EVS_Attack_Headbonk = {
    Call(UseIdleAnimation, ACTOR_SELF, FALSE)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)
    Call(SetTargetActor, ACTOR_SELF, ACTOR_PLAYER)
    Call(UseBattleCamPreset, BTL_CAM_ENEMY_APPROACH)
    Call(BattleCamTargetActor, ACTOR_SELF)
    Call(SetBattleCamTargetingModes, BTL_CAM_YADJ_TARGET, BTL_CAM_XADJ_AVG, FALSE)
    Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Run)
    Call(SetGoalToTarget, ACTOR_SELF)
    Call(AddGoalPos, ACTOR_SELF, 50, 0, 0)
    Call(SetActorSpeed, ACTOR_SELF, Float(6.0))
    Call(RunToGoal, ACTOR_SELF, 0, FALSE)
    Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Idle)
    Call(SetActorDispOffset, ACTOR_SELF, 0, -1, 0)
    Wait(1)
    Call(SetActorDispOffset, ACTOR_SELF, 0, -2, 0)
    Wait(5)
    Call(SetActorDispOffset, ACTOR_SELF, 0, 0, 0)
    Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Midair)
    Call(EnemyTestTarget, ACTOR_SELF, LVar0, 0, 0, 1, BS_FLAGS1_INCLUDE_POWER_UPS)
    Switch(LVar0)
        CaseOrEq(HIT_RESULT_MISS)
        CaseOrEq(HIT_RESULT_LUCKY)
            Set(LVarA, LVar0)
            Call(SetGoalToTarget, ACTOR_SELF)
            Call(GetGoalPos, ACTOR_SELF, LVar0, LVar1, LVar2)
            Sub(LVar0, 10)
            Set(LVar1, 10)
            Add(LVar2, 3)
            Call(SetGoalPos, ACTOR_SELF, LVar0, LVar1, LVar2)
            Call(SetActorJumpGravity, ACTOR_SELF, Float(1.2))
            Thread
                Call(GetActorPos, ACTOR_SELF, LVar1, LVar2, LVar0)
                Set(LVar0, 0)
                Loop(16)
                    Call(GetActorPos, ACTOR_SELF, LVar4, LVar5, LVar6)
                    Call(N(CalculateArcsinDeg), LVar1, LVar2, LVar4, LVar5, LVar0)
                    Call(SetActorRotation, ACTOR_SELF, 0, 0, LVar0)
                    Set(LVar1, LVar4)
                    Set(LVar2, LVar5)
                    Set(LVar3, LVar6)
                    Wait(1)
                EndLoop
            EndThread
            Thread
                Wait(6)
                Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Midair)
            EndThread
            Call(JumpToGoal, ACTOR_SELF, 16, FALSE, TRUE, FALSE)
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Sleep)
            Call(SetActorScale, ACTOR_SELF, Float(1.1), Float(0.8), Float(1.0))
            Call(SetActorDispOffset, ACTOR_SELF, 0, 5, 0)
            Wait(1)
            Call(SetActorScale, ACTOR_SELF, Float(1.3), Float(0.5), Float(1.0))
            Call(SetActorDispOffset, ACTOR_SELF, 0, -2, 0)
            Wait(1)
            Call(SetActorScale, ACTOR_SELF, Float(1.0), Float(1.0), Float(1.0))
            Call(SetActorDispOffset, ACTOR_SELF, 0, 7, 0)
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Hurt)
            Wait(5)
            IfEq(LVarA, HIT_RESULT_LUCKY)
                Call(EnemyTestTarget, ACTOR_SELF, LVar0, DAMAGE_TYPE_TRIGGER_LUCKY, 0, 0, 0)
            EndIf
            Wait(5)
            Call(SetActorDispOffset, ACTOR_SELF, 0, 0, 0)
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Midair)
            Call(SetGoalToTarget, ACTOR_SELF)
            Call(GetGoalPos, ACTOR_SELF, LVar0, LVar1, LVar2)
            Add(LVar0, 20)
            Set(LVar1, 0)
            Call(SetGoalPos, ACTOR_SELF, LVar0, LVar1, LVar2)
            Call(SetActorJumpGravity, ACTOR_SELF, Float(2.0))
            Thread
                Wait(4)
                Set(LVar0, 180)
                Loop(4)
                    Sub(LVar0, 45)
                    Call(SetActorRotation, ACTOR_SELF, 0, 0, LVar0)
                    Wait(1)
                EndLoop
                Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Midair)
            EndThread
            Call(JumpToGoal, ACTOR_SELF, 15, FALSE, TRUE, FALSE)
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Dizzy)
            Wait(5)
            Call(UseBattleCamPreset, BTL_CAM_DEFAULT)
            Call(YieldTurn)
            Call(SetActorYaw, ACTOR_SELF, 180)
            Call(AddActorDecoration, ACTOR_SELF, PRT_MAIN, 0, ACTOR_DECORATION_SWEAT)
            Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(2.0))
            Call(SetGoalToHome, ACTOR_SELF)
            Call(SetActorSpeed, ACTOR_SELF, Float(8.0))
            Call(RunToGoal, ACTOR_SELF, 0, FALSE)
            Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(1.0))
            Call(SetActorYaw, ACTOR_SELF, 0)
            Wait(5)
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Idle)
            Call(SetActorJumpGravity, ACTOR_SELF, Float(1.6))
            Call(JumpToGoal, ACTOR_SELF, 5, FALSE, TRUE, FALSE)
            Call(RemoveActorDecoration, ACTOR_SELF, PRT_MAIN, 0)
            Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
            Call(UseIdleAnimation, ACTOR_SELF, TRUE)
            Return
        EndCaseGroup
        CaseDefault
            Call(SetGoalToTarget, ACTOR_SELF)
            Call(SetActorJumpGravity, ACTOR_SELF, Float(1.2))
            Thread
                Call(GetActorPos, ACTOR_SELF, LVar1, LVar2, LVar0)
                Set(LVar0, 0)
                Loop(16)
                    Call(GetActorPos, ACTOR_SELF, LVar4, LVar5, LVar6)
                    Call(N(CalculateArcsinDeg), LVar1, LVar2, LVar4, LVar5, LVar0)
                    Call(SetActorRotation, ACTOR_SELF, 0, 0, LVar0)
                    Set(LVar1, LVar4)
                    Set(LVar2, LVar5)
                    Set(LVar3, LVar6)
                    Wait(1)
                EndLoop
            EndThread
            Thread
                Wait(6)
                Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Midair)
            EndThread
            Call(JumpToGoal, ACTOR_SELF, 16, FALSE, TRUE, FALSE)
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Midair)
            Call(SetActorScale, ACTOR_SELF, Float(1.1), Float(0.8), Float(1.0))
            Wait(1)
            Call(SetActorScale, ACTOR_SELF, Float(1.3), Float(0.5), Float(1.0))
            Wait(1)
    EndSwitch
    // Call(GetActorVar, ACTOR_SELF, AVAR_Weakened, LVar6)
    // IfEq(LVar6, FALSE)
        Call(EnemyDamageTarget, ACTOR_SELF, LVar0, 0, 0, 0, dmgHeadbonk, BS_FLAGS1_TRIGGER_EVENTS)
    // Else
        Call(EnemyDamageTarget, ACTOR_SELF, LVar0, 0, 0, 0, dmgHeadbonkWeak, BS_FLAGS1_TRIGGER_EVENTS)
    // EndIf
    Switch(LVar0)
        CaseOrEq(HIT_RESULT_HIT)
        CaseOrEq(HIT_RESULT_NO_DAMAGE)
            Call(UseBattleCamPreset, BTL_CAM_DEFAULT)
            Call(SetActorScale, ACTOR_SELF, Float(1.1), Float(0.8), Float(1.0))
            Wait(1)
            Call(SetActorScale, ACTOR_SELF, Float(1.0), Float(1.0), Float(1.0))
            Wait(1)
            Call(SetActorRotation, ACTOR_SELF, 0, 0, 0)
            Call(SetActorDispOffset, ACTOR_SELF, 0, 0, 0)
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Idle)
            Call(GetGoalPos, ACTOR_SELF, LVar0, LVar1, LVar2)
            Add(LVar0, 40)
            Set(LVar1, 0)
            Call(SetActorJumpGravity, ACTOR_SELF, Float(1.8))
            Call(SetGoalPos, ACTOR_SELF, LVar0, LVar1, LVar2)
            Call(JumpToGoal, ACTOR_SELF, 10, FALSE, TRUE, FALSE)
            Add(LVar0, 30)
            Call(SetGoalPos, ACTOR_SELF, LVar0, LVar1, LVar2)
            Call(JumpToGoal, ACTOR_SELF, 8, FALSE, TRUE, FALSE)
            Add(LVar0, 20)
            Call(SetGoalPos, ACTOR_SELF, LVar0, LVar1, LVar2)
            Call(JumpToGoal, ACTOR_SELF, 6, FALSE, TRUE, FALSE)
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Idle)
            Wait(3)
            Call(YieldTurn)
            Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(2.0))
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Run)
            Call(SetGoalToHome, ACTOR_SELF)
            Call(SetActorSpeed, ACTOR_SELF, Float(8.0))
            Call(RunToGoal, ACTOR_SELF, 0, FALSE)
            Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(1.0))
        EndCaseGroup
    EndSwitch
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, TRUE)
    Return
    End
};

EvtScript EVS_Attack_Firebonk = {
    Call(UseIdleAnimation, ACTOR_SELF, FALSE)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)
    Call(SetTargetActor, ACTOR_SELF, ACTOR_PLAYER)
    Call(UseBattleCamPreset, BTL_CAM_ENEMY_APPROACH)
    Call(BattleCamTargetActor, ACTOR_SELF)
    Call(SetBattleCamTargetingModes, BTL_CAM_YADJ_TARGET, BTL_CAM_XADJ_AVG, FALSE)
    Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Run)
    Call(SetGoalToTarget, ACTOR_SELF)
    Call(AddGoalPos, ACTOR_SELF, 50, 0, 0)
    Call(SetActorSpeed, ACTOR_SELF, Float(6.0))
    Call(RunToGoal, ACTOR_SELF, 0, FALSE)
    Call(SetIdleAnimations, ACTOR_SELF, PRT_MAIN, Ref(FireAnims))
    Call(AddActorDecoration, ACTOR_SELF, PRT_MAIN, 0, ACTOR_DECORATION_RED_FLAMES)
    Call(ModifyActorDecoration, ACTOR_SELF, PRT_MAIN, 0, 100, 100, 0, 0)
    Wait(10)
    Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Fire_Idle)
    Call(SetActorDispOffset, ACTOR_SELF, 0, -1, 0)
    Wait(1)
    Call(SetActorDispOffset, ACTOR_SELF, 0, -2, 0)
    Wait(5)
    Call(SetActorDispOffset, ACTOR_SELF, 0, 0, 0)
    Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Fire_Midair)
    Call(EnemyTestTarget, ACTOR_SELF, LVar0, 0, 0, 1, BS_FLAGS1_INCLUDE_POWER_UPS)
    Switch(LVar0)
        CaseOrEq(HIT_RESULT_MISS)
        CaseOrEq(HIT_RESULT_LUCKY)
            Set(LVarA, LVar0)
            Call(SetGoalToTarget, ACTOR_SELF)
            Call(GetGoalPos, ACTOR_SELF, LVar0, LVar1, LVar2)
            Sub(LVar0, 10)
            Set(LVar1, 10)
            Add(LVar2, 3)
            Call(SetGoalPos, ACTOR_SELF, LVar0, LVar1, LVar2)
            Call(SetActorJumpGravity, ACTOR_SELF, Float(1.2))
            Thread
                Call(GetActorPos, ACTOR_SELF, LVar1, LVar2, LVar0)
                Set(LVar0, 0)
                Loop(16)
                    Call(GetActorPos, ACTOR_SELF, LVar4, LVar5, LVar6)
                    Call(N(CalculateArcsinDeg), LVar1, LVar2, LVar4, LVar5, LVar0)
                    Call(SetActorRotation, ACTOR_SELF, 0, 0, LVar0)
                    Set(LVar1, LVar4)
                    Set(LVar2, LVar5)
                    Set(LVar3, LVar6)
                    Wait(1)
                EndLoop
            EndThread
            Thread
                Wait(6)
                Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Fire_Midair)
            EndThread
            Call(JumpToGoal, ACTOR_SELF, 16, FALSE, TRUE, FALSE)
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Fire_Sleep)
            Call(SetActorScale, ACTOR_SELF, Float(1.1), Float(0.8), Float(1.0))
            Call(SetActorDispOffset, ACTOR_SELF, 0, 5, 0)
            Wait(1)
            Call(SetActorScale, ACTOR_SELF, Float(1.3), Float(0.5), Float(1.0))
            Call(SetActorDispOffset, ACTOR_SELF, 0, -2, 0)
            Wait(1)
            Call(SetActorScale, ACTOR_SELF, Float(1.0), Float(1.0), Float(1.0))
            Call(SetActorDispOffset, ACTOR_SELF, 0, 7, 0)
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Hurt)
            Call(SetIdleAnimations, ACTOR_SELF, PRT_MAIN, Ref(DefaultAnims))
            Call(RemoveActorDecoration, ACTOR_SELF, PRT_MAIN, 0)
            Wait(5)
            Call(SetGoalToTarget, ACTOR_SELF)
            IfEq(LVarA, HIT_RESULT_LUCKY)
                Call(EnemyTestTarget, ACTOR_SELF, LVar0, DAMAGE_TYPE_TRIGGER_LUCKY, 0, 0, 0)
            EndIf
            Wait(5)
            Call(SetActorDispOffset, ACTOR_SELF, 0, 0, 0)
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Midair)
            Call(SetGoalToTarget, ACTOR_SELF)
            Call(GetGoalPos, ACTOR_SELF, LVar0, LVar1, LVar2)
            Add(LVar0, 20)
            Set(LVar1, 0)
            Call(SetGoalPos, ACTOR_SELF, LVar0, LVar1, LVar2)
            Call(SetActorJumpGravity, ACTOR_SELF, Float(2.0))
            Thread
                Wait(4)
                Set(LVar0, 180)
                Loop(4)
                    Sub(LVar0, 45)
                    Call(SetActorRotation, ACTOR_SELF, 0, 0, LVar0)
                    Wait(1)
                EndLoop
                Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Midair)
            EndThread
            Call(JumpToGoal, ACTOR_SELF, 15, FALSE, TRUE, FALSE)
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Dizzy)
            Wait(5)
            Call(UseBattleCamPreset, BTL_CAM_DEFAULT)
            Call(YieldTurn)
            Call(SetActorYaw, ACTOR_SELF, 180)
            Call(AddActorDecoration, ACTOR_SELF, PRT_MAIN, 0, ACTOR_DECORATION_SWEAT)
            Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(2.0))
            Call(SetGoalToHome, ACTOR_SELF)
            Call(SetActorSpeed, ACTOR_SELF, Float(8.0))
            Call(RunToGoal, ACTOR_SELF, 0, FALSE)
            Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(1.0))
            Call(SetActorYaw, ACTOR_SELF, 0)
            Wait(5)
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Idle)
            Call(SetActorJumpGravity, ACTOR_SELF, Float(1.6))
            Call(JumpToGoal, ACTOR_SELF, 5, FALSE, TRUE, FALSE)
            Call(RemoveActorDecoration, ACTOR_SELF, PRT_MAIN, 0)
            Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
            Call(UseIdleAnimation, ACTOR_SELF, TRUE)
            Return
        EndCaseGroup
        CaseDefault
            Call(SetGoalToTarget, ACTOR_SELF)
            Call(SetActorJumpGravity, ACTOR_SELF, Float(1.2))
            Thread
                Call(GetActorPos, ACTOR_SELF, LVar1, LVar2, LVar0)
                Set(LVar0, 0)
                Loop(16)
                    Call(GetActorPos, ACTOR_SELF, LVar4, LVar5, LVar6)
                    Call(N(CalculateArcsinDeg), LVar1, LVar2, LVar4, LVar5, LVar0)
                    Call(SetActorRotation, ACTOR_SELF, 0, 0, LVar0)
                    Set(LVar1, LVar4)
                    Set(LVar2, LVar5)
                    Set(LVar3, LVar6)
                    Wait(1)
                EndLoop
            EndThread
            Thread
                Wait(6)
                Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Fire_Midair)
            EndThread
            Call(JumpToGoal, ACTOR_SELF, 16, FALSE, TRUE, FALSE)
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Fire_Midair)
            Call(SetActorScale, ACTOR_SELF, Float(1.1), Float(0.8), Float(1.0))
            Wait(1)
            Call(SetActorScale, ACTOR_SELF, Float(1.3), Float(0.5), Float(1.0))
            Wait(1)
    EndSwitch
    // Call(GetActorVar, ACTOR_SELF, AVAR_Weakened, LVar6)
    // IfEq(LVar6, TRUE)
        // Set(LVar7, dmgFirebonkWeak)
        // Set(LVar8, dmgBackFirebonkWeak)
    // Else
        Set(LVar7, dmgFirebonk)
        Set(LVar8, dmgBackFirebonk)
    // EndIf
    Call(SetGoalToTarget, ACTOR_SELF)
    Wait(2)
    Call(EnemyDamageTarget, ACTOR_SELF, LVar0, DAMAGE_TYPE_FIRE, 0, 0, LVar7, BS_FLAGS1_TRIGGER_EVENTS)
    Call(GetBattleFlags, LVar0)
    IfNotFlag(LVar0, BS_FLAGS1_ATK_BLOCKED)
        Call(UseBattleCamPreset, BTL_CAM_DEFAULT)
        Call(SetActorScale, ACTOR_SELF, Float(1.1), Float(0.8), Float(1.0))
        Wait(1)
        Call(SetActorScale, ACTOR_SELF, Float(1.0), Float(1.0), Float(1.0))
        Wait(1)
        Call(SetActorRotation, ACTOR_SELF, 0, 0, 0)
        Call(SetActorDispOffset, ACTOR_SELF, 0, 0, 0)
        Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Idle)
        Call(SetIdleAnimations, ACTOR_SELF, PRT_MAIN, Ref(DefaultAnims))
        Call(RemoveActorDecoration, ACTOR_SELF, PRT_MAIN, 0)
        Call(GetGoalPos, ACTOR_SELF, LVar0, LVar1, LVar2)
        Add(LVar0, 40)
        Set(LVar1, 0)
        Call(SetActorJumpGravity, ACTOR_SELF, Float(1.8))
        Call(SetGoalPos, ACTOR_SELF, LVar0, LVar1, LVar2)
        Call(JumpToGoal, ACTOR_SELF, 10, FALSE, TRUE, FALSE)
        Add(LVar0, 30)
        Call(SetGoalPos, ACTOR_SELF, LVar0, LVar1, LVar2)
        Call(JumpToGoal, ACTOR_SELF, 8, FALSE, TRUE, FALSE)
        Add(LVar0, 20)
        Call(SetGoalPos, ACTOR_SELF, LVar0, LVar1, LVar2)
        Call(JumpToGoal, ACTOR_SELF, 6, FALSE, TRUE, FALSE)
        Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Idle)
        Wait(3)
        Call(YieldTurn)
        Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(2.0))
        Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Run)
        Call(SetGoalToHome, ACTOR_SELF)
        Call(SetActorSpeed, ACTOR_SELF, Float(8.0))
        Call(RunToGoal, ACTOR_SELF, 0, FALSE)
        Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(1.0))
    Else
        Call(UseBattleCamPreset, BTL_CAM_DEFAULT)
        Call(SetActorScale, ACTOR_SELF, Float(1.1), Float(0.8), Float(1.0))
        Wait(1)
        Call(SetActorScale, ACTOR_SELF, Float(1.0), Float(1.0), Float(1.0))
        Wait(1)
        Call(AddActorDecoration, ACTOR_SELF, PRT_MAIN, 0, ACTOR_DECORATION_RED_FLAMES)
        Call(ModifyActorDecoration, ACTOR_SELF, PRT_MAIN, 0, 100, 100, 0, 0)
        Wait(1)
        Call(SetActorRotation, ACTOR_SELF, 0, 0, 0)
        Call(SetActorDispOffset, ACTOR_SELF, 0, 0, 0)
        Call(SetActorSounds, ACTOR_SELF, ACTOR_SOUND_JUMP, SOUND_ACTOR_HURT, 0)
        Call(SetActorRotationOffset, ACTOR_SELF, 0, 12, 0)
        Call(SetActorRotation, ACTOR_SELF, 0, 0, 80)
        Call(SetOwnerTarget, ACTOR_ENEMY0, PRT_MAIN)
        Call(SetGoalToTarget, ACTOR_SELF)
        Call(SetActorJumpGravity, ACTOR_SELF, Float(0.01))
        Call(SetActorSpeed, ACTOR_SELF, Float(15.0))
        Call(JumpToGoal, ACTOR_SELF, 0, FALSE, TRUE, FALSE)
        Wait(2)
        Call(EnemyDamageTarget, ACTOR_SELF, LVar0, DAMAGE_TYPE_FIRE, 0, 0, LVar8, BS_FLAGS1_TRIGGER_EVENTS)
        Switch(LVar0)
        CaseOrEq(HIT_RESULT_HIT)
        CaseOrEq(HIT_RESULT_NO_DAMAGE)
            Thread
                Call(SetActorRotationOffset, ACTOR_SELF, 0, 12, 0)
                Set(LVar0, 80)
                Loop(60)
                    Add(LVar0, 18)
                    Call(SetActorRotation, ACTOR_SELF, 0, 0, LVar0)
                    Wait(1)
                EndLoop
                Call(SetActorRotationOffset, ACTOR_SELF, 0, 0, 0)
                Call(SetActorRotation, ACTOR_SELF, 0, 0, 0)
            EndThread
            Call(UseBattleCamPreset, BTL_CAM_DEFAULT)
            Call(ResetAllActorSounds, ACTOR_SELF)
            Call(SetActorJumpGravity, ACTOR_SELF, Float(0.8))
            Call(GetActorPos, ACTOR_SELF, LVar0, LVar1, LVar2)
            Set(LVar1, 0)
            Sub(LVar0, 50)
            Call(SetGoalPos, ACTOR_SELF, LVar0, LVar1, LVar2)
            Call(JumpToGoal, ACTOR_SELF, 30, FALSE, TRUE, FALSE)
            Sub(LVar0, 40)
            Call(SetGoalPos, ACTOR_SELF, LVar0, LVar1, LVar2)
            Call(JumpToGoal, ACTOR_SELF, 20, FALSE, TRUE, FALSE)
            Sub(LVar0, 30)
            Call(SetGoalPos, ACTOR_SELF, LVar0, LVar1, LVar2)
            Call(JumpToGoal, ACTOR_SELF, 10, FALSE, TRUE, FALSE)
            Wait(10)
            Call(SetIdleAnimations, ACTOR_SELF, PRT_MAIN, Ref(DefaultAnims))
            Call(RemoveActorDecoration, ACTOR_SELF, PRT_MAIN, 0)
            Call(YieldTurn)
            Call(SetActorYaw, ACTOR_SELF, 180)
            Call(SetActorSpeed, ACTOR_SELF, Float(8.0))
            Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(2.0))
            Call(SetGoalToHome, ACTOR_SELF)
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Run)
            Call(SetActorYaw, ACTOR_SELF, 0)
            Wait(3)
            Call(SetGoalToHome, ACTOR_SELF)
            Call(SetActorSpeed, ACTOR_SELF, Float(8.0))
            Call(RunToGoal, ACTOR_SELF, 0, FALSE)
            Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(1.0))
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Idle)
        EndCaseGroup
        EndSwitch
    EndIf
    Call(SetActorVar, ACTOR_SELF, AVAR_HitBlue, FALSE)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, TRUE)
    Return
    End
};

EvtScript EVS_Attack_FireBurst = {
    Call(UseIdleAnimation, ACTOR_SELF, FALSE)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)
    Call(UseBattleCamPreset, BTL_CAM_DEFAULT)
    Call(SetBattleCamDist, 500)
    Call(BattleCamTargetActor, ACTOR_SELF)
    Call(SetBattleCamTargetingModes, BTL_CAM_YADJ_TARGET, BTL_CAM_XADJ_AVG, FALSE)
    Call(MoveBattleCamOver, 20)
    Wait(20)
    Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Red_Fire_Run)
    // Call(ModifyActorDecoration, ACTOR_SELF, PRT_MAIN, 0, 100, 100, 0, 0)
    Call(GetActorPos, ACTOR_SELF, LVar0, LVar1, LVar2)
    Add(LVar2, 30)
    PlayEffect(EFFECT_RING_BLAST, 0, LVar0, LVar1, LVar2, Float(12.0), 15)
    Call(SetTargetActor, ACTOR_SELF, ACTOR_PLAYER)
    Call(SetGoalToTarget, ACTOR_SELF)
    Wait(2)
    Call(EnemyDamageTarget, ACTOR_SELF, LVar0, DAMAGE_TYPE_FIRE, 0, 0, dmgFireBurst, BS_FLAGS1_TRIGGER_EVENTS)
    Call(SetOwnerTarget, ACTOR_ENEMY0, PRT_MAIN)
    Call(SetGoalToTarget, ACTOR_SELF)
    Call(EnemyDamageTarget, ACTOR_SELF, LVar0, DAMAGE_TYPE_FIRE, 0, 0, dmgFireBurst, BS_FLAGS1_TRIGGER_EVENTS)
    // Call(SetGoalToTarget, ACTOR_SELF)
    // Call(GetOwnerTarget, LVar0, LVar1)
    // Call(DispatchDamageEvent, LVar0, 3, EVENT_HIT)
    // Call(EnemyDamageTarget, ACTOR_SELF, LVar0, DAMAGE_TYPE_IGNORE_DEFENSE | DAMAGE_TYPE_MULTIPLE_POPUPS | DAMAGE_TYPE_FIRE, SUPPRESS_EVENT_ALL, 0, 3, BS_FLAGS1_TRIGGER_EVENTS)
    Call(SetActorVar, ACTOR_ENEMY0, 1, TRUE)
    SetConst(LVar0, PRT_MAIN)
    SetConst(LVar1, ANIM_Goomates_Red_BurnStill)
    Set(LVar2, EXEC_DEATH_NO_SPINNING)
    ExecWait(EVS_Enemy_Death)
    Return
    End
};

}; // namespace red_mate

namespace blue_mate {

extern s32 DefaultAnims[];
extern EvtScript EVS_Init;
extern EvtScript EVS_Init_Inner;
extern EvtScript EVS_Idle;
extern EvtScript EVS_TakeTurn;
extern EvtScript EVS_HandleEvent;
extern EvtScript EVS_HandlePhase;
extern EvtScript EVS_Move_Heal;
extern EvtScript EVS_Move_ValorUp;

enum ActorPartIDs {
    PRT_MAIN        = 1,
};

// Blue Mate Actor Stats
constexpr s32 hp = 2;
constexpr s32 amtHeal = 3;
constexpr s32 amtAttackBoost = -1;
constexpr s32 amtDefenseBoost = 1;

enum ActorVars {
    AVAR_TurnCount,
    AVAR_ValorUp,
};

// enum ActorParams {
//     DMG_HEADBONK    = 1,
// };

s32 KuriboDefense[] = {
    ELEMENT_NORMAL,   -2,
    ELEMENT_END,
};

s32 NormalDefense[] = {
    ELEMENT_NORMAL,   -1,
    ELEMENT_END,
};

s32 DefenseTable[] = {
    ELEMENT_NORMAL,   1,
    ELEMENT_END,
};

s32 BoostedDefense[] = {
    ELEMENT_NORMAL,   2,
    ELEMENT_END,
};

s32 StatusTable[] = {
    STATUS_KEY_NORMAL,              0,
    STATUS_KEY_DEFAULT,             0,
    STATUS_KEY_SLEEP,             100,
    STATUS_KEY_POISON,              0,
    STATUS_KEY_FROZEN,              0,
    STATUS_KEY_DIZZY,             100,
    STATUS_KEY_FEAR,                0,
    STATUS_KEY_STATIC,              0,
    STATUS_KEY_PARALYZE,          100,
    STATUS_KEY_SHRINK,            100,
    STATUS_KEY_STOP,              100,
    STATUS_TURN_MOD_DEFAULT,        0,
    STATUS_TURN_MOD_SLEEP,          0,
    STATUS_TURN_MOD_POISON,         0,
    STATUS_TURN_MOD_FROZEN,         0,
    STATUS_TURN_MOD_DIZZY,          0,
    STATUS_TURN_MOD_FEAR,           0,
    STATUS_TURN_MOD_STATIC,         0,
    STATUS_TURN_MOD_PARALYZE,       0,
    STATUS_TURN_MOD_SHRINK,         0,
    STATUS_TURN_MOD_STOP,           0,
    STATUS_END,
};

ActorPartBlueprint ActorParts[] = {
    {
        .flags = ACTOR_PART_FLAG_PRIMARY_TARGET,
        .index = PRT_MAIN,
        .posOffset = { 0, 0, 0 },
        .targetOffset = { 0, 24 },
        .opacity = 255,
        .idleAnimations = DefaultAnims,
        .defenseTable = DefenseTable,
        .eventFlags = 0,
        .elementImmunityFlags = 0,
        .projectileTargetOffset = { 0, -3 },
    },
};

s32 DefaultAnims[] = {
    STATUS_KEY_NORMAL,    ANIM_Goomates_Blue_Idle,
    STATUS_KEY_STONE,     ANIM_Goomates_Blue_Still,
    STATUS_KEY_SLEEP,     ANIM_Goomates_Blue_Sleep,
    STATUS_KEY_POISON,    ANIM_Goomates_Blue_Idle,
    STATUS_KEY_STOP,      ANIM_Goomates_Blue_Still,
    STATUS_KEY_STATIC,    ANIM_Goomates_Blue_Idle,
    STATUS_KEY_PARALYZE,  ANIM_Goomates_Blue_Still,
    STATUS_KEY_DIZZY,     ANIM_Goomates_Blue_Dizzy,
    STATUS_KEY_FEAR,      ANIM_Goomates_Blue_Dizzy,
    STATUS_END,
};

// while shuffling around during idle
s32 ShuffleAnims[] = {
    STATUS_KEY_NORMAL,    ANIM_Goomates_Blue_Run,
    STATUS_KEY_STONE,     ANIM_Goomates_Blue_Still,
    STATUS_KEY_SLEEP,     ANIM_Goomates_Blue_Sleep,
    STATUS_KEY_POISON,    ANIM_Goomates_Blue_Idle,
    STATUS_KEY_STOP,      ANIM_Goomates_Blue_Still,
    STATUS_KEY_STATIC,    ANIM_Goomates_Blue_Run,
    STATUS_KEY_PARALYZE,  ANIM_Goomates_Blue_Still,
    STATUS_KEY_DIZZY,     ANIM_Goomates_Blue_Dizzy,
    STATUS_KEY_FEAR,      ANIM_Goomates_Blue_Dizzy,
    STATUS_END,
};

EvtScript EVS_Init = {
    ExecWait(EVS_Init_Inner)
    Return
    End
};

EvtScript EVS_Init_Inner = {
    Call(BindTakeTurn, ACTOR_SELF, Ref(EVS_TakeTurn))
    Call(BindIdle, ACTOR_SELF, Ref(EVS_Idle))
    Call(BindHandleEvent, ACTOR_SELF, Ref(EVS_HandleEvent))
    Call(BindHandlePhase, ACTOR_SELF, Ref(EVS_HandlePhase))
    Call(SetActorVar, ACTOR_SELF, AVAR_TurnCount, 1)
    Call(SetActorVar, ACTOR_SELF, AVAR_ValorUp, FALSE)
    Return
    End
};

EvtScript EVS_Idle = {
    Return
    End
};

EvtScript EVS_HandleEvent = {
    Call(UseIdleAnimation, ACTOR_SELF, FALSE)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)
    Call(SetActorScale, ACTOR_SELF, Float(1.0), Float(1.0), Float(1.0))
    Call(GetLastEvent, ACTOR_SELF, LVar0)
    Switch(LVar0)
        CaseOrEq(EVENT_HIT_COMBO)
        CaseOrEq(EVENT_HIT)
            Call(SetActorVar, ACTOR_ENEMY1, red_mate::AVAR_HitBlue, TRUE)
            Call(GetActorPos, ACTOR_ENEMY1, LVar0, LVar1, LVar2)
            Add(LVar1, 20)
            Call(PlaySoundAtActor, ACTOR_ENEMY1, SOUND_EMOTE_IDEA)
            PlayEffect(EFFECT_EMOTE, EMOTE_EXCLAMATION, 0, LVar0, LVar1, LVar2, 24, 0, 25, 0, 0)
            Call(SetAnimation, ACTOR_ENEMY1, PRT_MAIN, ANIM_Goomates_Red_Run)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Blue_Hurt)
            ExecWait(EVS_Enemy_Hit)
            Wait(25)
        EndCaseGroup
        CaseEq(EVENT_BURN_HIT)
            Call(SetActorVar, ACTOR_ENEMY1, red_mate::AVAR_HitBlue, TRUE)
            Call(GetActorPos, ACTOR_ENEMY1, LVar0, LVar1, LVar2)
            Add(LVar1, 20)
            Call(PlaySoundAtActor, ACTOR_ENEMY1, SOUND_EMOTE_IDEA)
            PlayEffect(EFFECT_EMOTE, EMOTE_EXCLAMATION, 0, LVar0, LVar1, LVar2, 24, 0, 25, 0, 0)
            Call(SetAnimation, ACTOR_ENEMY1, PRT_MAIN, ANIM_Goomates_Red_Run)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Blue_BurnHurt)
            SetConst(LVar2, ANIM_Goomates_Blue_BurnStill)
            ExecWait(EVS_Enemy_BurnHit)
            Wait(25)
        CaseEq(EVENT_BURN_DEATH)
            Call(SetActorVar, ACTOR_ENEMY1, red_mate::AVAR_HitBlue, FALSE)
            Call(SetActorVar, ACTOR_ENEMY1, red_mate::AVAR_BlueDead, TRUE)
            Call(GetActorPos, ACTOR_ENEMY1, LVar0, LVar1, LVar2)
            Add(LVar1, 20)
            Call(PlaySoundAtActor, ACTOR_ENEMY1, SOUND_EMOTE_IDEA)
            PlayEffect(EFFECT_EMOTE, EMOTE_EXCLAMATION, 0, LVar0, LVar1, LVar2, 24, 0, 25, 0, 0)
            Call(SetAnimation, ACTOR_ENEMY1, PRT_MAIN, ANIM_Goomates_Red_Run)
            // Call(SetIdleAnimations, ACTOR_ENEMY1, PRT_MAIN, Ref(red_mate::FireAnims))
            // Call(AddActorDecoration, ACTOR_ENEMY1, PRT_MAIN, 0, ACTOR_DECORATION_RED_FLAMES)
            // Call(ModifyActorDecoration, ACTOR_SELF, PRT_MAIN, 0, 100, 100, 0, 0)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Blue_BurnHurt)
            SetConst(LVar2, ANIM_Goomates_Blue_BurnStill)
            ExecWait(EVS_Enemy_BurnHit)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Blue_BurnStill)
            ExecWait(EVS_Enemy_Death)
            Wait(25)
            Return
        CaseEq(EVENT_SPIN_SMASH_HIT)
            Call(SetActorVar, ACTOR_ENEMY1, red_mate::AVAR_HitBlue, TRUE)
            Call(GetActorPos, ACTOR_ENEMY1, LVar0, LVar1, LVar2)
            Add(LVar1, 20)
            Call(PlaySoundAtActor, ACTOR_ENEMY1, SOUND_EMOTE_IDEA)
            PlayEffect(EFFECT_EMOTE, EMOTE_EXCLAMATION, 0, LVar0, LVar1, LVar2, 24, 0, 25, 0, 0)
            Call(SetAnimation, ACTOR_ENEMY1, PRT_MAIN, ANIM_Goomates_Red_Run)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Blue_Hurt)
            ExecWait(EVS_Enemy_SpinSmashHit)
            Wait(25)
        CaseEq(EVENT_SPIN_SMASH_DEATH)
            Call(SetActorVar, ACTOR_ENEMY1, red_mate::AVAR_HitBlue, FALSE)
            Call(SetActorVar, ACTOR_ENEMY1, red_mate::AVAR_BlueDead, TRUE)
            Call(GetActorPos, ACTOR_ENEMY1, LVar0, LVar1, LVar2)
            Add(LVar1, 20)
            Call(PlaySoundAtActor, ACTOR_ENEMY1, SOUND_EMOTE_IDEA)
            PlayEffect(EFFECT_EMOTE, EMOTE_EXCLAMATION, 0, LVar0, LVar1, LVar2, 24, 0, 25, 0, 0)
            Call(SetAnimation, ACTOR_ENEMY1, PRT_MAIN, ANIM_Goomates_Red_Run)
            // Call(SetIdleAnimations, ACTOR_ENEMY1, PRT_MAIN, Ref(red_mate::FireAnims))
            // Call(AddActorDecoration, ACTOR_ENEMY1, PRT_MAIN, 0, ACTOR_DECORATION_RED_FLAMES)
            // Call(ModifyActorDecoration, ACTOR_SELF, PRT_MAIN, 0, 100, 100, 0, 0)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Blue_Hurt)
            ExecWait(EVS_Enemy_SpinSmashHit)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Blue_Hurt)
            ExecWait(EVS_Enemy_Death)
            Wait(25)
            Return
        CaseEq(EVENT_SHOCK_HIT)
            Call(SetActorVar, ACTOR_ENEMY1, red_mate::AVAR_HitBlue, TRUE)
            Call(GetActorPos, ACTOR_ENEMY1, LVar0, LVar1, LVar2)
            Add(LVar1, 20)
            Call(PlaySoundAtActor, ACTOR_ENEMY1, SOUND_EMOTE_IDEA)
            PlayEffect(EFFECT_EMOTE, EMOTE_EXCLAMATION, 0, LVar0, LVar1, LVar2, 24, 0, 25, 0, 0)
            Call(SetAnimation, ACTOR_ENEMY1, PRT_MAIN, ANIM_Goomates_Red_Run)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Blue_Hurt)
            ExecWait(EVS_Enemy_ShockHit)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Blue_Hurt)
            ExecWait(EVS_Enemy_Knockback)
            Call(JumpToGoal, ACTOR_SELF, 5, FALSE, TRUE, FALSE)
            Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(2.0))
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Blue_Dizzy)
            Call(SetGoalToHome, ACTOR_SELF)
            Call(SetActorSpeed, ACTOR_SELF, Float(8.0))
            Call(RunToGoal, ACTOR_SELF, 0, FALSE)
            Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(1.0))
            Wait(5)
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Blue_Idle)
            Call(SetActorJumpGravity, ACTOR_SELF, Float(1.6))
            Call(JumpToGoal, ACTOR_SELF, 5, FALSE, TRUE, FALSE)
            Wait(20)
        CaseEq(EVENT_SHOCK_DEATH)
            Call(SetActorVar, ACTOR_ENEMY1, red_mate::AVAR_HitBlue, FALSE)
            Call(SetActorVar, ACTOR_ENEMY1, red_mate::AVAR_BlueDead, TRUE)
            Call(GetActorPos, ACTOR_ENEMY1, LVar0, LVar1, LVar2)
            Add(LVar1, 20)
            Call(PlaySoundAtActor, ACTOR_ENEMY1, SOUND_EMOTE_IDEA)
            PlayEffect(EFFECT_EMOTE, EMOTE_EXCLAMATION, 0, LVar0, LVar1, LVar2, 24, 0, 25, 0, 0)
            Call(SetAnimation, ACTOR_ENEMY1, PRT_MAIN, ANIM_Goomates_Red_Run)
            // Call(SetIdleAnimations, ACTOR_ENEMY1, PRT_MAIN, Ref(red_mate::FireAnims))
            // Call(AddActorDecoration, ACTOR_ENEMY1, PRT_MAIN, 0, ACTOR_DECORATION_RED_FLAMES)
            // Call(ModifyActorDecoration, ACTOR_SELF, PRT_MAIN, 0, 100, 100, 0, 0)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Blue_Hurt)
            ExecWait(EVS_Enemy_ShockHit)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Blue_Hurt)
            ExecWait(EVS_Enemy_Death)
            Wait(25)
            Return
        CaseEq(EVENT_STAR_BEAM)
            // do nothing
        CaseOrEq(EVENT_ZERO_DAMAGE)
        CaseOrEq(EVENT_IMMUNE)
        CaseOrEq(EVENT_AIR_LIFT_FAILED)
            Call(SetActorVar, ACTOR_ENEMY1, red_mate::AVAR_HitBlue, TRUE)
            Call(GetActorPos, ACTOR_ENEMY1, LVar0, LVar1, LVar2)
            Add(LVar1, 20)
            Call(PlaySoundAtActor, ACTOR_ENEMY1, SOUND_EMOTE_IDEA)
            PlayEffect(EFFECT_EMOTE, EMOTE_EXCLAMATION, 0, LVar0, LVar1, LVar2, 24, 0, 25, 0, 0)
            Call(SetAnimation, ACTOR_ENEMY1, PRT_MAIN, ANIM_Goomates_Red_Run)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Blue_Idle)
            ExecWait(EVS_Enemy_NoDamageHit)
            Wait(25)
        EndCaseGroup
        CaseEq(EVENT_DEATH)
            Call(SetActorVar, ACTOR_ENEMY1, red_mate::AVAR_HitBlue, FALSE)
            Call(SetActorVar, ACTOR_ENEMY1, red_mate::AVAR_BlueDead, TRUE)
            Call(GetActorPos, ACTOR_ENEMY1, LVar0, LVar1, LVar2)
            Add(LVar1, 20)
            Call(PlaySoundAtActor, ACTOR_ENEMY1, SOUND_EMOTE_IDEA)
            PlayEffect(EFFECT_EMOTE, EMOTE_EXCLAMATION, 0, LVar0, LVar1, LVar2, 24, 0, 25, 0, 0)
            Call(SetAnimation, ACTOR_ENEMY1, PRT_MAIN, ANIM_Goomates_Red_Run)
            // Call(SetIdleAnimations, ACTOR_ENEMY1, PRT_MAIN, Ref(red_mate::FireAnims))
            // Call(AddActorDecoration, ACTOR_ENEMY1, PRT_MAIN, 0, ACTOR_DECORATION_RED_FLAMES)
            // Call(ModifyActorDecoration, ACTOR_SELF, PRT_MAIN, 0, 100, 100, 0, 0)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Blue_Hurt)
            ExecWait(EVS_Enemy_Hit)
            Wait(10)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Blue_Hurt)
            ExecWait(EVS_Enemy_Death)
            Wait(25)
            Return
        CaseEq(EVENT_END_FIRST_STRIKE)
            Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(2.0))
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Blue_Run)
            Call(SetGoalToHome, ACTOR_SELF)
            Call(SetActorSpeed, ACTOR_SELF, Float(4.0))
            Call(RunToGoal, ACTOR_SELF, 0, FALSE)
            Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(1.0))
            Call(HPBarToHome, ACTOR_SELF)
        CaseEq(EVENT_RECOVER_STATUS)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Blue_Idle)
            ExecWait(EVS_Enemy_Recover)
        CaseEq(EVENT_SCARE_AWAY)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Blue_Run)
            SetConst(LVar2, ANIM_Goomates_Blue_Hurt)
            ExecWait(EVS_Enemy_ScareAway)
            Return
        CaseEq(EVENT_BEGIN_AIR_LIFT)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Blue_Run)
            ExecWait(EVS_Enemy_AirLift)
        CaseEq(EVENT_BLOW_AWAY)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_Goomates_Blue_Hurt)
            ExecWait(EVS_Enemy_BlowAway)
            Return
        CaseDefault
    EndSwitch
    Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Blue_Idle)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, TRUE)
    Return
    End
};

EvtScript EVS_TakeTurn = {
    Call(UseIdleAnimation, ACTOR_SELF, FALSE)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)
    Call(GetActorVar, ACTOR_SELF, AVAR_TurnCount, LVar3)
    // IfEq(LVar3, 3)
    //     Call(SetDefenseTable, ACTOR_SELF, PRT_MAIN, Ref(DefenseTable))
    //     Call(SetDefenseTable, ACTOR_ENEMY1, PRT_MAIN, Ref(DefenseTable))
    //     Call(SetDefenseTable, ACTOR_ENEMY0, PRT_MAIN, Ref(KuriboDefense))
    //     Call(SetActorVar, ACTOR_ENEMY1, red_mate::AVAR_Weakened, FALSE)
    // EndIf
    ExecWait(EVS_Move_Heal)
    Call(AddActorVar, ACTOR_SELF, AVAR_TurnCount, 1)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, TRUE)
    Return
    End
};

EvtScript EVS_Move_Heal = {
    Call(UseIdleAnimation, ACTOR_SELF, FALSE)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)
    Call(SetTargetActor, ACTOR_SELF, ACTOR_PLAYER)
    Call(UseBattleCamPreset, BTL_CAM_ACTOR)
    Call(BattleCamTargetActor, ACTOR_SELF)
    Call(MoveBattleCamOver, 15)
    Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(2.0))
    Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Blue_Run)
    Wait(30)
    Thread
        Wait(5)
        Call(PlaySoundAtActor, ACTOR_ENEMY0, SOUND_RECOVER_HEART)
        Call(PlaySoundAtActor, ACTOR_ENEMY0, SOUND_HEART_BOUNCE)
        Wait(30)
        Call(PlaySoundAtActor, ACTOR_ENEMY0, SOUND_STAR_BOUNCE_A)
    EndThread
    Thread
        Call(FreezeBattleState, TRUE)
        Call(HealActor, ACTOR_ENEMY0, amtHeal, FALSE)
        Call(FreezeBattleState, FALSE)
    EndThread
    Call(WaitForBuffDone)
    Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(1.0))
    Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Blue_Idle)
    Call(UseBattleCamPreset, BTL_CAM_DEFAULT)
    Call(MoveBattleCamOver, 10)
    Wait(10)
    Call(YieldTurn)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, TRUE)
    Return
    End
};

EvtScript EVS_Move_ValorUp = {
    Call(UseIdleAnimation, ACTOR_SELF, FALSE)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)
    Call(SetTargetActor, ACTOR_SELF, ACTOR_PLAYER)
    Call(UseBattleCamPreset, BTL_CAM_ACTOR)
    Call(BattleCamTargetActor, ACTOR_SELF)
    Call(MoveBattleCamOver, 15)
    Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(2.0))
    Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Blue_Run)
    Wait(30)
    Thread
        Wait(10)
        Call(PlaySoundAtActor, LVar8, SOUND_MAGIKOOPA_POWER_UP)
    EndThread
    Call(SetDefenseTable, ACTOR_SELF, PRT_MAIN, Ref(BoostedDefense))
    Call(SetDefenseTable, ACTOR_ENEMY1, PRT_MAIN, Ref(BoostedDefense))
    Call(SetDefenseTable, ACTOR_ENEMY0, PRT_MAIN, Ref(NormalDefense))
    Call(SetActorVar, ACTOR_ENEMY1, red_mate::AVAR_Weakened, TRUE)
    Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(1.0))
    Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_Goomates_Blue_Idle)
    Wait(1)
    Call(UseBattleCamPreset, BTL_CAM_DEFAULT)
    Call(MoveBattleCamOver, 10)
    Wait(10)
    // Call(YieldTurn)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, TRUE)
    Return
    End
};

EvtScript EVS_HandlePhase = {
    Call(UseIdleAnimation, ACTOR_SELF, FALSE)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)
    // Call(GetBattlePhase, LVar0)
    // Switch(LVar0)
    //     CaseEq(PHASE_PLAYER_BEGIN)
    //         Call(GetActorVar, ACTOR_SELF, AVAR_ValorUp, LVar3)
    //         IfEq(LVar3, FALSE)
    //             Call(SetActorVar, ACTOR_SELF, AVAR_ValorUp, TRUE)
    //             // Call(UseBattleCamPreset, BTL_CAM_ACTOR)
    //             // Call(BattleCamTargetActor, ACTOR_SELF)
    //             // Call(MoveBattleCamOver, 30)
    //             // Wait(30)
    //             // ExecWait(EVS_Move_ValorUp)
    //             // Call(UseIdleAnimation, ACTOR_SELF, FALSE)
    //             // Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)
    //             // Call(UseBattleCamPreset, BTL_CAM_ACTOR)
    //             // Call(BattleCamTargetActor, ACTOR_SELF)
    //             // Call(MoveBattleCamOver, 30)
    //             // Wait(30)
    //             // Call(UseIdleAnimation, ACTOR_ENEMY1, FALSE)
    //             Call(EnableIdleScript, ACTOR_ENEMY1, IDLE_SCRIPT_DISABLE)
    //             Call(UseBattleCamPreset, BTL_CAM_ACTOR)
    //             Call(BattleCamTargetActor, ACTOR_ENEMY1)
    //             Call(MoveBattleCamOver, 30)
    //             Wait(30)
    //         EndIf
    //     CaseEq(PHASE_ENEMY_BEGIN)
    // EndSwitch
    // Call(EnableIdleScript, ACTOR_ENEMY1, IDLE_SCRIPT_ENABLE)
    // Call(UseIdleAnimation, ACTOR_ENEMY1, TRUE)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, TRUE)
    Return
    End
};

}; // namespace blue_mate

ActorBlueprint RedMate = {
    .flags = 0,
    .maxHP = red_mate::hp,
    .type = ACTOR_TYPE_RED_MATE,
    .level = ACTOR_LEVEL_RED_MATE,
    .partCount = ARRAY_COUNT(red_mate::ActorParts),
    .partsData = red_mate::ActorParts,
    .initScript = &red_mate::EVS_Init,
    .statusTable = red_mate::StatusTable,
    .escapeChance = 0,
    .airLiftChance = 0,
    .hurricaneChance = 0,
    .spookChance = 0,
    .upAndAwayChance = 0,
    .spinSmashReq = 0,
    .powerBounceChance = 80,
    .coinReward = 0,
    .size = { 32, 32 },
    .healthBarOffset = { 0, 0 },
    .statusIconOffset = { -10, 20 },
    .statusTextOffset = { 8, 25 },
};

ActorBlueprint BlueMate = {
    .flags = 0,
    .maxHP = blue_mate::hp,
    .type = ACTOR_TYPE_BLUE_MATE,
    .level = ACTOR_LEVEL_BLUE_MATE,
    .partCount = ARRAY_COUNT(blue_mate::ActorParts),
    .partsData = blue_mate::ActorParts,
    .initScript = &blue_mate::EVS_Init,
    .statusTable = blue_mate::StatusTable,
    .escapeChance = 0,
    .airLiftChance = 0,
    .hurricaneChance = 0,
    .spookChance = 0,
    .upAndAwayChance = 0,
    .spinSmashReq = 0,
    .powerBounceChance = 80,
    .coinReward = 0,
    .size = { 32, 32 },
    .healthBarOffset = { 0, 0 },
    .statusIconOffset = { -10, 20 },
    .statusTextOffset = { 8, 25 },
};

}; // namespace battle::actor
