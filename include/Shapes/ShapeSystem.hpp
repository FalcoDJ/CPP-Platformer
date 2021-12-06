#pragma once

#ifndef SHAPE_SYSTEM_HPP
#define SHAPE_SYSTEM_HPP

#include <limits>
#include <memory>

#include "olcPixelGameEngine.h"
#include "Extensions/olcPGEX_LayerController.h"
#include "Camera.hpp"

class Rectangle
{
public: // Public Functions
    Rectangle() {}
    Rectangle(olc::vf2d size) : Size(size) {}
    ~Rectangle() {}

    const olc::vf2d GlobalCenter()
    {
        return GlobalPosition + Size * 0.5f;
    }

    const olc::vf2d Far()
    {
        return GlobalPosition + Size;
    }
public: // Public Enum
    enum CollisionDirection {
        ALL   =0,
        UP    =1,
        DOWN  =2,
        LEFT  =3,
        RIGHT =4
    };

public: // Public Variables
    olc::vf2d GlobalPosition = {0.0f, 0.0f}, Velocity = {0.0f, 0.0f}, Size = {0.0f, 0.0f};
    int Layer = 0;

    CollisionDirection CollisionDirectionMode = CollisionDirection::ALL;
    bool IsOnGround = false, IsOnCeiling = false, IsOnWallLeft = false, IsOnWallRight = false;
};

class ShapeSystem : public olc::PGEX
{
public: // Public Member Classes
    using sysRect = std::shared_ptr<Rectangle>;

public: // Public Functions
    ShapeSystem() : olc::PGEX(true) {}
    ~ShapeSystem() {}

    static ShapeSystem &rGet()
    {
        static ShapeSystem Sys;
        return Sys;
    }
    int GetLayer(const std::string &layerName)
    {
        auto &layer_map = rGet().mLayerMap;
        auto key_value_pair = layer_map.find(layerName);

        // check if layer exists
        if (key_value_pair != layer_map.end())
        {
            // it does so return its' intger IDs
            return key_value_pair->second;
        }
        else
        {
            // it doesn't create it
            auto &layer = layer_map[layerName];
            layer = layer_map.size() - 1;

            return layer;
        }
    }
    void AddStaticRectangle(sysRect &staticRect)
    {
        mStaticRectangles.push_back(staticRect);
    }
    void AddDynamicRectangle(sysRect &dynamicRect)
    {
        mDynamicRectangles.push_back(dynamicRect);
    }
    void RemoveStaticRectangle(sysRect &staticRect)
    {
        mStaticRectangles.erase(std::find(mStaticRectangles.begin(), mStaticRectangles.end(), staticRect));
    }
    void RemoveDynamicRectangle(sysRect &staticRect)
    {
        mStaticRectangles.erase(std::find(mStaticRectangles.begin(), mStaticRectangles.end(), staticRect));
    }
    
