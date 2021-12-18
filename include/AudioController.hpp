#pragma once

#include <Extensions/olcPGEX_AudioListener.h>

class AudioController
{
public:
    static olcPGEX_AudioListener *rGet()
    {
        static olcPGEX_AudioListener al;
        return &al;
    }
private:
    int samples;
};