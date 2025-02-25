#include "battle/battle.h"
#include "script_api/battle.h"
#include "effects.h"
#include "sprite/npc/PlayerGoomba.h"

namespace battle::actor {

namespace player_goomba {


extern EvtScript EVS_Init;
extern s32 DefaultAnims[];
extern EvtScript EVS_Idle;
extern EvtScript EVS_TakeTurn;
extern EvtScript EVS_HandleEvent;
extern EvtScript EVS_Attack_Headbonk;
extern EvtScript EVS_Attack_Multibonk;
extern EvtScript EVS_Move_RallyWink;
extern EvtScript EVS_Move_Charge;

enum ActorParams {
    DMG_HEADBONK         = 1,
    DMG_MULTIBONK        = 1,
};

enum ActorPartIDs {
    PRT_MAIN            = 1,
};

s32 DefenseTable[] = {
    ELEMENT_NORMAL,   0,
    ELEMENT_END,
};

s32 StatusTable[] = {
    STATUS_KEY_NORMAL,              0,
    STATUS_KEY_DEFAULT,             0,
    STATUS_KEY_SLEEP,             100,
    STATUS_KEY_POISON,            100,
    STATUS_KEY_FROZEN,            100,
    STATUS_KEY_DIZZY,             100,
    STATUS_KEY_FEAR,              100,
    STATUS_KEY_STATIC,            100,
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
        .targetOffset = { 0, 20 },
        .opacity = 255,
        .idleAnimations = DefaultAnims,
        .defenseTable = DefenseTable,
        .eventFlags = ACTOR_EVENT_FLAGS_NONE,
        .elementImmunityFlags = 0,
        .projectileTargetOffset = { 0, -10 },
    },
};

s32 DefaultAnims[] = {
    STATUS_KEY_NORMAL,    ANIM_PlayerGoomba_Idle,
    STATUS_KEY_STONE,     ANIM_PlayerGoomba_Still,
    STATUS_KEY_SLEEP,     ANIM_PlayerGoomba_Sleep,
    STATUS_KEY_POISON,    ANIM_PlayerGoomba_Idle,
    STATUS_KEY_STOP,      ANIM_PlayerGoomba_Still,
    STATUS_KEY_STATIC,    ANIM_PlayerGoomba_Idle,
    STATUS_KEY_PARALYZE,  ANIM_PlayerGoomba_Still,
    STATUS_KEY_DIZZY,     ANIM_PlayerGoomba_Dizzy,
    STATUS_KEY_FEAR,      ANIM_PlayerGoomba_Dizzy,
    STATUS_KEY_INACTIVE,  ANIM_PlayerGoomba_Idle,
    STATUS_KEY_INACTIVE_WEARY, ANIM_PlayerGoomba_Idle,
    STATUS_KEY_DANGER,    ANIM_PlayerGoomba_Idle,
    STATUS_KEY_THINKING,  ANIM_PlayerGoomba_Idle,
    STATUS_KEY_WEARY,     ANIM_PlayerGoomba_Idle,
    STATUS_END,
};

// while shuffling around during idle
s32 ShuffleAnims[] = {
    STATUS_KEY_NORMAL,    ANIM_PlayerGoomba_Run,
    STATUS_KEY_STONE,     ANIM_PlayerGoomba_Still,
    STATUS_KEY_SLEEP,     ANIM_PlayerGoomba_Sleep,
    STATUS_KEY_POISON,    ANIM_PlayerGoomba_Idle,
    STATUS_KEY_STOP,      ANIM_PlayerGoomba_Still,
    STATUS_KEY_STATIC,    ANIM_PlayerGoomba_Run,
    STATUS_KEY_PARALYZE,  ANIM_PlayerGoomba_Still,
    STATUS_KEY_DIZZY,     ANIM_PlayerGoomba_Dizzy,
    STATUS_KEY_FEAR,      ANIM_PlayerGoomba_Dizzy,
    STATUS_END,
};

EvtScript EVS_Init = {
    Call(BindTakeTurn, ACTOR_SELF, Ref(EVS_TakeTurn))
    Call(BindIdle, ACTOR_SELF, Ref(EVS_Idle))
    Call(BindHandleEvent, ACTOR_SELF, Ref(EVS_HandleEvent))
    Call(GetActorPos, ACTOR_SELF, LVar0, LVar1, LVar2)
    IfGt(LVar0, 0)
        Call(SetActorYaw, ACTOR_SELF, 0)
    Else
        Call(SetActorYaw, ACTOR_SELF, 180)
    EndIf
    Return
    End
};

EvtScript EVS_Idle = {
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
            SetConst(LVar1, ANIM_PlayerGoomba_Hurt)
            ExecWait(EVS_Enemy_Hit)
        EndCaseGroup
        CaseEq(EVENT_BURN_HIT)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_PlayerGoomba_BurnHurt)
            SetConst(LVar2, ANIM_PlayerGoomba_BurnStill)
            ExecWait(EVS_Enemy_BurnHit)
        CaseEq(EVENT_BURN_DEATH)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_PlayerGoomba_BurnHurt)
            SetConst(LVar2, ANIM_PlayerGoomba_BurnStill)
            ExecWait(EVS_Enemy_BurnHit)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_PlayerGoomba_BurnStill)
            ExecWait(EVS_Enemy_Death)
            Return
        CaseEq(EVENT_SPIN_SMASH_HIT)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_PlayerGoomba_Hurt)
            ExecWait(EVS_Enemy_SpinSmashHit)
        CaseEq(EVENT_SPIN_SMASH_DEATH)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_PlayerGoomba_Hurt)
            ExecWait(EVS_Enemy_SpinSmashHit)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_PlayerGoomba_HurtStill)
            ExecWait(EVS_Enemy_Death)
            Return
        CaseEq(EVENT_SHOCK_HIT)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_PlayerGoomba_HurtStill) // ANIM_PlayerGoomba_HurtStill
            ExecWait(EVS_Enemy_ShockHit)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_PlayerGoomba_Hurt)
            ExecWait(EVS_Enemy_Knockback)
            Call(JumpToGoal, ACTOR_SELF, 5, FALSE, TRUE, FALSE)
            Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(2.0))
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_PlayerGoomba_Dizzy)
            Call(SetGoalToHome, ACTOR_SELF)
            Call(SetActorSpeed, ACTOR_SELF, Float(8.0))
            Call(RunToGoal, ACTOR_SELF, 0, FALSE)
            Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(1.0))
            Wait(5)
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_PlayerGoomba_Idle)
            Call(SetActorJumpGravity, ACTOR_SELF, Float(1.6))
            Call(JumpToGoal, ACTOR_SELF, 5, FALSE, TRUE, FALSE)
        CaseEq(EVENT_SHOCK_DEATH)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_PlayerGoomba_HurtStill)
            ExecWait(EVS_Enemy_ShockHit)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_PlayerGoomba_HurtStill)
            ExecWait(EVS_Enemy_Death)
            Return
        CaseEq(EVENT_STAR_BEAM)
            // do nothing
        CaseOrEq(EVENT_ZERO_DAMAGE)
        CaseOrEq(EVENT_IMMUNE)
        CaseOrEq(EVENT_AIR_LIFT_FAILED)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_PlayerGoomba_Idle)
            ExecWait(EVS_Enemy_NoDamageHit)
        EndCaseGroup
        CaseEq(EVENT_DEATH)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_PlayerGoomba_Hurt)
            ExecWait(EVS_Enemy_Hit)
            Wait(10)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_PlayerGoomba_HurtStill)
            ExecWait(EVS_Enemy_Death)
            Return
        CaseEq(EVENT_END_FIRST_STRIKE)
            Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(2.0))
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_PlayerGoomba_Run)
            Call(SetGoalToHome, ACTOR_SELF)
            Call(SetActorSpeed, ACTOR_SELF, Float(4.0))
            Call(RunToGoal, ACTOR_SELF, 0, FALSE)
            Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(1.0))
            Call(HPBarToHome, ACTOR_SELF)
        CaseEq(EVENT_RECOVER_STATUS)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_PlayerGoomba_Idle)
            ExecWait(EVS_Enemy_Recover)
        CaseEq(EVENT_SCARE_AWAY)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_PlayerGoomba_Run)
            SetConst(LVar2, ANIM_PlayerGoomba_Hurt)
            ExecWait(EVS_Enemy_ScareAway)
            Return
        CaseEq(EVENT_BEGIN_AIR_LIFT)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_PlayerGoomba_Run)
            ExecWait(EVS_Enemy_AirLift)
        CaseEq(EVENT_BLOW_AWAY)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_PlayerGoomba_Hurt)
            ExecWait(EVS_Enemy_BlowAway)
            Return
        CaseDefault
    EndSwitch
    Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_PlayerGoomba_Idle)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, TRUE)
    Return
    End
};

