#pragma once

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <Extensions/olcPGEX_InputMap.h>
#include <Extensions/olcPGEX_Timer.hpp>
#include <FileChecker.hpp>
#include <Extensions/olcPGEX_ResourceManager.h>
#include <Extensions/olcPGEX_Animator2D.h>
#include <Math.hpp>
#include <StateMachine.hpp>
#include <Shapes/ShapeSystem.hpp>
#include <HitBoxSystem/HitBoxSystem.hpp>
#include <Camera.hpp>

class Player;

class p_StateMachine : public StateMachine<Player>
{
public:
    p_StateMachine()
    {
        IdleState.Name = "idle", IdleState.Id = AddState(IdleState.Name);
        JumpState.Name = "jump", JumpState.Id = AddState(JumpState.Name);
        FallState.Name = "fall", FallState.Id = AddState(FallState.Name);
        TurnState.Name = "turn", TurnState.Id = AddState(TurnState.Name);
        RunState.Name = "run", RunState.Id = AddState(RunState.Name);
    }

    void Logic(float delta) override;
    int GetTransition(float delta) override;
    void EnterState(int newState, int oldState) override;
    void ExitState(int oldState, int newState) override{};

private:
    StateMachineKey IdleState, JumpState, FallState, RunState, TurnState;
};

class Player
{
public:
    Player()
    {
        m_StateMachine.Intialize(this);
        ShapeSystem::rGet().AddDynamicRectangle(m_Bounds);
        m_Bounds->Size = {16, 24};
        m_Bounds->Layer = ShapeSystem::rGet().GetLayer("player");

        HitBoxSystem::rGet().AddHurtBox(m_HurtBox);
        m_HurtBox->Circle.Radius = 8.0f;
        m_HurtBox->Circle.GlobalPosition = m_Bounds->GlobalCenter();
        m_HurtBox->Layer = HitBoxSystem::rGet().GetLayer("player");

        m_JumpKey.Keys = {olc::Key::SPACE, olc::Key::UP, olc::Key::W};
        m_RightKey.Keys = {olc::Key::D, olc::Key::RIGHT};
        m_LeftKey.Keys = {olc::Key::A, olc::Key::LEFT};

        m_Gravity = 2 * m_MaxJumpHeight / pow(m_MaxJumpDuration, 2);
        m_MaxJumpSpeed = -std::sqrt(2 * m_Gravity * m_MaxJumpHeight);
        m_MinJumpSpeed = -std::sqrt(2 * m_Gravity * m_MinJumpHeight);
    }
    ~Player() {}

    bool Init()
    {
        if (file::doesExist("assets/Pixel Adventure 1/Free/Main Characters/Virtual Guy/Idle (32x32).png"))
            m_IdleDecal = new olc::Decal(olc::ResourceManager::GetSprite("assets/Pixel Adventure 1/Free/Main Characters/Virtual Guy/Idle (32x32).png"));
        else
            return false;

        if (file::doesExist("assets/Pixel Adventure 1/Free/Main Characters/Virtual Guy/Run (32x32).png"))
            m_RunDecal = new olc::Decal(olc::ResourceManager::GetSprite("assets/Pixel Adventure 1/Free/Main Characters/Virtual Guy/Run (32x32).png"));
        else
            return false;

        if (file::doesExist("assets/Pixel Adventure 1/Free/Main Characters/Virtual Guy/Jump (32x32).png"))
            m_JumpDecal = new olc::Decal(olc::ResourceManager::GetSprite("assets/Pixel Adventure 1/Free/Main Characters/Virtual Guy/Jump (32x32).png"));
        else
            return false;

        if (file::doesExist("assets/Pixel Adventure 1/Free/Main Characters/Virtual Guy/Fall (32x32).png"))
            m_FallDecal = new olc::Decal(olc::ResourceManager::GetSprite("assets/Pixel Adventure 1/Free/Main Characters/Virtual Guy/Fall (32x32).png"));
        else
            return false;
        
        if (file::doesExist("assets/Pixel Adventure 1/Free/Main Characters/Virtual Guy/Double Jump (32x32).png"))
            m_TurnDecal = new olc::Decal(olc::ResourceManager::GetSprite("assets/Pixel Adventure 1/Free/Main Characters/Virtual Guy/Double Jump (32x32).png"));
        else
            return false;

        m_Animator.AddAnimation(m_IdleAnimation, 0.825f, 11, m_IdleDecal, {0, 0}, {32, 32}, {16, 32});
        m_Animator.AddAnimation(m_RunAnimation,    0.9f, 12, m_RunDecal,  {0, 0}, {32, 32}, {16, 32});
        m_Animator.AddAnimation(m_TurnAnimation, 0.240f,  6, m_TurnDecal, {0, 0}, {32, 32}, {16, 32});
        m_Animator.AddAnimation(m_JumpAnimation, 0.825f,  1, m_JumpDecal, {0, 0}, {32, 32}, {16, 32});
        m_Animator.AddAnimation(m_FallAnimation, 0.825f,  1, m_FallDecal, {0, 0}, {32, 32}, {16, 32});

        return true;
    }

