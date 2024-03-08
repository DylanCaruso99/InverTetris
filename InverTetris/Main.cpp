#include "Game.h"

#include <Windows.h>


int main();

#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	return main();
}
#endif

int main()
{
	Game game;
	game.run();

	return 0;
}
