#define VSYNC_ON 1
#define TWO_INSTANCES 0

#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/Clock.hpp>

#include <Gameplay/Globals.h>
#include <GGPOngApplication.h>

int main( int _argc, const char* _argv[] )
{
	// Load fonts from files
	if (!textFont.loadFromFile("./data/UbuntuMono-R.ttf"))
	{
		printf("ERROR: failed to load font...\n");
	}
	if (!titleFont.loadFromFile("./data/slkscr.ttf"))
	{
		printf("ERROR: failed to load font...\n");
	}

	// Enable windows sockets or whatever
	WSADATA wd = { 0 };
	WSAStartup(MAKEWORD(2, 2), &wd);

	// Init
	const char* windowName = "GGPOng";
#if TWO_INSTANCES
	windowName = "player1";
#endif

	GGPOngApplication application1;
	application1.localIP.port = 7000;
	application1.distantIP.port = 7001;
	application1.init(windowName, 0, 0, bool(VSYNC_ON));

#if TWO_INSTANCES
	GGPOngApplication application2;
	application2.localIP.port = 7001;
	application2.distantIP.port = 7000;
	application2.init("player2", 0, 30 + int(gameSize.y), bool(VSYNC_ON));
#endif


	// Update
	sf::Clock clock;
#if TWO_INSTANCES
	while (!application1.isExitRequested() && !application2.isExitRequested())
#else
	while (!application1.isExitRequested())
#endif
	{
#if VSYNC_ON
		application1.update();
		application1.draw();

	#if TWO_INSTANCES
		application2.update();
		application2.draw();
	#endif
#else
		if (clock.getElapsedTime().asMicroseconds() > 1000000 / 60)
		{
			clock.restart();
			application1.update();
			application1.draw();

	#if TWO_INSTANCES
			application2.update();
			application2.draw();
	#endif
		}
		else
		{
			application1.idle();
	#if TWO_INSTANCES
			application2.idle();
	#endif
		}
#endif
	}

	// Shutdown
#if TWO_INSTANCES
	application2.shutdown();
#endif
	application1.shutdown();

	return EXIT_SUCCESS;
}
