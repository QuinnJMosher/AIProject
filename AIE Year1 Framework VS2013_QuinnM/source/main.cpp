#include "AIE.h"
#include <iostream>

//constant vars
static int SCREEN_MAX_X = 900, SCREEN_MAX_Y = 600;

int main( int argc, char* argv[] )
{	
	Initialise(SCREEN_MAX_X, SCREEN_MAX_Y, false, "My Awesome Game");
    
    SetBackgroundColour(SColour(0, 0, 0, 255));

    //Game Loop
    do
	{
        ClearScreen();

    } while(!FrameworkUpdate());

    Shutdown();

    return 0;
}