    void Draw(Camera2d &cam2d)
    {
        olc::LayerController::SafelyClearDebuglayer(olc::BLANK);
        
        for (auto &dynamic_rect : mDynamicRectangles)
        {
            if (dynamic_rect == nullptr)
                continue;

            cam2d.rGetRenderer().FillRect(dynamic_rect->GlobalPosition, dynamic_rect->Size, mDynamicRectangleColor);
            cam2d.rGetRenderer().DrawRect(dynamic_rect->GlobalPosition, dynamic_rect->Size, olc::WHITE);
            cam2d.rGetRenderer().Draw(dynamic_rect->GlobalCenter(), olc::WHITE);
        }

        for (auto &static_rect : mStaticRectangles)
        {
            if (static_rect == nullptr)
                continue;

            cam2d.rGetRenderer().FillRect(static_rect->GlobalPosition, static_rect->Size, mStaticRectangleColor);

            if (static_rect->CollisionDirectionMode != Rectangle::CollisionDirection::ALL)
            {
               switch(static_rect->CollisionDirectionMode)
                {
                    case Rectangle::CollisionDirection::UP:
                        cam2d.rGetRenderer().DrawLine(static_rect->GlobalPosition, {static_rect->Far().x, static_rect->GlobalPosition.y}, olc::WHITE);
                        break;
                    case Rectangle::CollisionDirection::DOWN:
                        cam2d.rGetRenderer().DrawLine({static_rect->GlobalPosition.x, static_rect->Far().y}, static_rect->Far(), olc::WHITE);
                        break;
                    case Rectangle::CollisionDirection::LEFT:
                        cam2d.rGetRenderer().DrawLine({static_rect->Far().x, static_rect->GlobalPosition.y}, static_rect->Far(), olc::WHITE);
                        break;
                    case Rectangle::CollisionDirection::RIGHT:
                        cam2d.rGetRenderer().DrawLine(static_rect->GlobalPosition, {static_rect->GlobalPosition.x, static_rect->Far().y}, olc::WHITE);
                        break;
                    default:
                        break;
                } 
            }
            else
            {
                cam2d.rGetRenderer().DrawRect(static_rect->GlobalPosition, static_rect->Size, olc::WHITE);
            }

            cam2d.rGetRenderer().Draw(static_rect->GlobalCenter(), olc::WHITE);
        }
    }

private: // Private Functions
    Rectangle GetBroadPhaseBox(Rectangle *dynamicRectangle, const float &fElapsedTime)
    { 
        Rectangle broad_phase_box; 

        // Expand size by velocity multiplied by time
        broad_phase_box.Size.x           = dynamicRectangle->Velocity.x > 0 ? dynamicRectangle->Velocity.x * fElapsedTime + dynamicRectangle->Size.x : dynamicRectangle->Size.x - dynamicRectangle->Velocity.x * fElapsedTime;  
        broad_phase_box.Size.y           = dynamicRectangle->Velocity.y > 0 ? dynamicRectangle->Velocity.y * fElapsedTime + dynamicRectangle->Size.y : dynamicRectangle->Size.y - dynamicRectangle->Velocity.y * fElapsedTime;  

        // Adjust position to compensate for expanded size
        broad_phase_box.GlobalPosition.x = dynamicRectangle->Velocity.x > 0 ? dynamicRectangle->GlobalPosition.x : dynamicRectangle->GlobalPosition.x + dynamicRectangle->Velocity.x * fElapsedTime;
        broad_phase_box.GlobalPosition.y = dynamicRectangle->Velocity.y > 0 ? dynamicRectangle->GlobalPosition.y : dynamicRectangle->GlobalPosition.y + dynamicRectangle->Velocity.y * fElapsedTime;
        
        return broad_phase_box;
    }
    bool IsPointVsRect(const olc::vf2d &point, Rectangle *rectangle)
    {
        return (rectangle != nullptr &&
                point.x >= rectangle->GlobalPosition.x && point.y >= rectangle->GlobalPosition.x &&
                point.x <= rectangle->Far().x && point.y <= rectangle->Far().y);
    }

    bool TestRectangleVsRectangle(Rectangle &&rectangle1, Rectangle *rectangle2)
    {
        return (rectangle2 != nullptr &&
                rectangle1.GlobalPosition.x <= rectangle2->Far().x && rectangle1.Far().x >= rectangle2->GlobalPosition.x &&
                rectangle1.GlobalPosition.y <= rectangle2->Far().y && rectangle1.Far().y >= rectangle2->GlobalPosition.y);
    }

    bool TestLineVsRectangle(const olc::vf2d &lineOrigin, const olc::vf2d &lineDirection, Rectangle &rectangle, olc::vf2d &contact_point, olc::vf2d &contact_normal, float &t_hit_near)
    {
        olc::vf2d t_near = (rectangle.GlobalPosition - lineOrigin) / lineDirection;
        olc::vf2d t_far  = (rectangle.Far() - lineOrigin) / lineDirection;

        if (t_far.x < t_near.x) std::swap(t_far.x, t_near.x);
        if (t_far.y < t_near.y) std::swap(t_far.y, t_near.y);

        if (t_near.x > t_far.y || t_near.y > t_far.x)
            return false;

            t_hit_near = std::max(t_near.x, t_near.y);
        float t_hit_far  = std::min(t_far.x, t_far.y);

        if (t_hit_far < 0) return false;

        contact_point = lineOrigin + t_hit_near * lineDirection;

        if (t_near.x > t_near.y)
            if (lineDirection.x < 0)
                contact_normal = {1.0f, 0.0f};
            else
                contact_normal = {-1.0f, 0.0f};
        else if (t_near.x < t_near.y)
            if (lineDirection.y < 0)
                contact_normal = {0.0f, 1.0f};
            else
                contact_normal = {0.0f, -1.0f};

        return true;
    }

    bool TestDynamicRectVsRect(Rectangle *dynamicRectangle, Rectangle *staticRectangle, olc::vf2d &contactPoint, olc::vf2d &contactNormal, float &contactTime, const float &fElapsedTime)
    {
        if (dynamicRectangle == nullptr || staticRectangle == nullptr)
            return false;

        if (dynamicRectangle->Velocity == olc::vf2d(0.0f, 0.0f))
            return false;
        
        Rectangle expanded_rectangle;
        expanded_rectangle.GlobalPosition = staticRectangle->GlobalPosition - dynamicRectangle->Size * 0.5f;
        expanded_rectangle.Size = staticRectangle->Size + dynamicRectangle->Size;

        if (TestLineVsRectangle(dynamicRectangle->GlobalCenter(), dynamicRectangle->Velocity * fElapsedTime, expanded_rectangle, contactPoint, contactNormal, contactTime))
            return (contactTime >= 0.0f && contactTime < 1.0f);
        else
            return false;
    }

