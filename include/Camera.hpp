#pragma once

#ifndef CAMERA_2D_HPP
#define CAMERA_2D_HPP

#include "olcPixelGameEngine.h"

#ifndef OLC_PGEX_TRANSFORMEDVIEW
#define OLC_PGEX_TRANSFORMEDVIEW
#include "Extensions/olcPGEX_TransformedView.h"
#endif

#include "Math.hpp"

class Camera2d : public olc::PGEX
{
public: // Public Functions
    Camera2d() : olc::PGEX(true) {}
    ~Camera2d() {}

    olc::TransformedView &rGetRenderer() { return mRenderer; }
    olc::PixelGameEngine *pGetPixelGameEngine() { return pge; }

    void SetCameraSize(olc::vi2d cameraSize) { mCameraSize = cameraSize; mRenderer.Initialise(mCameraSize); }
    void SetCameraOffset(olc::vf2d offset) { mCameraOffset = offset; std::cout << offset << std::endl; }
    void SetCameraEasing(bool enabled, float easing = 0.0f) 
    { 
        mLinearlyInterpolateCamera = enabled;
        mEasing = std::max(0.0f, std::min(easing, 1.0f));
    }
    void SetCameraPosition(olc::vf2d cameraPosition) { mActualCameraPosition = cameraPosition - mCameraOffset / mRenderer.GetWorldScale(); }

private: // Private Functions
	void OnBeforeUserUpdate(float& fElapsedTime) override
    {
        if (!mLinearlyInterpolateCamera) mCameraPosition = mActualCameraPosition;
        else
        mCameraPosition = olc::vf2d(Math::lerp(mCameraPosition.x, mActualCameraPosition.x, 1 - std::pow(fElapsedTime, mEasing)),
                                    Math::lerp(mCameraPosition.y, mActualCameraPosition.y, 1 - std::pow(fElapsedTime, mEasing)));

        mRenderer.SetWorldOffset(mCameraPosition);
    }

public: // Public Variables

private: // Private Variables
    olc::TileTransformedView mRenderer;
    olc::vf2d mCameraPosition = {0,0};
    olc::vf2d mActualCameraPosition = {0,0};
    olc::vf2d mCameraOffset = {0,0}; // Usually the center of the Camera
    olc::vf2d mCameraSize = {0,0};

    float mEasing = 0.0;
    bool mLinearlyInterpolateCamera = false;
};

#endif