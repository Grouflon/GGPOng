#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <algorithm>
#include <assert.h>
#include <winsock.h>

#include <imgui-SFML.h>
#include <imgui.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <ggponet.h>

#include <Globals.h>
#include "SFML/Window/Mouse.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Text.hpp"

sf::String s_status;

bool __cdecl
ggpong_begin_game_callback(const char *)
{
	printf("begin game\n");

	return true;
}

/*
 * vw_on_event_callback --
 *
 * Notification from GGPO that something has happened.  Update the status
 * text at the bottom of the screen to notify the user.
 */
bool __cdecl
ggpong_on_event_callback(GGPOEvent* _info)
{
	printf("ggpo event: %d\n", _info->code);
	int progress;
	switch (_info->code) {
	case GGPO_EVENTCODE_CONNECTED_TO_PEER:
		s_status = "Synchronizing...";
		break;
	case GGPO_EVENTCODE_SYNCHRONIZING_WITH_PEER:
		char buf[256];
		sprintf(buf, "Synchronizing... (%.1f)", 100.f * float(_info->u.synchronizing.count / _info->u.synchronizing.total));
		s_status = buf;
		break;
	case GGPO_EVENTCODE_SYNCHRONIZED_WITH_PEER:
		s_status = "Synchronized !";
		break;
	case GGPO_EVENTCODE_RUNNING:
		s_status = "";
		break;
	case GGPO_EVENTCODE_CONNECTION_INTERRUPTED:
		/*ngs.SetDisconnectTimeout(info->u.connection_interrupted.player,
			timeGetTime(),
			info->u.connection_interrupted.disconnect_timeout);*/
		break;
	case GGPO_EVENTCODE_CONNECTION_RESUMED:
		//ngs.SetConnectState(info->u.connection_resumed.player, Running);
		break;
	case GGPO_EVENTCODE_DISCONNECTED_FROM_PEER:
		//ngs.SetConnectState(info->u.disconnected.player, Disconnected);
		break;
	case GGPO_EVENTCODE_TIMESYNC:
		//Sleep(1000 * info->u.timesync.frames_ahead / 60);
		break;
	}
	return true;
}


/*
 * vw_advance_frame_callback --
 *
 * Notification from GGPO we should step foward exactly 1 frame
 * during a rollback.
 */
bool __cdecl
ggpong_advance_frame_callback(int)
{
	printf("advance frame\n");

	/*int inputs[MAX_SHIPS] = { 0 };
	int disconnect_flags;

	// Make sure we fetch new inputs from GGPO and use those to update
	// the game state instead of reading from the keyboard.
	ggpo_synchronize_input(ggpo, (void *)inputs, sizeof(int) * MAX_SHIPS, &disconnect_flags);
	VectorWar_AdvanceFrame(inputs, disconnect_flags);*/
	return true;
}

/*
 * vw_load_game_state_callback --
 *
 * Makes our current state match the state passed in by GGPO.
 */
bool __cdecl
ggpong_load_game_state_callback(unsigned char *buffer, int len)
{
	printf("load game state\n");

	//memcpy(&gs, buffer, len);
	return true;
}

/*
 * vw_save_game_state_callback --
 *
 * Save the current state to a buffer and return it to GGPO via the
 * buffer and len parameters.
 */
bool __cdecl
ggpong_save_game_state_callback(unsigned char **buffer, int *len, int *checksum, int)
{
	printf("save game state\n");

	/**len = sizeof(gs);
	*buffer = (unsigned char *)malloc(*len);
	if (!*buffer) {
		return false;
	}
	memcpy(*buffer, &gs, *len);
	*checksum = fletcher32_checksum((short *)*buffer, *len / 2);*/
	return true;
}

/*
 * vw_log_game_state --
 *
 * Log the gamestate.  Used by the synctest debugging tool.
 */