    void OnBeforeUserUpdate(float &fElapsedTime) override
    {
        std::vector<sysRect> kept_dynamic_rectangles_vec;
        std::vector<sysRect> kept_static_rectangles_vec;

        bool looped_through_static_rectangles_once = false;

        for (auto &dynamic_rect1 : mDynamicRectangles)
        {
            if (dynamic_rect1 == nullptr)
                continue;

            kept_dynamic_rectangles_vec.push_back(dynamic_rect1);

            dynamic_rect1->IsOnGround    = false;
            dynamic_rect1->IsOnWallRight = false;
            dynamic_rect1->IsOnWallLeft  = false;

            olc::vf2d contact_point, contact_normal = {0.0f, 0.0f};
            float contact_time = 0.0f;

            // Work out collision point, add it to vector along with the static rectangle
            for (auto &static_rectangle : mStaticRectangles)
            {
                if (static_rectangle == nullptr)
                    continue;

                if (!looped_through_static_rectangles_once)
                    kept_static_rectangles_vec.push_back(static_rectangle);

                if (static_rectangle->Layer == dynamic_rect1->Layer)
                    continue;

                if (TestRectangleVsRectangle(GetBroadPhaseBox(dynamic_rect1.get(), fElapsedTime), static_rectangle.get()))
                if (TestDynamicRectVsRect(dynamic_rect1.get(), static_rectangle.get(), contact_point, contact_normal, contact_time, fElapsedTime))
                {
                    dynamic_rect1->IsOnGround    = (contact_normal.y < 0.0f) || dynamic_rect1->IsOnGround;
                    dynamic_rect1->IsOnWallRight = (contact_normal.x < 0.0f) || dynamic_rect1->IsOnWallRight;
                    dynamic_rect1->IsOnWallLeft  = (contact_normal.x > 0.0f) || dynamic_rect1->IsOnWallLeft;

                    bool resolve_collision = true;

                    if (static_rectangle->CollisionDirectionMode != Rectangle::CollisionDirection::ALL)
                    {
                        if (contact_normal.y > 0 && static_rectangle->CollisionDirectionMode != Rectangle::CollisionDirection::DOWN) // Dynamic collided with top of static
                            resolve_collision = false, dynamic_rect1->IsOnGround = false;
                        else
                        if (contact_normal.y < 0 && static_rectangle->CollisionDirectionMode != Rectangle::CollisionDirection::UP) // Dynamic collided with bottom of static
                            resolve_collision = false, dynamic_rect1->IsOnCeiling = false;
                        else
                        if (contact_normal.x > 0 && static_rectangle->CollisionDirectionMode != Rectangle::CollisionDirection::RIGHT) // Dynamic collided with left of static
                            resolve_collision = false, dynamic_rect1->IsOnWallRight = false;
                        else
                        if (contact_normal.x < 0 && static_rectangle->CollisionDirectionMode != Rectangle::CollisionDirection::LEFT) // Dynamic collided with right of static
                            resolve_collision = false, dynamic_rect1->IsOnWallLeft = false;
                        
                    }

                    if (resolve_collision)
                        dynamic_rect1->Velocity += contact_normal * olc::vf2d(std::abs(dynamic_rect1->Velocity.x),std::abs(dynamic_rect1->Velocity.y)) * (1 - contact_time);
                }
            }

            dynamic_rect1->GlobalPosition += dynamic_rect1->Velocity * fElapsedTime;

            looped_through_static_rectangles_once = true;
        }

        // Keep valid rectangles
        mDynamicRectangles.clear();
        mDynamicRectangles.swap(kept_dynamic_rectangles_vec);
        mStaticRectangles.clear();
        mStaticRectangles.swap(kept_static_rectangles_vec);
    }

private: // Private Variables
    std::vector<sysRect> mDynamicRectangles;
    std::vector<sysRect> mStaticRectangles;
    std::map<std::string, int> mLayerMap;

    olc::Pixel mDynamicRectangleColor = olc::Pixel( 43, 239, 69, 127);
    olc::Pixel mStaticRectangleColor  = olc::Pixel(239, 206, 43, 127);
};

#endif