    const olc::vf2d &GetGlobalPosition() { return m_Bounds->GlobalPosition; }

    void Update(float delta)
    {
        GetInput(delta);
        m_ActualAnimationFlipH = Math::lerp(m_ActualAnimationFlipH, m_AnimationFlipH, 1 - std::pow(0.0005f, delta));

        m_StateMachine.Update(delta);

        m_Animator.UpdateAnimations(delta);

        if (m_WasOnGround && !m_Bounds->IsOnGround && !m_CoyoteTimer.IsRunning())
            m_CoyoteTimer.Start(m_CoyoteTimerDuration);

        m_ShouldFall = m_Bounds->Velocity.y > 0 && !m_Bounds->IsOnGround;
        if (m_ShouldFall)
            m_IsJumping = false, m_ShouldJump = false;
        
        m_ShouldRun = m_InputX != 0 && m_Bounds->IsOnGround;
        m_ShouldIdle = m_InputX == 0 && m_Bounds->IsOnGround;

        m_WasOnGround = m_Bounds->IsOnGround;

        m_HurtBox->Circle.GlobalPosition = m_Bounds->GlobalCenter();
    }

    void Draw(Camera2d &cam2d)
    {
        m_Animator.DrawAnimationFrame(cam2d.rGetRenderer().WorldToScreen(m_Bounds->GlobalPosition + m_Origin), 0.0f, cam2d.rGetRenderer().GetWorldScale() * olc::vf2d(m_ActualAnimationFlipH, 1.0f));
    }

    friend class p_StateMachine;

protected:
    void GetInput(float delta)
    {
        m_InputX = (float)m_RightKey.IsHeld - (float)m_LeftKey.IsHeld;
        m_AnimationFlipH = m_InputX != 0 ? m_InputX : m_AnimationFlipH;

        if (m_JumpKey.IsPressed && (m_Bounds->IsOnGround || m_CoyoteTimer.IsRunning()) && !m_IsJumping)
        {
            m_Bounds->Velocity.y = m_MaxJumpSpeed;
            m_ShouldJump = true;
            m_IsJumping = true;
            m_CoyoteTimer.Stop();
        }
        if (m_JumpKey.IsReleased && m_IsJumping && m_Bounds->Velocity.y < m_MaxJumpSpeed * 0.8f)
        {
            m_Bounds->Velocity.y = m_MinJumpSpeed;
            m_ShouldJump = false;
            m_IsJumping = false;
        }
    }
    void ApplyGravityToVelocity(float delta)
    {
        m_Bounds->Velocity.y += m_Gravity * delta;
    }
    void ApplyInputXToVelocity(float delta)
    {
        m_Bounds->Velocity.x = Math::move_toward(m_Bounds->Velocity.x, m_InputX * m_Speed, ACC_and_FRC * delta);
    }

