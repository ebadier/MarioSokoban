#include "macros.h"
#include "sdlMarioSokoban.h"

int main(int argc, char* argv[])
{
	try
	{
		sdl::MarioSokoban game;
		game.Init("../../data/config.xml");
		game.Menu();
	}
	catch(std::exception& e)
	{
		MOutAndPopup(e.what());
	}
	catch(...)
	{
		MOutAndPopup("An unknown exception has occured !");
	}

	return EXIT_SUCCESS;
}