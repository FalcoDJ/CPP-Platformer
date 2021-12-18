#pragma once

/*
    ##----------------------------------------------------------------------##
    ##  Think of this singleton as the timemachine of the engine!           ##
    ##  By adjusting the SpeedModifier with SetSpeed(float),                ##
    ##  you can slow down or speed up the engine (if you use it properly).  ##
    ##----------------------------------------------------------------------##

    To use this singleton properly, include the header file and
    call GetDelta() wherever you would normally use GetElapsedTime().

    I think that this would usually be used in the OnUserUpdate() function,
    but an added benifit is that you can call GetDelta() from anywhere in your code.

*/


#ifndef DELTA_SPEED_MODIFIER_HPP
#define DELTA_SPEED_MODIFIER_HPP

#include <olcPixelGameEngine.h>
#include <assert.h>

class DeltaSpeedModifier : public olc::PGEX
{    
public:
    static float GetDelta() { return pge->GetElapsedTime() * rGet().m_SpeedModifier; }
    static float GetSpeed() { return rGet().m_SpeedModifier; }

    // Speed Modifier must always be greater than zero
    static void SetSpeed(float speedModifier) { assert(speedModifier > 0); rGet().m_SpeedModifier = speedModifier; }

private:
    DeltaSpeedModifier() {}
    ~DeltaSpeedModifier() {}
    
    static DeltaSpeedModifier &rGet()
    {
        static DeltaSpeedModifier dsm;
        return dsm;
    }

    float m_SpeedModifier = 1.0f;
};

#endif