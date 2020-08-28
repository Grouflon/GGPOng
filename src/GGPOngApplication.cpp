#include "GGPOngApplication.h"

#include <assert.h>
#include <imgui.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <Globals.h>
#include <imgui-SFML.h>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Text.hpp>

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

bool __cdecl
ggpong_begin_game_callback(const char*, void* _userdata)
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
ggpong_on_event_callback(GGPOEvent* _info, void* _userdata)
{
	GGPOngApplication* application = reinterpret_cast<GGPOngApplication*>(_userdata);
	printf("ggpo event: %d\n", _info->code);
	switch (_info->code) {
	case GGPO_EVENTCODE_CONNECTED_TO_PEER:
		application->setStatus("Synchronizing...");
		break;
	case GGPO_EVENTCODE_SYNCHRONIZING_WITH_PEER:
		char buf[256];
		sprintf(buf, "Synchronizing... (%.1f)", 100.f * float(_info->u.synchronizing.count / _info->u.synchronizing.total));
		application->setStatus(buf);
		break;
	case GGPO_EVENTCODE_SYNCHRONIZED_WITH_PEER:
		application->setStatus("Synchronized !");
		break;
	case GGPO_EVENTCODE_RUNNING:
		application->setStatus("");
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
ggpong_advance_frame_callback(int, void* _userdata)
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
ggpong_load_game_state_callback(unsigned char* buffer, int len, void* _userdata)
{
	printf("load game state\n");

	//memcpy(&m_gs, buffer, len);
	return true;
}

/*
 * vw_save_game_state_callback --
 *
 * Save the current state to a buffer and return it to GGPO via the
 * buffer and len parameters.
 */
bool __cdecl
ggpong_save_game_state_callback(unsigned char** buffer, int* len, int* checksum, int, void* _userdata)
{
	printf("save game state\n");

	/**len = sizeof(m_gs);
	*buffer = (unsigned char *)malloc(*len);
	if (!*buffer) {
		return false;
	}
	memcpy(*buffer, &m_gs, *len);
	*checksum = fletcher32_checksum((short *)*buffer, *len / 2);*/
	return true;
}

/*
 * vw_log_game_state --
 *
 * Log the gamestate.  Used by the synctest debugging tool.
 */
bool __cdecl
ggpong_log_game_state(char* filename, unsigned char* buffer, int, void* _userdata)
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
ggpong_free_buffer(void* buffer, void* _userdata)
{
	printf("free buffer\n");

	free(buffer);
}

GGPOngApplication::GGPOngApplication()
{

}

void GGPOngApplication::init(const char* _name, int _windowX, int _windowY)
{
	assert(!m_window);

	m_window = new sf::RenderWindow(sf::VideoMode(int(gameSize.x), int(gameSize.y)), _name);
	m_window->setPosition(sf::Vector2i(_windowX, _windowY));

	m_window->setFramerateLimit(60);
	m_window->setVerticalSyncEnabled(true);

	ImGui::SFML::Init(m_imguiSFMLContext, *m_window, true);

	m_gs.reset();
}

void GGPOngApplication::shutdown()
{
	_setState(AS_Menu);

	ImGui::SFML::Shutdown(m_imguiSFMLContext);

	delete m_window;
	m_window = nullptr;
}

void GGPOngApplication::update()
{
	ImGui::SetCurrentContext(m_imguiSFMLContext.imguiContext);

	// INPUT
	sf::Event event;
	while (m_window->pollEvent(event)) {
		ImGui::SFML::ProcessEvent(m_imguiSFMLContext, event);

		if (m_state == AS_Game)
		{
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Key::Up) { m_gs.p1.upPressed = true; }
				if (event.key.code == sf::Keyboard::Key::Down) { m_gs.p1.downPressed = true; }

				if (event.key.code == sf::Keyboard::Key::E) { m_gs.p2.upPressed = true; }
				if (event.key.code == sf::Keyboard::Key::D) { m_gs.p2.downPressed = true; }
			}

			if (event.type == sf::Event::KeyReleased)
			{
				if (event.key.code == sf::Keyboard::Key::Up) { m_gs.p1.upPressed = false; }
				if (event.key.code == sf::Keyboard::Key::Down) { m_gs.p1.downPressed = false; }

				if (event.key.code == sf::Keyboard::Key::E) { m_gs.p2.upPressed = false; }
				if (event.key.code == sf::Keyboard::Key::D) { m_gs.p2.downPressed = false; }
			}
		}

		if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Escape)) {
			m_window->close();
		}
	}

	// UPDATE
	m_dt = m_clock.restart();
	ImGui::SFML::Update(m_imguiSFMLContext, *m_window, m_dt);

	switch (m_state)
	{
	case AS_Menu:

		ImGui::SetNextWindowPos(ImVec2(100, 100));

		if (ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove))
		{
			ImGui_IpAddress("local IP", "local Port", &m_localIP, false);
			ImGui_IpAddress("distant IP", "distant Port", &m_distantIP, true);
			ImGui::Separator();
			if (ImGui::Button("Host Online Game"))
			{
				m_playerStates[0].player.size = sizeof(GGPOPlayer);
				m_playerStates[0].player.type = GGPO_PLAYERTYPE_LOCAL;
				m_playerStates[0].player.player_num = 1;
				m_playerStates[1].player.size = sizeof(GGPOPlayer);
				m_playerStates[1].player.type = GGPO_PLAYERTYPE_REMOTE;
				m_playerStates[1].player.player_num = 2;
				strcpy(m_playerStates[1].player.u.remote.ip_address, m_distantIP.address);
				m_playerStates[1].player.u.remote.port = m_distantIP.port;
				_setState(AS_Game);
			}
			ImGui::SameLine();
			if (ImGui::Button("Join Online Game"))
			{
				m_playerStates[1].player.type = GGPO_PLAYERTYPE_LOCAL;
				m_playerStates[1].player.player_num = 2;
				m_playerStates[0].player.type = GGPO_PLAYERTYPE_REMOTE;
				m_playerStates[0].player.player_num = 1;
				strcpy(m_playerStates[0].player.u.remote.ip_address, m_distantIP.address);
				m_playerStates[0].player.u.remote.port = m_distantIP.port;
				_setState(AS_Game);
			}
		}
		ImGui::End();
		break;
	case AS_Game:
		ggpo_idle(m_ggpo, 0);

		m_gs.p1.update();
		m_gs.p2.update();
		m_gs.ball.update(m_gs);

		++m_frameCount;
		break;
	default:
		break;
	}

	ImGui::SetCurrentContext(nullptr);
}

