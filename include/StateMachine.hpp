#pragma once

#ifndef STATE_MACHINE_HPP

#include <assert.h>
#include <string>
#include <map>

struct StateMachineKey
{
    int Id = 0;
    std::string Name = "";
};

bool operator == (const int &i, const StateMachineKey &smk) { return smk.Id == i; }
bool operator != (const int &i, const StateMachineKey &smk) { return smk.Id != i; }

template<typename T>
class StateMachine
{
public:
    StateMachine() { NullState = AddState("null_0"); }
    ~StateMachine() { Parent = nullptr; }

    void Intialize(T *parent)
    {
        assert(parent != nullptr);
        Parent = parent;
    }

    void Update(float delta)
    {
        if (State != NullState)
        {
            Logic(delta);
            int transition = GetTransition(delta);
            if (transition != State)
            {
                SetState(transition);
            }
        }
    }

    friend T;

protected:
    virtual void Logic(float delta) = 0;
    virtual int GetTransition(float delta) = 0;
    virtual void EnterState(int newState, int oldState) = 0;
    virtual void ExitState(int oldState, int newState) = 0;

    int AddState(std::string stateName)
    {
        auto key_value_pair = States.find(stateName);

        if (key_value_pair == States.end())
        {
            States[stateName] = States.size();
            return States.size();
        }

        return key_value_pair->second;
    }

    void SetState(int newState)
    {
        PreviousState = State;
        State = newState;

        ExitState(PreviousState, newState);
        EnterState(newState, PreviousState);
    }

protected:
    T *Parent = nullptr;
    std::map<std::string, int> States;

public:
    int NullState = 0;
    int State = 0;
    int PreviousState = 0;
};

#endif