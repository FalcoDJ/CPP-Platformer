#ifndef HIT_BOX_HANDLER
#define HIT_BOX_HANDLER

#include <memory>

#include "olcPixelGameEngine.h"
#include "Extensions/olcPGEX_LayerController.h"
#include "Camera.hpp"

const float FRAME_DURATION = 0.016f;

enum class HbEffect {
    DEFAULT = 0, // Used to indicate that no hitbox touched a hurtbox
    HURT = 1 // Has no effects, but does damage
};

struct HitBoxCircle
{
    HitBoxCircle() {}
    HitBoxCircle(float radius) : Radius(radius) {}
    ~HitBoxCircle() {}

    olc::vf2d GlobalPosition, Offset, Velocity = {0.0f,0.0f};
    float Radius = 0.0f;
    bool Enabled = true;
};

class HitBox : public olc::PGEX
{
public: // Public Functions
    HitBox() : olc::PGEX(true) {}
    HitBox(HitBoxCircle circle, int lifeInFrames, int layerId, float damage, olc::vf2d knockBackVector) : olc::PGEX(true),
    mLifeTime(lifeInFrames), Layer(layerId), Damage(damage), KnockBack(knockBackVector) {}

    bool IsAlive()
    {
        return mIsAlive;
    }

    bool JustDied()
    {
        return (mWasAlive && !mIsAlive);
    }

    void SetLifeTime(int lifeInFrames = 1)
    {
        mLifeTime = lifeInFrames;
        mIsAlive = true;
    }

    void QueueDestruction()
    {
        mIsAlive = false;
    }

    void Reset()
    {
        mDuration = 0.0f;
        mIsAlive = true;
        mWasAlive = true;
    }

    void OnAfterUserUpdate(float fElapsedTime)
    {
        mWasAlive = mIsAlive;

        if (mLifeTime != -1)
        {
            mDuration += fElapsedTime;
            if (mDuration / FRAME_DURATION >= mLifeTime)
                mIsAlive = false;
        }

        if (mIsAlive)
            Circle.GlobalPosition += Circle.Velocity * fElapsedTime;
    }

public: // Public Variables
    HitBoxCircle Circle;

    int Layer = 0; // HitBoxes wont interact with HurtBoxes of the same layer
    
    float Damage = 0.0f;
    HbEffect Effect = HbEffect::HURT;
    
    olc::vf2d KnockBack = {0.0f, 0.0f};
    float KnockBackModifier = 1.0f;

private: // Private Variables
    int mLifeTime = 1; // Duration in frames! (Frame time is usually 16ms)
    float mDuration = 0.0f; // Duration in seconds!
    bool mIsAlive = true;
    bool mWasAlive = true;
};

class HurtBox : public HitBox
{
    public: // Public Functions
        HurtBox()
        {
            Effect = HbEffect::DEFAULT;
            SetLifeTime(-1);
        }
        ~HurtBox(){}
    
    public: // Public Variables
        olc::vf2d OverlapDirection = {0.0f, 0.0f};
};

class HitBoxSystem : olc::PGEX
{
public: // Public Member Classes
    using sysHitBox  = std::shared_ptr<HitBox>;
    using sysHurtBox = std::shared_ptr<HurtBox>;

public: // Public Functions
    HitBoxSystem() : olc::PGEX(true) {}
    ~HitBoxSystem() {}

    static HitBoxSystem &rGet()
    {
        static HitBoxSystem HbS;
        return HbS;
    }