    bool p_ShouldIdle() { return m_ShouldIdle; }
    bool p_ShouldFall() { return m_ShouldFall; }
    bool p_ShouldJump() { return m_ShouldJump; }
    bool p_ShouldRun() { return m_ShouldRun; }
    bool p_ShouldTurn() { return Math::SIGN(m_Bounds->Velocity.x) != Math::SIGN(m_InputX) && Math::SIGN(m_InputX) != 0; }

private:
    olc::Decal *m_IdleDecal;
    olc::Decal *m_RunDecal;
    olc::Decal *m_JumpDecal;
    olc::Decal *m_FallDecal;
    olc::Decal *m_TurnDecal;
    std::string m_IdleAnimation = "idle";
    std::string m_RunAnimation = "run";
    std::string m_JumpAnimation = "jump";
    std::string m_FallAnimation = "fall";
    std::string m_TurnAnimation = "turn";
    olcPGEX_Animator2D m_Animator;
    float m_AnimationFlipH = 1;
    float m_ActualAnimationFlipH = 1;
    olc::vf2d m_Origin = {8, 24};

    p_StateMachine m_StateMachine;
    olc::InputMap m_JumpKey, m_LeftKey, m_RightKey;
    ShapeSystem::sysRect m_Bounds = std::make_shared<Rectangle>();
    HitBoxSystem::sysHurtBox m_HurtBox = std::make_shared<HurtBox>();
    bool m_WasOnGround = false;
    Timer m_CoyoteTimer;
    float m_CoyoteTimerDuration = 0.115f;

    float m_InputX = 1.0f;
    float m_Speed = 180.0f;
    const float ACC_and_FRC = m_Speed * 2.0f;

    float m_Gravity = 0.0f;
    float m_MaxJumpHeight = 80.0f;
    float m_MinJumpHeight = 24.0f;
    float m_MaxJumpSpeed = 0.0f;
    float m_MinJumpSpeed = 0.0f;
    float m_MaxJumpDuration = 0.3f;
    bool m_IsJumping = false;

    bool m_ShouldJump = false;
    bool m_ShouldIdle = false;
    bool m_ShouldFall = false;
    bool m_ShouldRun = false;
};

void p_StateMachine::Logic(float delta)
{
    Parent->ApplyGravityToVelocity(delta);

    if (State == IdleState)
    {
        // do some Idle Stuff...
        // std::cout << IdleState.Name << "\n";
    }
    else if (State == FallState)
    {
        // do some Fall Stuff...
        // std::cout << FallState.Name << "\n";
    }
    else if (State == JumpState)
    {
        // do some Jump Stuff...
        // std::cout << JumpState.Name << "\n";
    }
    else if (State == RunState)
    {
        // do some Run Stuff...
        // std::cout << RunState.Name << "\n";
    }
    else if (State == TurnState)
    {
        // do some Turn Stuff...
        // std::cout << TurnState.Name << "\n";
    }

    Parent->ApplyInputXToVelocity(delta);
}

int p_StateMachine::GetTransition(float delta)
{
    if (State == NullState) return IdleState.Id;

    if (State == IdleState)
    {
        if (Parent->p_ShouldRun())
            return RunState.Id;
    }

    if (State == RunState)
    {
        // pass
    }

    if (State == TurnState)
    {
        if (Parent->p_ShouldRun() && !Parent->p_ShouldTurn())
            return RunState.Id;
    }

    if (State == FallState)
    {
        if (Parent->p_ShouldRun())
            return RunState.Id;
    }

    if (Parent->p_ShouldJump() && State != JumpState)
        return JumpState.Id;
    else if (State == JumpState)
    {
        if (Parent->p_ShouldFall())
            return FallState.Id;
    }

    if (Parent->p_ShouldIdle() && !Parent->p_ShouldTurn())
        return IdleState.Id;
    else if (Parent->p_ShouldTurn() && State != JumpState && State != FallState)
        return TurnState.Id;

    return State;
}

void p_StateMachine::EnterState(int newState, int oldState)
{
    Parent->m_Animator.StopAll();

    if (newState == RunState)
        Parent->m_Animator.Play(Parent->m_RunAnimation);
    
    if (newState == TurnState)
        Parent->m_Animator.Play(Parent->m_RunAnimation);

    if (newState == IdleState)
        Parent->m_Animator.Play(Parent->m_IdleAnimation);
    
    if (newState == JumpState)
        Parent->m_Animator.Play(Parent->m_JumpAnimation);
    
    if (newState == FallState && oldState == JumpState)
        Parent->m_Animator.Play(Parent->m_FallAnimation);
};

#endif