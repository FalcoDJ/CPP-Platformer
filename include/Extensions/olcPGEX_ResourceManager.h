#pragma once

#pragma region readme_region
/*
    olcPGEX_InputMap.h

    +-------------------------------------------------------------+
	|         OneLoneCoder Pixel Game Engine Extension            |
	|                  ResourceManager v1.0.0                     |
	+-------------------------------------------------------------+

    What is this?
	~~~~~~~~~~~~~
	This extension provides you the ability to test weather or not
    a PNG file has been loaded and get a Sprite from the PNG you requested.
    This prevent's loading the same file multiple times and taking up more 
    resources.

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
#pragma region example_region
// 0---------------------------------------------------0
// | Example:                                          |
// 0---------------------------------------------------0
/*
 Where you have define OLC_PGE_APPLICATION:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    #define OLC_PGEX_RESOURCE_MANAGER
    #include "olcPGEX_ResourceManager.h"


 Anywhere else in your code:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    olc::Decal *Decal = new olc::Decal(olc::ResourceManager::GetSprite("path_to_sprite"));

*/
#pragma endregion

#ifndef OLC_PGEX_RESOURCE_MANAGER_HPP
#define OLC_PGEX_RESOURCE_MANAGER_HPP

#include "olcPixelGameEngine.h"
#include <map>

namespace olc
{
    class ResourceManager : public olc::PGEX
    {
    public: // Public Functions
        ResourceManager() {}
        ~ResourceManager() {}

        static olc::Sprite *GetSprite(std::string pathToSpritePNG);

    private: // Private Functions
        static ResourceManager &rGet()
        {
            static ResourceManager RM;
            return RM;
        }

    private: // Private Variables
        std::map<std::string, olc::Sprite*> mSpriteMap; // has file paths and sprites
    };
};

#ifdef OLC_PGEX_RESOURCE_MANAGER
#undef OLC_PGEX_RESOURCE_MANAGER

olc::Sprite *olc::ResourceManager::GetSprite(std::string pathToSpritePNG)
{
    auto &sprite_map = rGet().mSpriteMap;

    auto key_value_pair = sprite_map.find(pathToSpritePNG);

    if (key_value_pair != sprite_map.end())
    {
        return key_value_pair->second;
    }
    else
    {
        sprite_map[pathToSpritePNG] = new olc::Sprite(pathToSpritePNG);
        return sprite_map[pathToSpritePNG];
    }
}

#endif
#endif