void GGPOngApplication::draw()
{
	ImGui::SetCurrentContext(m_imguiSFMLContext.imguiContext);

	m_window->clear();

	m_gs.p1.draw(*m_window);
	m_gs.p2.draw(*m_window);
	m_gs.ball.draw(*m_window);

	char buf[256];
	sprintf(buf, "frame %d\n%.1f fps", m_frameCount, 1.f / m_dt.asSeconds());
	sf::Text fpsText(buf, font, 12);
	fpsText.setFillColor(sf::Color(255, 255, 255, 127));
	fpsText.setPosition(sf::Vector2f(gameSize.x - 80.f, 20.f));
	m_window->draw(fpsText);

	sf::Text statusText(m_status, font, 12);
	statusText.setFillColor(sf::Color(255, 255, 255, 127));
	statusText.setPosition(sf::Vector2f(20.f, gameSize.y - 40.f));
	m_window->draw(statusText);

	ImGui::SFML::Render(m_imguiSFMLContext, *m_window);
	m_window->display();

	ImGui::SetCurrentContext(nullptr);
}

void GGPOngApplication::setStatus(const char* _status)
{
	m_status = _status;
}

bool GGPOngApplication::isExitRequested() const
{
	return m_window && !m_window->isOpen();
}

void GGPOngApplication::_setState(ApplicationState _state)
{
	if (_state == m_state)
		return;

	// EXIT STATE
	switch (m_state)
	{
	case AS_Menu:
		break;
	case AS_Game:
	{
		assert(m_ggpo);
		ggpo_close_session(m_ggpo);
		m_ggpo = nullptr;
	}
	break;
	default:
		break;
	}

	m_state = _state;

	// ENTER STATE
	switch (m_state)
	{
	case AS_Menu:
		m_status = "";
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
		cb.userdata = this;

		assert(m_ggpo == nullptr);
		GGPOErrorCode result = ggpo_start_session(&m_ggpo, &cb, "GGPOng", 2, sizeof(int), m_localIP.port);
		assert(GGPO_SUCCEEDED(result));

		// automatically disconnect clients after 3000 ms and start our count-down timer
		// for disconnects after 1000 ms.   To completely disable disconnects, simply use
		// a value of 0 for ggpo_set_disconnect_timeout.
		ggpo_set_disconnect_timeout(m_ggpo, 3000);
		ggpo_set_disconnect_notify_start(m_ggpo, 1000);

		for (int i = 0; i < 2; i++)
		{
			result = ggpo_add_player(m_ggpo, &m_playerStates[i].player, &m_playerStates[i].handle);
			if (m_playerStates[i].player.type == GGPO_PLAYERTYPE_LOCAL)
			{
				ggpo_set_frame_delay(m_ggpo, m_playerStates[i].handle, 2);
			}
		}

		m_gs.reset();

		m_status = "Waiting for peers...";
	}
	break;
	default:
		break;
	}
}
