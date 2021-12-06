#pragma once

#pragma region readme_region
/*
    olcPGEX_InputMap.h

    +-------------------------------------------------------------+
	|         OneLoneCoder Pixel Game Engine Extension            |
	|                  InputMap View v1.0.0                       |
	+-------------------------------------------------------------+

    What is this?
	~~~~~~~~~~~~~
	This extension provides global input to your project and
    enables you to test for input from multiple keys much easier
    with less writing, making your code look nicer.

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2018 - 2021 OneLoneCoder.com

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma endregion
#pragma region example_code_region
// 0---------------------------------------------------0
// | Example:                                          |
// 0---------------------------------------------------0
/*
 Where you have define OLC_PGE_APPLICATION:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    #define OLC_INPUTMAP
    #include "olcPGEX_InputMap.h"


 Anywhere else in your code:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    InputMap JumpMap;
    JumpMap.Keys = {
        olc::Key::SPACE,
        olc::KEY::Up
    }
    if (JumpMap.Pressed)
    {
        //JUMP
    }

*/
#pragma endregion

#ifndef OLC_INPUTMAP_HPP
#define OLC_INPUTMAP_HPP

#include "olcPixelGameEngine.h"

namespace olc
{
    class InputMap;

    class InputMapHandler : public olc::PGEX
    {
    public: //Public Functions
        static InputMapHandler &rGet()
        {
            static InputMapHandler imh;
            return imh;
        }

        static olc::vf2d GetMousePosition() { return rGet().pge->GetMousePos(); }

        void RegisterInputMap(olc::InputMap *im)
        {
            if (std::find(mInputMaps.begin(), mInputMaps.end(), im) == mInputMaps.end())
			    mInputMaps.push_back(im);
        }

    private: // Private Functions
        InputMapHandler() : olc::PGEX(true) {}
        ~InputMapHandler() {}
        
        void OnBeforeUserUpdate(float& fElapsedTime) override;

    public: // Public Variables

    private: // Private Variables
        std::vector<InputMap*> mInputMaps;
    };

    class InputMap
    {
    public:
        InputMap() { InputMapHandler::rGet().RegisterInputMap(this); }
        ~InputMap() {}
        std::vector<olc::Key> Keys;

        bool IsPressed = false;
        bool IsReleased = false;
        bool IsHeld = false;
    };

};

#ifdef OLC_INPUTMAP
#undef OLC_INPUTMAP

void olc::InputMapHandler::OnBeforeUserUpdate(float& fElapsedTime)
{
    for (auto &CurrentInputMap : mInputMaps)
    {
        // Check to make sure that the map is valid
        if (CurrentInputMap == nullptr || CurrentInputMap->Keys.empty())
            continue;

        bool is_held = false;
        bool is_pressed = false;
        bool is_released = false;

        for (auto &key : CurrentInputMap->Keys)
        {
            is_held = pge->GetKey(key).bHeld;
            is_pressed = pge->GetKey(key).bPressed;
            is_released = pge->GetKey(key).bReleased;

            if (is_held || is_pressed || is_released) break;
        }

        CurrentInputMap->IsHeld     = is_held;
        CurrentInputMap->IsPressed  = is_pressed;
        CurrentInputMap->IsReleased = is_released;
    }
}

#endif
#endif