EvtScript EVS_TakeTurn = {
    Call(UseIdleAnimation, ACTOR_SELF, FALSE)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)
    // Decide which attack to use from menu selection
    Call(GetMenuSelection, LVar0, LVar1, LVar2)
    Switch(LVar2)
        CaseEq(MOVE_HEADBONK1)
            ExecWait(EVS_Attack_Headbonk)
        CaseEq(MOVE_MULTIBONK)
            ExecWait(EVS_Attack_Multibonk)
        CaseEq(MOVE_RALLY_WINK)
            ExecWait(EVS_Move_RallyWink)
        CaseEq(MOVE_CHARGE)
            ExecWait(EVS_Move_Charge)
    EndSwitch
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
    Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_PlayerGoomba_Run)
    Call(SetGoalToTarget, ACTOR_SELF)
    Call(AddGoalPos, ACTOR_SELF, 50, 0, 0)
    Call(SetActorSpeed, ACTOR_SELF, Float(6.0))
    Call(RunToGoal, ACTOR_SELF, 0, FALSE)
    Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_PlayerGoomba_Idle)
    Loop(2)
        Call(SetActorDispOffset, ACTOR_SELF, 0, -1, 0)
        Wait(1)
        Call(SetActorDispOffset, ACTOR_SELF, 0, -2, 0)
        Wait(5)
        Call(SetActorDispOffset, ACTOR_SELF, 0, 0, 0)
        Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_PlayerGoomba_Midair)
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
                        Call(CalculateArcsinDeg, LVar1, LVar2, LVar4, LVar5, LVar0)
                        Call(SetActorRotation, ACTOR_SELF, 0, 0, LVar0)
                        Set(LVar1, LVar4)
                        Set(LVar2, LVar5)
                        Set(LVar3, LVar6)
                        Wait(1)
                    EndLoop
                EndThread
                Thread
                    Wait(6)
                    Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_PlayerGoomba_Midair)
                EndThread
                Call(JumpToGoal, ACTOR_SELF, 16, FALSE, TRUE, FALSE)
                Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_PlayerGoomba_Sleep)
                Call(SetActorScale, ACTOR_SELF, Float(1.1), Float(0.8), Float(1.0))
                Call(SetActorDispOffset, ACTOR_SELF, 0, 5, 0)
                Wait(1)
                Call(SetActorScale, ACTOR_SELF, Float(1.3), Float(0.5), Float(1.0))
                Call(SetActorDispOffset, ACTOR_SELF, 0, -2, 0)
                Wait(1)
                Call(SetActorScale, ACTOR_SELF, Float(1.0), Float(1.0), Float(1.0))
                Call(SetActorDispOffset, ACTOR_SELF, 0, 7, 0)
                Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_PlayerGoomba_Hurt)
                Wait(5)
                IfEq(LVarA, HIT_RESULT_LUCKY)
                    Call(EnemyTestTarget, ACTOR_SELF, LVar0, DAMAGE_TYPE_TRIGGER_LUCKY, 0, 0, 0)
                EndIf
                Wait(5)
                Call(SetActorDispOffset, ACTOR_SELF, 0, 0, 0)
                Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_PlayerGoomba_Midair)
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
                    Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_PlayerGoomba_Midair)
                EndThread
                Call(JumpToGoal, ACTOR_SELF, 15, FALSE, TRUE, FALSE)
                Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_PlayerGoomba_Dizzy)
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
                Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_PlayerGoomba_Idle)
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
                        Call(CalculateArcsinDeg, LVar1, LVar2, LVar4, LVar5, LVar0)
                        Call(SetActorRotation, ACTOR_SELF, 0, 0, LVar0)
                        Set(LVar1, LVar4)
                        Set(LVar2, LVar5)
                        Set(LVar3, LVar6)
                        Wait(1)
                    EndLoop
                EndThread
                Thread
                    Wait(6)
                    Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_PlayerGoomba_Midair)
                EndThread
                Call(JumpToGoal, ACTOR_SELF, 16, FALSE, TRUE, FALSE)
                Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_PlayerGoomba_Tense)
                Call(SetActorScale, ACTOR_SELF, Float(1.1), Float(0.8), Float(1.0))
                Wait(1)
                Call(SetActorScale, ACTOR_SELF, Float(1.3), Float(0.5), Float(1.0))
                Wait(1)
        EndSwitch
        Call(EnemyDamageTarget, ACTOR_SELF, LVar0, 0, 0, 0, DMG_HEADBONK, BS_FLAGS1_TRIGGER_EVENTS)
    EndLoop
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
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_PlayerGoomba_Idle)
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
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_PlayerGoomba_Idle)
            Wait(3)
            Call(YieldTurn)
            Call(SetAnimationRate, ACTOR_SELF, PRT_MAIN, Float(2.0))
            Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_PlayerGoomba_Run)
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

