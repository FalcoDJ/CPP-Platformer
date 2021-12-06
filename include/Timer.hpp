#pragma once
#ifndef TIMER_HPP
#define TIMER_HPP

#include "olcPixelGameEngine.h"

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
        if (IsRunning()) mElapsedTime += fElapsedTime;
    }

private: // Private Variables
    float mTotalDuration, mElapsedTime = 0.0f;
    bool mWasRunning = false;
};

#endif