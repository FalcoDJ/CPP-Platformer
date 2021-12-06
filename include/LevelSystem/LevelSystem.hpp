#pragma once
#ifndef LEVEL_SYSTEM_HPP
#define LEVEL_SYSTEM_HPP

#include <assert.h>
#include <string>
#include <cstring>
#include "FileChecker.hpp"
#include "PugiXML/src/pugixml.hpp"
#include "PugiXML/src/pugiconfig.hpp"

#include "LevelSystem/Level.hpp"

class LevelSystem
{
public:  // Public Functions
    LevelSystem() {}
    ~LevelSystem() 
    {
        if (mActiveLevel != nullptr)
            delete mActiveLevel;
    }

    bool LoadLevel(std::string levelXmlPath)
    {
        // Check if level exists
        if (!file::doesExist(levelXmlPath))
            return false;

        // Reset the active level before we load a new one
        if (mActiveLevel != nullptr)
            delete mActiveLevel;

        // Create new level
        mActiveLevel = new Level();

        std::vector<ShapeSystem::sysRect> level_rectangles;

        // Load XML document
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(levelXmlPath.c_str());
        if (result)
        {
            pugi::xml_node level_node = doc.child("level");
            if (!level_node.empty())
            {
                for (pugi::xml_node background = level_node.child("background"); background; background = background.next_sibling("background"))
                {
                    mActiveLevel->AddBackground(background.attribute("path").value());
                }

                for (pugi::xml_node stage = level_node.child("stage"); stage; stage = stage.next_sibling("stage"))
                {
                    mActiveLevel->AddStage(stage.attribute("path").value());
                }

                for (pugi::xml_node rectangle = level_node.child("rectangle"); rectangle; rectangle = rectangle.next_sibling("rectangle"))
                {
                    ShapeSystem::sysRect temp_rectangle = std::make_shared<Rectangle>();
                    temp_rectangle->Size.x = atoi(rectangle.attribute("width").value());
                    temp_rectangle->Size.y = atoi(rectangle.attribute("height").value());
                    temp_rectangle->GlobalPosition.x = atoi(rectangle.attribute("x").value());
                    temp_rectangle->GlobalPosition.y = atoi(rectangle.attribute("y").value());
                    temp_rectangle->CollisionDirectionMode = Rectangle::CollisionDirection(atoi(rectangle.attribute("collision_direction").value()));

                    level_rectangles.push_back(temp_rectangle);
                }
            }
            
        }
        else
        {
            std::cout << "XML [" << levelXmlPath.c_str() << "] parsed with errors, attr value: [" << doc.child("level").attribute("name").value() << "]\n";
            std::cout << "Error description: " << result.description() << "\n";
            std::cout << "Error offset: " << result.offset << " (error at [..." << (levelXmlPath.c_str() + result.offset) << "]\n\n";
        }

        // Loop through document
        // Find rectangles
        // Find Backgrounds
        // Find Enemies
        // Add everything to mActiveLevel

        mActiveLevel->AddRectangles(level_rectangles);
        mActiveLevel->Initialize();

        return true;
    }

    void Update(const float &fElapsedTime)
    {
        return;
    }

    void Draw(Camera2d *cam2d)
    {
        if (cam2d == nullptr)
            return;
        
        mActiveLevel->Draw(cam2d);
    }

private: // Private Functions

public:  // Public Variables

private: // Private Variables
    Level *mActiveLevel = nullptr;
};

#endif