EvtScript EVS_Attack_Multibonk = {
    Call(UseIdleAnimation, ACTOR_SELF, FALSE)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)
    // Attack Code here
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, TRUE)
    Return
    End
};

EvtScript EVS_Move_RallyWink = {
    Call(UseIdleAnimation, ACTOR_SELF, FALSE)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)
    // Move Code here
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, TRUE)
    Return
    End
};

EvtScript EVS_Move_Charge = {
    Call(UseIdleAnimation, ACTOR_SELF, FALSE)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)
    // Move Code here
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, TRUE)
    Return
    End
};

}; // namespace player_goomba

ActorBlueprint PlayerGoomba = {
    .flags = 0,
    .maxHP = 2,
    .type = ACTOR_TYPE_GOOMBA,
    .level = ACTOR_LEVEL_GOOMBA,
    .partCount = ARRAY_COUNT(player_goomba::ActorParts),
    .partsData = player_goomba::ActorParts,
    .initScript = &player_goomba::EVS_Init,
    .statusTable = player_goomba::StatusTable,
    .escapeChance = 90,
    .airLiftChance = 100,
    .hurricaneChance = 90,
    .spookChance = 100,
    .upAndAwayChance = 95,
    .spinSmashReq = 0,
    .powerBounceChance = 100,
    .coinReward = 1,
    .size = { 24, 24 },
    .healthBarOffset = { 0, 0 },
    .statusIconOffset = { -10, 20 },
    .statusTextOffset = { 10, 20 },
};

}; // namespace battle::actor
