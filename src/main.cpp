#include "Engine.hpp"

int main(int argc, char const *argv[])
{
    Engine e;
    if (e.Construct(e.CanvasSize.x, e.CanvasSize.y, e.PixelScale, e.PixelScale, false, true))
        e.Start();
    
    return 0;
}