bool __cdecl
ggpong_log_game_state(char *filename, unsigned char *buffer, int)
{
	printf("log game state\n");

	/*FILE* fp = nullptr;
	fopen_s(&fp, filename, "w");
	if (fp) {
		GameState *gamestate = (GameState *)buffer;
		fprintf(fp, "GameState object.\n");
		fprintf(fp, "  bounds: %d,%d x %d,%d.\n", gamestate->_bounds.left, gamestate->_bounds.top,
			gamestate->_bounds.right, gamestate->_bounds.bottom);
		fprintf(fp, "  num_ships: %d.\n", gamestate->_num_ships);
		for (int i = 0; i < gamestate->_num_ships; i++) {
			Ship *ship = gamestate->_ships + i;
			fprintf(fp, "  ship %d position:  %.4f, %.4f\n", i, ship->position.x, ship->position.y);
			fprintf(fp, "  ship %d velocity:  %.4f, %.4f\n", i, ship->velocity.dx, ship->velocity.dy);
			fprintf(fp, "  ship %d radius:    %d.\n", i, ship->radius);
			fprintf(fp, "  ship %d heading:   %d.\n", i, ship->heading);
			fprintf(fp, "  ship %d health:    %d.\n", i, ship->health);
			fprintf(fp, "  ship %d speed:     %d.\n", i, ship->speed);
			fprintf(fp, "  ship %d cooldown:  %d.\n", i, ship->cooldown);
			fprintf(fp, "  ship %d score:     %d.\n", i, ship->score);
			for (int j = 0; j < MAX_BULLETS; j++) {
				Bullet *bullet = ship->bullets + j;
				fprintf(fp, "  ship %d bullet %d: %.2f %.2f -> %.2f %.2f.\n", i, j,
					bullet->position.x, bullet->position.y,
					bullet->velocity.dx, bullet->velocity.dy);
			}
		}
		fclose(fp);
	}*/
	return true;
}

/*
 * vw_free_buffer --
 *
 * Free a save state buffer previously returned in vw_save_game_state_callback.
 */
void __cdecl
ggpong_free_buffer(void *buffer)
{
	printf("free buffer\n");

	free(buffer);
}

enum ApplicationState
{
	AS_Menu,
	AS_Game,
};

ApplicationState as = AS_Menu;
GGPOSession* ggpo = nullptr;

struct IP
{
	char address[32] = { '1', '2', '7', '.', '0', '.', '0', '.', '1', 0 };
	unsigned short port = 7500;
};

IP localIP;
IP distantIP;

struct PlayerState
{
	GGPOPlayer player = {};
	GGPOPlayerHandle handle = {};
};
PlayerState playerStates[GGPO_MAX_SPECTATORS + GGPO_MAX_PLAYERS];

void SetState(ApplicationState _state)
{
	if (_state == as)
		return;

	// EXIT STATE
	switch (as)
	{
	case AS_Menu:
		break;
	case AS_Game:
	{
		assert(ggpo);
		ggpo_close_session(ggpo);
		ggpo = nullptr;
	}
		break;
	default:
		break;
	}

	as = _state;

	// ENTER STATE
	switch (as)
	{
	case AS_Menu:
		s_status = "";
		break;
	case AS_Game:
	{
		GGPOSessionCallbacks cb = { 0 };
		cb.begin_game = ggpong_begin_game_callback;
		cb.advance_frame = ggpong_advance_frame_callback;
		cb.load_game_state = ggpong_load_game_state_callback;
		cb.save_game_state = ggpong_save_game_state_callback;
		cb.free_buffer = ggpong_free_buffer;
		cb.on_event = ggpong_on_event_callback;
		cb.log_game_state = ggpong_log_game_state;

		assert(ggpo == nullptr);
		GGPOErrorCode result = ggpo_start_session(&ggpo, &cb, "GGPOng", 2, sizeof(int), localIP.port);
		assert(GGPO_SUCCEEDED(result));

		// automatically disconnect clients after 3000 ms and start our count-down timer
		// for disconnects after 1000 ms.   To completely disable disconnects, simply use
		// a value of 0 for ggpo_set_disconnect_timeout.
		ggpo_set_disconnect_timeout(ggpo, 3000);
		ggpo_set_disconnect_notify_start(ggpo, 1000);

		for (int i = 0; i < 2; i++)
		{
			result = ggpo_add_player(ggpo, &playerStates[i].player, &playerStates[i].handle);
			if (playerStates[i].player.type == GGPO_PLAYERTYPE_LOCAL)
			{
				ggpo_set_frame_delay(ggpo, playerStates[i].handle, 2);
			}
		}

		gs.reset();

		s_status = "Waiting for peers...";
	}
		break;
	default:
		break;
	}
}

