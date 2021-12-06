#pragma once
#ifndef PARALLAX_BACKGROUND_HPP
#define PARALLAX_BACKGROUND_HPP

#include "olcPixelGameEngine.h"
#include "Extensions/olcPGEX_ResourceManager.h"
#include "FileChecker.hpp"
#include "Camera.hpp"

struct ParallaxLayer
{
    olc::Decal *Decal = nullptr;

    olc::vf2d Origin = {0.0f, 0.0f};
    float Distance = 0.0f;
    int ZedIndex = 0;
};

class ParallaxBackground
{
public: // Public Functions
    ParallaxBackground() {}
    ~ParallaxBackground() 
    {
        if (mParallaxLayers != nullptr)
            delete mParallaxLayers;
    }

    bool Initialize(const std::vector<std::string> &pathsToSpriteFiles, float motionSpeed, float layerSpeedDifference) // Should be listed top to bottom
    {
        if (!file::batchDoesExist(pathsToSpriteFiles))
            return false;

        mParallaxLayers = new ParallaxLayer[pathsToSpriteFiles.size()];
        mParallaxLayerArraySize = pathsToSpriteFiles.size();

        if (mParallaxLayers != nullptr)
        for (int i = 0; i < mParallaxLayerArraySize; i++)
        {
            olc::Sprite *p_sprite_cache = olc::ResourceManager::GetSprite(pathsToSpriteFiles[i]);
            mParallaxLayers[i].Decal    = new olc::Decal(p_sprite_cache);
            mParallaxLayers[i].Origin   = olc::vf2d(p_sprite_cache->width, p_sprite_cache->height) * 0.5f;
            mParallaxLayers[i].ZedIndex = i;
            mParallaxLayers[i].Distance = 1.0f / (motionSpeed + layerSpeedDifference * i);
        }

        return true;
    }

    void Draw(Camera2d *cam2d)
    {
        if (mParallaxLayers != nullptr)
        for (int i = mParallaxLayerArraySize - 1; i >= 0; i--)
            cam2d->rGetRenderer().DrawDecal(GlobalPosition - mParallaxLayers[i].Origin - cam2d->rGetRenderer().GetWorldOffset() * mParallaxLayers[i].Distance, mParallaxLayers[i].Decal);
    }

private: // Private Functions

public: // Public Variables
    olc::vf2d GlobalPosition = {0.0f, 0.0f};

private: // Private Variables
    ParallaxLayer *mParallaxLayers = nullptr;
    int mParallaxLayerArraySize = 0;
};

#endif