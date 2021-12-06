#ifndef files_funcs
#define files_funcs

#include <string>
#include <vector>
#include "olcPixelGameEngine.h"
#include "DebugController.hpp"

class file
{
public:
    static bool doesExist(const std::string &sPath)
    {
        bool success = true;

        if (!_gfs::exists(sPath))
        {
            #ifdef DEBUG_FILE_SYSTEM
                std::cout << '"' + sPath + '"' + " Doesn't exist" << std::endl;
            #endif
            success = false;
        }
        else
        {
            #ifdef DEBUG_FILE_SYSTEM
                std::cout << "found: " + sPath << std::endl;
            #endif
        }

        #ifdef DEBUG_FILE_SYSTEM
            std::cout << std::endl;
        #endif

        return success;
    }

    static bool batchDoesExist(const std::vector<std::string> &sPaths)
    {
        bool success = true;

        for (const auto &file : sPaths)
        {
            if (!_gfs::exists(file))
            {
                #ifdef DEBUG_FILE_SYSTEM
                std::cout << '"' + file + '"' + " Doesn't exist" << std::endl;
                #endif
                success = false;
            }
            else
            {
                #ifdef DEBUG_FILE_SYSTEM
                std::cout << "found: " + file << std::endl;
                #endif
            }
        }

        #ifdef DEBUG_FILE_SYSTEM
            std::cout << std::endl;
        #endif

        return success;
    }
};

#endif
