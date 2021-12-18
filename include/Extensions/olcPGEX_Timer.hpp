#pragma once

#ifndef TIMER_HPP
#define TIMER_HPP

#include <olcPixelGameEngine.h>

#ifdef DELTA_SPEED_MODIFIER_HPP
#include "olcPGEX_DeltaSpeedModifier.h"
#endif

class Timer : public olc::PGEX
{
public: // Public Functions
    Timer() : olc::PGEX(true) {}
    ~Timer() {}

    void Start(float lifeTimeInSeconds)
    {
        mTotalDuration = lifeTimeInSeconds;
        mElapsedTime = 0.0f;
    }

    void Stop()
    {
        mElapsedTime = mTotalDuration;
    }

    bool IsRunning()
    {
        return mElapsedTime < mTotalDuration;
    }

    bool JustFinished()
    {
        return !IsRunning() && mWasRunning;
    }

    bool JustStarted()
    {
        return IsRunning() && !mWasRunning;
    }

private: // Private Functions
    void OnBeforeUserUpdate(float &fElapsedTime) override
    {
        mWasRunning = IsRunning();

        #ifdef DELTA_SPEED_MODIFIER_HPP
            if (IsRunning()) mElapsedTime += DeltaSpeedModifier::GetDelta();
        #else
            if (IsRunning()) mElapsedTime += fElapsedTime;
        #endif
    }

private: // Private Variables
    float mTotalDuration, mElapsedTime = 0.0f;
    bool mWasRunning = false;
};

#endif