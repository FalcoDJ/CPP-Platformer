#pragma once
#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "olcPixelGameEngine.h"
#include "LevelSystem/ParallaxBackground.hpp"
#include "Extensions/olcPGEX_ResourceManager.h"

class Level
{
public:  // Public Functions
    Level() {}
    ~Level() 
    {
        // Reset mRectangles array
        ClearRectangles();
    }

    void ClearRectangles()
    {
        for (int i = 0; i < mRectanglesLength; i++)
        {
            ShapeSystem::rGet().RemoveStaticRectangle(mRectangles[i]);
        }
        delete[] mRectangles;
    }

    void AddRectangles(const std::vector<ShapeSystem::sysRect> &rectangleVec)
    {
        // Reset mRectangles array
        ClearRectangles();

        // Tell this level how many rectangles it should have
        mRectanglesLength = rectangleVec.size();

        /*
            TODO - fix wierd bug where deleting[] mRectangles does not cause the 
                Rectangles in the array to be removed from the ShapeSystem, thus creating a
                memory leak, and junking up the system with rectangles.
        */
        
        mRectangles = new ShapeSystem::sysRect[mRectanglesLength];

        for (int i = 0; i < mRectanglesLength; i++)
        {
            mRectangles[i] = ShapeSystem::sysRect(rectangleVec[i]);
            mRectangles[i]->Layer = ShapeSystem::rGet().GetLayer(mLayer);
        }

    }

    void AddBackground(std::string backgroundSpritePath)
    {
        mPathsToParallaxLayers.push_back(backgroundSpritePath);
    }
    
    void AddStage(std::string stageSpritePath)
    {
        mPathToStageSprite = stageSpritePath;
    }

    bool Initialize(olc::vf2d globalPos = {0.0f, 0.0f})
    {
        float level_speed = 10.0f;
        float layer_speed_difference = 2.0f;
        GlobalPosition = globalPos;
        mParallaxBackground.Initialize(mPathsToParallaxLayers, level_speed, layer_speed_difference);
        mParallaxBackground.GlobalPosition = GlobalPosition;

        olc::Sprite *stage_sprite = olc::ResourceManager::GetSprite(mPathToStageSprite);
        mStageOrigin = olc::vf2d(stage_sprite->width, stage_sprite->height) * 0.5f;
        StageDecal = new olc::Decal(stage_sprite);

        if (mRectangles != nullptr && mRectanglesLength > 0)
        for (int i = 0; i < mRectanglesLength; i++)
        {
            ShapeSystem::rGet().AddStaticRectangle(mRectangles[i]);
        }

        return true;
    }

    void Draw(Camera2d *cam2d)
    {
        mParallaxBackground.Draw(cam2d);
        cam2d->rGetRenderer().DrawDecal(GlobalPosition - mStageOrigin, StageDecal);
    }

private: // Priavte Functions

public:  // Public Variables

private: // Priavte Variables
    ParallaxBackground mParallaxBackground;
    
    olc::vf2d mStageOrigin = {0.0f, 0.0f};
    olc::Decal *StageDecal = nullptr;
    std::string mPathToStageSprite;
    std::vector<std::string> mPathsToParallaxLayers;
    
    std::string mLayer = "world_layer";
    olc::vf2d GlobalPosition = {0.0f, 0.0f};
    ShapeSystem::sysRect *mRectangles = nullptr;
    int mRectanglesLength = 0;
};

#endif