#pragma once

#ifndef OLC_SLIDER_PGEX_HPP
#define OLC_SLIDER_PGEX_HPP

#include <olcPixelGameEngine.h>
#include <Extensions/olcPGEX_LayerController.h>
#include <assert.h>

class Slider : public olc::PGEX
{
public:
    Slider() : olc::PGEX(true) 
    {
        m_Marker.radius = 2.0f;
    }
    
    void Activate(bool on) { m_Active = on; }

    void SetRange(float min, float max, float initial_value) 
    {
        m_FullValue = max;
        m_MinValue = min;
        m_Value = initial_value;
        m_LeftMost = -m_Length * 0.5f;
        m_RightMost = m_Length * 0.5f;
        m_Marker.pos.x = initial_value / max * m_Length - m_RightMost;
    }

    void SetSliderLength(float length) { m_Length = length; }

    float GetRatio() { return m_Value / m_FullValue; }
    float GetValue() { return m_Value; }

    void Draw()
    {
        if (m_Active)
        {
            olc::LayerController::SetActiveLayerToDebugLayer();
            pge->DrawLine(m_Pos + olc::vf2d(m_LeftMost, 0.0f), m_Pos + olc::vf2d(m_RightMost, 0.0f), olc::DARK_BLUE);
            pge->FillCircle(m_Marker.pos + m_Pos, m_Marker.radius, olc::RED);

            std::string str = std::to_string (floorf(GetValue() * 10) / 10);
            str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
            str.erase ( str.find_last_not_of('.') + 1, std::string::npos );
            
            pge->DrawString(m_Marker.pos + m_Pos - olc::vf2d(4.0f, 10.0f), str);
        }
    }

private:
    struct Marker {
        olc::vf2d pos;
        float radius;
    };

    void OnBeforeUserUpdate(float &fElapsedTime) override
    {
        // Get Mouse Position
            // If mouse clicks on slider update ratio
        
        if (m_Active)
        {
            olc::vf2d mouse_pos = pge->GetMousePos();

            if (((m_Marker.pos + m_Pos) - mouse_pos).mag2() < std::pow(m_MouseRadius + m_Marker.radius, 2))
            if (pge->GetMouse(0).bHeld)
            {
                float marker_x = mouse_pos.x - m_Pos.x;
                
                m_Marker.pos.x = std::max(std::min(marker_x, m_RightMost), m_LeftMost);
                m_Value = (m_Marker.pos.x + m_RightMost) / m_Length * (m_FullValue - m_MinValue) + m_MinValue;
            }
        }
    }
    
    olc::Sprite *m_SliderCanvas = nullptr;
    olc::Decal m_Decal = nullptr;

    bool m_Active = true;

    float m_Value = 0.0f;

    float m_FullValue, m_MinValue; // Note that this is the real Maximum Value

    float m_Length = 200.0f;
    float m_LeftMost = 0.0f;
    float m_RightMost = 0.0f;
    olc::vf2d m_Pos = {160.0f, 12.0f};
    float m_MouseRadius = 8.0f;
    Marker m_Marker;
};

#endif