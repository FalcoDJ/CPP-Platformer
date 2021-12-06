// 0---------------------0
// |  Example: main.cpp  |
// 0---------------------0

// #define OLC_PGE_APPLICATION
// #include "olcPixelGameEngine.h"

// #include "olcPGEX_LayerController.h"

// // Override base class with your custom functionality
// class LC_Example : public olc::PixelGameEngine
// {
// public:
// 	LC_Example()
// 	{
// 		// Name your application
// 		sAppName = "LayerController";
// 	}
//     std::string ui_layer = "1";
//     std::string first_layer = "2";
//     std::string second_layer = "3";
//     std::string main_layer = "4a 65 73 75 73 4c 6f 76 65 73 59 6f 75";

// public:
// 	bool OnUserCreate() override
// 	{
//         // Layers should be created in order from top to bottom 
//         // (i.e., ui first then main then background)
// 		olc::LayerController::CreateLayer(ui_layer);
// 		olc::LayerController::CreateLayer(first_layer);
// 		olc::LayerController::CreateLayer(main_layer);
// 		olc::LayerController::CreateLayer(second_layer);
        
// 		return true;
// 	}

// 	bool OnUserUpdate(float fElapsedTime) override
// 	{
//         olc::LayerController::SetActiveLayer(main_layer);

//         olc::vf2d vMouse = GetMousePos();

//         Clear(olc::VERY_DARK_BLUE);

//         olc::LayerController::SetActiveLayer(ui_layer);
//         Clear(olc::BLANK);

//         DrawString({16,16}, std::to_string(GetFPS()), olc::YELLOW);

//         olc::LayerController::SetActiveLayer(main_layer);
//         Clear(olc::BLANK);

//         FillCircle(vMouse, 8, olc::WHITE);

//         olc::LayerController::SetActiveLayer(first_layer);
//         Clear(olc::BLANK);

//         FillRect({64, 0}, {128,256}, olc::VERY_DARK_GREEN);

//         olc::LayerController::SetActiveLayer(second_layer);
//         Clear(olc::BLANK);

//         FillRect({0,0},{256,256}, olc::VERY_DARK_BLUE);

// 		return true;
// 	}
// };

// int main(int argc, char const *argv[])
// {
//     LC_Example demo;
    
//     if (demo.Construct(256,256,2,2))
//         demo.Start();

//     return 0;
// }

#pragma once
#ifndef OLC_PGEX_LAYER_CONTROLLER_H
#define OLC_PGEX_LAYER_CONTROLLER_H

#include <iostream>
#include "olcPixelGameEngine.h"

namespace olc
{
    class LayerController : public olc::PGEX
    {
    private: // Private Functions
        LayerController() : olc::PGEX(true) {}

        static LayerController& rGet()
        {
            static LayerController instance;
            return instance;
        }

        void OnAfterUserCreate() override
        {
            rGet().pge->SetDrawTarget(nullptr);
            rGet().pge->SetPixelMode(olc::Pixel::ALPHA);
            rGet().pge->Clear(olc::BLANK);

            SetActiveLayerToDebugLayer();
            auto key_value_pair = m_LayerIdMap.find(m_CurrentLayer);
            rGet().pge->SetLayerTint(key_value_pair->second, olc::Pixel(255, 255, 255, 153));

            rGet().pge->SetDrawTarget(nullptr);
        }

        void OnAfterUserUpdate(float fElapsedTime) override
        {
            m_ClearedDebugLayer = false;
        }

    public: // Public Functions
        static void ClearCurrentLayer(olc::Pixel clearColor)
        {
            rGet().pge->Clear(clearColor);
        }
        static void CreateLayer(const std::string &layerName)
        {
            auto& m = rGet().m_LayerIdMap;
            m[layerName] = rGet().pge->CreateLayer();
            rGet().pge->EnableLayer(m[layerName], true);
            rGet().pge->SetDrawTarget(m[layerName]);
            rGet().pge->SetPixelMode(olc::Pixel::ALPHA);
            rGet().pge->Clear(olc::BLANK);
            rGet().pge->SetDrawTarget(nullptr);
        }
        static void SetActiveLayer(const std::string &layerName)
        {
            auto& m = rGet().m_LayerIdMap;

            auto key_value_pair = m.find(layerName);

            rGet().m_PreviousLayer = rGet().m_CurrentLayer;
            rGet().m_CurrentLayer = layerName;

            if (key_value_pair != m.end())
            {
                rGet().pge->SetDrawTarget(key_value_pair->second);
            }
            else
            {
                rGet().CreateLayer(layerName);
                rGet().pge->SetDrawTarget(key_value_pair->second);
            }
        }
        static void SetActiveLayerToDebugLayer()
        {
            SetActiveLayer(rGet().m_DebugLayer);
        }
        static void SafelyClearDebuglayer(olc::Pixel clearColor)
        {
            SetActiveLayerToDebugLayer();
            
            // Clear Debug Layer If It Hasn't Been Cleared This Frame!
            bool &has_been_cleared = rGet().m_ClearedDebugLayer;

            if (!has_been_cleared)
                ClearCurrentLayer(clearColor), has_been_cleared = true;
        }
        static std::string GetPreviousLayer()
        {
            return rGet().m_PreviousLayer;
        }
    
    private: // Private Variables
        std::map<std::string,int> m_LayerIdMap;
        std::string m_CurrentLayer;
        std::string m_PreviousLayer;
        std::string m_DebugLayer = "DEBUG_LAYER";
        bool m_ClearedDebugLayer = false;
    };
};

#endif