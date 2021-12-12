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

#include <olcPixelGameEngine.h>
#include <Camera.hpp>
#include <HitBoxSystem/HitBoxSystem.hpp>
#include <Shapes/ShapeSystem.hpp>
#include <DebugController.hpp>
#include <DeltaSpeedModifier.hpp>

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
        floor->Size = {CanvasSize.x * 0.75f, 8.0f};
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

        DeltaSpeedModifier::SetSpeed(1.0f);

        return true;
    }

    bool OnUserUpdate(float delta) override
    {
        olc::LayerController::SafelyClearDebuglayer(olc::BLANK);
        Clear(olc::BLANK);

        p.Update(DeltaSpeedModifier::GetDelta());

        ShapeSystem::rGet().Update(DeltaSpeedModifier::GetDelta());
        HitBoxSystem::rGet().Update(DeltaSpeedModifier::GetDelta());

        m_Cam2d.SetCameraPosition(p.GetGlobalPosition());

        p.Draw(m_Cam2d);

        ShapeSystem::rGet().Draw(m_Cam2d);
        HitBoxSystem::rGet().Draw(m_Cam2d);

        return true;
    }

private:
    Camera2d m_Cam2d;

    Player p;
    ShapeSystem::sysRect floor = std::make_shared<Rectangle>();
};

#endif