void ImGui_IpAddress(const char* _ipName, const char* _portName, IP* _ipAddr, bool _isIpEditable)
{
	ImGui::PushItemWidth(100.f);
	if (_isIpEditable)
	{
		ImGui::InputText(_ipName, _ipAddr->address, 32);
	}
	else
	{
		ImGui::Text(_ipAddr->address);
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::PushItemWidth(50.f);
	char portBuf[6] = {};
	sprintf(portBuf, "%d", _ipAddr->port);
	if (ImGui::InputText(_portName, portBuf, 6))
	{
		_ipAddr->port = atoi(portBuf);
	}
	ImGui::PopItemWidth();
}


int main( int _argc, const char* _argv[] )
{
	unsigned int frameCount = 0u;

	// Declare a new font
	sf::Font font;
	// Load it from a file
	if (!font.loadFromFile("./data/UbuntuMono-R.ttf"))
	{
		printf("ERROR: failed to load font...\n");
	}

	localIP.port = 7002;
	distantIP.port = 7003;

	// Enable windows sockets or whatever
	WSADATA wd = { 0 };
	WSAStartup(MAKEWORD(2, 2), &wd);

	sf::RenderWindow window(sf::VideoMode(int(gameSize.x), int(gameSize.y)), "GGPOng");

	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);
	ImGui::SFML::Init(window);

	gs.reset();

	sf::Clock deltaClock;
	while (window.isOpen()) {
		// INPUT
		sf::Event event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);

			if (as == AS_Game)
			{
				if (event.type == sf::Event::KeyPressed)
				{
					if (event.key.code == sf::Keyboard::Key::Up) { gs.p1.upPressed = true; }
					if (event.key.code == sf::Keyboard::Key::Down) { gs.p1.downPressed = true; }

					if (event.key.code == sf::Keyboard::Key::E) { gs.p2.upPressed = true; }
					if (event.key.code == sf::Keyboard::Key::D) { gs.p2.downPressed = true; }
				}

				if (event.type == sf::Event::KeyReleased)
				{
					if (event.key.code == sf::Keyboard::Key::Up) { gs.p1.upPressed = false; }
					if (event.key.code == sf::Keyboard::Key::Down) { gs.p1.downPressed = false; }

					if (event.key.code == sf::Keyboard::Key::E) { gs.p2.upPressed = false; }
					if (event.key.code == sf::Keyboard::Key::D) { gs.p2.downPressed = false; }
				}
			}

			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Escape)) {
				window.close();
			}
		}

		// UPDATE
		sf::Time dt = deltaClock.restart();
		ImGui::SFML::Update(window, dt);

		switch (as)
		{
		case AS_Menu:

			ImGui::SetNextWindowPos(ImVec2(100, 100));

			if (ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove))
			{
				ImGui_IpAddress("local IP", "local Port", &localIP, false);
				ImGui_IpAddress("distant IP", "distant Port", &distantIP, true);
				ImGui::Separator();
				if (ImGui::Button("Host Online Game"))
				{
					playerStates[0].player.size = sizeof(GGPOPlayer);
					playerStates[0].player.type = GGPO_PLAYERTYPE_LOCAL;
					playerStates[0].player.player_num = 1;
					playerStates[1].player.size = sizeof(GGPOPlayer);
					playerStates[1].player.type = GGPO_PLAYERTYPE_REMOTE;
					playerStates[1].player.player_num = 2;
					strcpy(playerStates[1].player.u.remote.ip_address, distantIP.address);
					playerStates[1].player.u.remote.port = distantIP.port;
					SetState(AS_Game);
				}
				ImGui::SameLine();
				if (ImGui::Button("Join Online Game"))
				{
					playerStates[1].player.type = GGPO_PLAYERTYPE_LOCAL;
					playerStates[1].player.player_num = 2;
					playerStates[0].player.type = GGPO_PLAYERTYPE_REMOTE;
					playerStates[0].player.player_num = 1;
					strcpy(playerStates[0].player.u.remote.ip_address, distantIP.address);
					playerStates[0].player.u.remote.port = distantIP.port;
					SetState(AS_Game);
				}
			}
			ImGui::End();
			break;
		case AS_Game:
			ggpo_idle(ggpo, 0);

			gs.p1.update();
			gs.p2.update();
			gs.ball.update(gs);

			++frameCount;
			break;
		default:
			break;
		}

		// DRAW
		window.clear();

		gs.p1.draw(window);
		gs.p2.draw(window);
		gs.ball.draw(window);

		char buf[256];
		sprintf(buf, "frame %d\n%.1f fps", frameCount, 1.f / dt.asSeconds());
		sf::Text fpsText(buf, font, 12);
		fpsText.setFillColor(sf::Color(255,255,255,127));
		fpsText.setPosition(sf::Vector2f(gameSize.x - 80.f, 20.f));
		window.draw(fpsText);

		sf::Text statusText(s_status, font, 12);
		statusText.setFillColor(sf::Color(255, 255, 255, 127));
		statusText.setPosition(sf::Vector2f(20.f, gameSize.y - 40.f));
		window.draw(statusText);

		ImGui::SFML::Render(window);
		window.display();
	}

	SetState(AS_Menu);

	ImGui::SFML::Shutdown();

	return EXIT_SUCCESS;
}
