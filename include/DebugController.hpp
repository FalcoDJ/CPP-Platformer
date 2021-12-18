#pragma once
#ifndef DEBUG_ENTITY
#define DEBUG_ENTITY

#include <vector>
#include <algorithm>

class DebugEnitity;

class DebugController
{
public: // Public Functions
    DebugController() {}
    ~DebugController() {}

    static DebugController &rGet()
    {
        static DebugController DbC;
        return DbC;
    }

    void RegisterInputMap(DebugEnitity *im)
    {
        if (std::find(mDebugEntityMap.begin(), mDebugEntityMap.end(), im) == mDebugEntityMap.end())
            mDebugEntityMap.push_back(im);
    }

    void SetDebugging(bool enabled);

private: // Private Variables
    std::vector<DebugEnitity*> mDebugEntityMap;
};

class DebugEnitity
{
public:
    DebugEnitity() { DebugController::rGet().RegisterInputMap(this); }
    ~DebugEnitity() {}

    bool IsDebuggerEnabled() { return DebugEnabled; }

private:
    bool DebugEnabled = false;

    friend class DebugController;
};

void DebugController::SetDebugging(bool enabled)
{
    for (auto &debug_entity : mDebugEntityMap)
    {
        debug_entity->DebugEnabled = enabled;
    }
}


#endif