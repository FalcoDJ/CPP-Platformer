#pragma once

#ifndef ENGINE_HPP
#define ENGINE_HPP

#ifndef OLC_PGE_APPLICATION
#define OLC_PGE_APPLICATION
#endif

#ifndef OLC_INPUTMAP
#define OLC_INPUTMAP
#endif

#ifndef OLC_PGEX_RESOURCE_MANAGER
#define OLC_PGEX_RESOURCE_MANAGER
#endif

#ifndef AUDIO_LISTENER_IMPLEMENTATION
#define AUDIO_LISTENER_IMPLEMENTATION
#endif

#ifndef AUDIO_SOURCE_IMPLEMENTATION
#define AUDIO_SOURCE_IMPLEMENTATION
#endif

#ifndef OLC_PGE_GAMEPAD
#define OLC_PGE_GAMEPAD
#endif

#include <Extensions/olcPGEX_AudioSource.h>
#include <Extensions/olcPGEX_AudioListener.h>
#include <Extensions/olcPGEX_Gamepad.h>

#include <olcPixelGameEngine.h>
#include <Extensions/olcPGEX_DeltaSpeedModifier.h>
#include <Shapes/ShapeSystem.hpp>
#include <HitBoxSystem/HitBoxSystem.hpp>
#include <DebugController.hpp>
#include <Camera.hpp>
#include <Extensions/olcPGEX_Slider.h>
#include <DebugController.hpp>
#include <AudioController.hpp>

#include "Entities/Player/Player.hpp"

class Engine : public olc::PixelGameEngine
{
public:
    Engine()
    {
        ShapeSystem::rGet();
        HitBoxSystem::rGet();

        PixelScale = std::max(WindowSize.x / CanvasSize.x, WindowSize.y / CanvasSize.y);
        ShapeSystem::rGet().AddStaticRectangle(floor);
        floor->Size = {CanvasSize.x, 8.0f};
        floor->GlobalPosition = {0.0f, 150.0f};
        floor->Layer = ShapeSystem::rGet().GetLayer("world");
    }

    olc::vf2d WindowSize = {1024, 576}, CanvasSize = {320, 180};
    float PixelScale = 1;

private: 
    bool OnUserCreate() override
    {
        m_Cam2d.SetCameraSize(CanvasSize);
        m_Cam2d.SetCameraOffset(CanvasSize/2);
        m_Cam2d.SetCameraEasing(true, 0.2f);

        if (!p.Init()) return false;

        sl.SetSliderLength(100.0f);
        sl.SetRange(0.5f, 3.5f, 1.0f);

        DebugController::rGet().SetDebugging(true);

        AudioController::rGet()->AudioSystemInit();

        flip.AL = AudioController::rGet();
        flip.LoadAudioSample(AudioController::rGet()->audioSamples.size(), "assets/Audio/flip.wav");

        olc::GamePad::init();

        DeltaSpeedModifier::SetSpeed(0.0f);

        return true;
    }

    bool OnUserUpdate(float delta) override
    {
        olc::LayerController::SafelyClearDebuglayer(olc::BLANK);
        Clear(olc::BLANK);

        if (GetKey(olc::P).bPressed)
        {
            DebugController::rGet().SetDebugging(!db_key.IsDebuggerEnabled());
            sl.Activate(db_key.IsDebuggerEnabled());
            flip.Play(DeltaSpeedModifier::GetSpeed());
        }

        p.Update(DeltaSpeedModifier::GetDelta());

        ShapeSystem::rGet().Update(DeltaSpeedModifier::GetDelta());
        HitBoxSystem::rGet().Update(DeltaSpeedModifier::GetDelta());

        DeltaSpeedModifier::SetSpeed(sl.GetValue());

        m_Cam2d.SetCameraPosition(p.GetGlobalPosition());

        sl.Draw();
        p.Draw(m_Cam2d);

        if (db_key.IsDebuggerEnabled())
        {
            ShapeSystem::rGet().Draw(m_Cam2d);
            HitBoxSystem::rGet().Draw(m_Cam2d);
        }

        return true;
    }

private:
    DebugEnitity db_key;
    Camera2d m_Cam2d;

    Player p;
    ShapeSystem::sysRect floor = std::make_shared<Rectangle>();
    Slider sl;

    olcPGEX_AudioSource flip;
};

#endif