    int GetLayer(const std::string &layerName)
    {
        auto& layer_map = rGet().mLayerMap;
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
            auto& layer = layer_map[layerName];
            layer = layer_map.size() - 1;

            return layer;
        }
    }

    void AddHitBox(sysHitBox &hitBox)
    {
        mHitBoxes.push_back(hitBox);
        mHitBoxes.back()->Reset();
    }
    void AddHurtBox(sysHurtBox &hurtBox)
    {
        mHurtBoxes.push_back(hurtBox);
        mHurtBoxes.back()->Reset();
    }

    void Draw(Camera2d &cam2d)
    {
        olc::LayerController::SafelyClearDebuglayer(olc::BLANK);

        for (auto &hurt_box : rGet().mHurtBoxes)
        {
            if (hurt_box == nullptr)
                continue;
            
            cam2d.rGetRenderer().FillCircle(hurt_box->Circle.GlobalPosition + hurt_box->Circle.Offset, hurt_box->Circle.Radius, hurt_box->Circle.Enabled ? mHurtBoxColor : mBoxDisabled);
            cam2d.rGetRenderer().DrawCircle(hurt_box->Circle.GlobalPosition + hurt_box->Circle.Offset, hurt_box->Circle.Radius, olc::WHITE);
            cam2d.rGetRenderer().Draw(hurt_box->Circle.GlobalPosition + hurt_box->Circle.Offset, olc::WHITE);
        }

        for (auto &hit_box : rGet().mHitBoxes)
        {
            if (hit_box == nullptr)
                continue;
            
            cam2d.rGetRenderer().FillCircle(hit_box->Circle.GlobalPosition + hit_box->Circle.Offset, hit_box->Circle.Radius, hit_box->Circle.Enabled ? mHitBoxColor : mBoxDisabled);
            cam2d.rGetRenderer().DrawCircle(hit_box->Circle.GlobalPosition + hit_box->Circle.Offset, hit_box->Circle.Radius, olc::WHITE);
            cam2d.rGetRenderer().DrawLine(hit_box->Circle.GlobalPosition + hit_box->Circle.Offset, hit_box->Circle.GlobalPosition + hit_box->Circle.Offset + hit_box->KnockBack.norm() * (hit_box->Circle.Radius - 1.0f), olc::WHITE);
            cam2d.rGetRenderer().Draw(hit_box->Circle.GlobalPosition + hit_box->Circle.Offset, olc::WHITE);
        }
    }

    void OnBeforeUserUpdate(float &fElapsedTime) override
    {
        std::vector<sysHitBox>  kept_hit_boxes;
        std::vector<sysHurtBox> kept_hurt_boxes;

        bool looped_through_hit_boxes_once = false;

        for (auto &hurt_box : mHurtBoxes)
        {
            if (hurt_box == nullptr || hurt_box->JustDied())
                continue;

            hurt_box->Effect = HbEffect::DEFAULT;
            hurt_box->OverlapDirection = {0.0f, 0.0f};

            for (auto &hit_box : mHitBoxes)
            {
                if (hit_box == nullptr)
                    continue;

                // Check for overlaps between hurtboxes and hitboxes if they don't  share a layer
                if (hurt_box->Layer != hit_box->Layer && hurt_box->Circle.Enabled)
                {
                    olc::vf2d vec_distance = ((hit_box->Circle.GlobalPosition + hit_box->Circle.Offset) - (hurt_box->Circle.GlobalPosition + hurt_box->Circle.Offset));
                    float distance2 = vec_distance.mag2();
                    float sum_of_radii = (hit_box->Circle.Radius + hurt_box->Circle.Radius);
                    if (distance2 <= sum_of_radii * sum_of_radii)
                    {
                        // There was an overlap so transfer hitbox attack data to hurtbox
                        hurt_box->Damage = hit_box->Damage;
                        hurt_box->KnockBack = hit_box->KnockBack;
                        hurt_box->Effect = hit_box->Effect;
                        hurt_box->OverlapDirection = (hit_box->Circle.GlobalPosition - hurt_box->Circle.GlobalPosition).norm();
                    }
                }

                if (hit_box->JustDied())
                    continue;
                
                // prevent looping through hitboxes thousands of times per frame
                if (!looped_through_hit_boxes_once)
                    kept_hit_boxes.push_back(hit_box);
            }
            
            looped_through_hit_boxes_once = true;

            kept_hurt_boxes.push_back(hurt_box);
        }

        // keep valid hitboxes and hurtboxes
        mHitBoxes.clear();
        mHitBoxes.swap(kept_hit_boxes);
        mHurtBoxes.clear();
        mHurtBoxes.swap(kept_hurt_boxes);
    }

private: // Private Variables
    std::map<std::string, int> mLayerMap;
    std::vector<sysHitBox>  mHitBoxes;
    std::vector<sysHurtBox> mHurtBoxes;

    olc::Pixel mHitBoxColor  = olc::Pixel(239,  43,  85, 127);
    olc::Pixel mHurtBoxColor = olc::Pixel( 43, 144, 239, 127);
    olc::Pixel mBoxDisabled  = olc::Pixel(255, 255, 255, 127);
};


#endif