#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#include <SFML/Graphics/Font.hpp>
#include <GGPOngApplication.h>
#include <Globals.h>

int main( int _argc, const char* _argv[] )
{
	// Load font from file
	if (!font.loadFromFile("./data/UbuntuMono-R.ttf"))
	{
		printf("ERROR: failed to load font...\n");
	}

	// Enable windows sockets or whatever
	WSADATA wd = { 0 };
	WSAStartup(MAKEWORD(2, 2), &wd);

	// Init
	GGPOngApplication application1, application2;
	application1.init("player1", 10, 10);
	application2.init("player2", 10 + gameSize.x + 10, 10);

	// Update
	while (!application1.isExitRequested() && !application2.isExitRequested()) {
		application1.update();
		application2.update();

		application1.draw();
		application2.draw();
	}

	// Shutdown
	application2.shutdown();
	application1.shutdown();

	return EXIT_SUCCESS;
}
