#include "GGPOngApplication.h"

#include <assert.h>
#include <imgui.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <Globals.h>
#include <imgui-SFML.h>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Sleep.hpp>

/*
 * Simple checksum function stolen from wikipedia:
 *
 *   http://en.wikipedia.org/wiki/Fletcher%27s_checksum
 */

int
fletcher32_checksum(short* data, size_t len)
{
	int sum1 = 0xffff, sum2 = 0xffff;

	while (len) {
		size_t tlen = len > 360 ? 360 : len;
		len -= tlen;
		do {
			sum1 += *data++;
			sum2 += sum1;
		} while (--tlen);
		sum1 = (sum1 & 0xffff) + (sum1 >> 16);
		sum2 = (sum2 & 0xffff) + (sum2 >> 16);
	}

	/* Second reduction step to reduce sums to 16 bits */
	sum1 = (sum1 & 0xffff) + (sum1 >> 16);
	sum2 = (sum2 & 0xffff) + (sum2 >> 16);
	return sum2 << 16 | sum1;
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

GGPOngApplication::GGPOngApplication()
{

}

void GGPOngApplication::init(const char* _name, int _windowX, int _windowY, bool _vsyncEnabled)
{
	assert(!m_window);

	m_name = _name;
	m_window = new sf::RenderWindow(sf::VideoMode(int(gameSize.x), int(gameSize.y)), _name, sf::Style::Titlebar|sf::Style::Close);
	m_window->setPosition(sf::Vector2i(_windowX, _windowY));

	if (_vsyncEnabled)
	{
		m_window->setFramerateLimit(60);
		m_window->setVerticalSyncEnabled(true);
	}

	ImGui::SFML::Init(m_imguiSFMLContext, *m_window, true);

	m_gs.init();
}

void GGPOngApplication::shutdown()
{
	_setApplicationState(AS_Menu);

	ImGui::SFML::Shutdown(m_imguiSFMLContext);

	delete m_window;
	m_window = nullptr;
}

void GGPOngApplication::idle()
{
	if (m_applicationState == AS_Game)
	{
		ggpo_idle(m_ggpo, 0);
	}
}

void GGPOngApplication::update()
{
	ImGui::SetCurrentContext(m_imguiSFMLContext.imguiContext);

	// INPUT
	sf::Event event;
	while (m_window->pollEvent(event)) {
		ImGui::SFML::ProcessEvent(m_imguiSFMLContext, event);

		if (m_applicationState == AS_Game)
		{
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Key::Up) { m_upPressed = true; }
				if (event.key.code == sf::Keyboard::Key::Down) { m_downPressed = true; }
			}

			if (event.type == sf::Event::KeyReleased)
			{
				if (event.key.code == sf::Keyboard::Key::Up) { m_upPressed = false; }
				if (event.key.code == sf::Keyboard::Key::Down) { m_downPressed = false; }
			}

			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Escape)
			{
				_setApplicationState(AS_Menu);
			}
		}

		if (event.type == sf::Event::Closed) {
			m_window->close();
		}
	}

	// UPDATE
	m_dt = m_clock.restart();
	ImGui::SFML::Update(m_imguiSFMLContext, *m_window, m_dt);

	switch (m_applicationState)
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
				m_playerStates[0].player.size = sizeof(GGPOPlayer);
				m_playerStates[0].player.type = GGPO_PLAYERTYPE_LOCAL;
				m_playerStates[0].player.player_num = 1;
				m_localPlayerId = 0;
				m_playerStates[1].player.size = sizeof(GGPOPlayer);
				m_playerStates[1].player.type = GGPO_PLAYERTYPE_REMOTE;
				m_playerStates[1].player.player_num = 2;
				strcpy(m_playerStates[1].player.u.remote.ip_address, distantIP.address);
				m_playerStates[1].player.u.remote.port = distantIP.port;
				_setApplicationState(AS_Game);
			}
			ImGui::SameLine();
			if (ImGui::Button("Join Online Game"))
			{
				m_playerStates[1].player.type = GGPO_PLAYERTYPE_LOCAL;
				m_playerStates[1].player.player_num = 2;
				m_localPlayerId = 1;
				m_playerStates[0].player.type = GGPO_PLAYERTYPE_REMOTE;
				m_playerStates[0].player.player_num = 1;
				strcpy(m_playerStates[0].player.u.remote.ip_address, distantIP.address);
				m_playerStates[0].player.u.remote.port = distantIP.port;
				_setApplicationState(AS_Game);
			}
		}
		ImGui::End();
		break;
	case AS_Game:
		ggpo_idle(m_ggpo, 0);

		if (m_connectionState == CS_Connected)
		{
			if (m_framesToSkip > 0)
			{
				--m_framesToSkip;
				return;
			}

			GGPOErrorCode result = GGPO_OK;
			int disconnect_flags;
			int inputs[2] = { 0 };

			if (m_playerStates[m_localPlayerId].handle != GGPO_INVALID_HANDLE) {
				int input = (m_upPressed << 0) | (m_downPressed << 1);
				result = ggpo_add_local_input(m_ggpo, m_playerStates[m_localPlayerId].handle, &input, sizeof(input));
			}

			// synchronize these inputs with ggpo.  If we have enough input to proceed
			// ggpo will modify the input list with the correct inputs to use and
			// return 1.
			if (GGPO_SUCCEEDED(result)) {
				result = ggpo_synchronize_input(m_ggpo, (void*)inputs, sizeof(int) * 2, &disconnect_flags);
				if (GGPO_SUCCEEDED(result)) {
					// inputs[0] and inputs[1] contain the inputs for p1 and p2.  Advance
					// the game by 1 frame using those inputs.

					_advanceFrame(inputs, disconnect_flags);
				}
			}
		}
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

	m_gs.draw(*m_window);

	char buf[256];
	sprintf(buf, "frame %d\n%.1f fps", m_gs.frameNumber, 1.f / m_dt.asSeconds());
	sf::Text fpsText(buf, textFont, 12);
	fpsText.setFillColor(sf::Color(255, 255, 255, 127));
	fpsText.setPosition(sf::Vector2f(gameSize.x - 80.f, 20.f));
	m_window->draw(fpsText);

	sf::Text statusText(m_status, textFont, 12);
	statusText.setFillColor(sf::Color(255, 255, 255, 127));
	statusText.setPosition(sf::Vector2f(20.f, gameSize.y - 40.f));
	m_window->draw(statusText);

	ImGui::SFML::Render(m_imguiSFMLContext, *m_window);
	m_window->display();

	ImGui::SetCurrentContext(nullptr);
}

void GGPOngApplication::_setStatus(const char* _status)
{
	m_status = _status;
}

bool GGPOngApplication::isExitRequested() const
{
	return m_window && !m_window->isOpen();
}

void GGPOngApplication::_setApplicationState(ApplicationState _state)
{
	if (_state == m_applicationState)
		return;

	// EXIT STATE
	switch (m_applicationState)
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

	m_applicationState = _state;

	// ENTER STATE
	switch (m_applicationState)
	{
	case AS_Menu:
		_setConnectionState(CS_None);
		_setStatus("");
		break;
	case AS_Game:
	{
		GGPOSessionCallbacks cb = { 0 };
		cb.begin_game = &GGPOngApplication::begin_game_callback;
		cb.advance_frame = &GGPOngApplication::advance_frame_callback;
		cb.load_game_state = &GGPOngApplication::load_game_state_callback;
		cb.save_game_state = &GGPOngApplication::save_game_state_callback;
		cb.free_buffer = &GGPOngApplication::free_buffer;
		cb.on_event = &GGPOngApplication::on_event_callback;
		cb.log_game_state = &GGPOngApplication::log_game_state;
		cb.userdata = this;

		assert(m_ggpo == nullptr);
		//GGPOErrorCode result = ggpo_start_synctest(&m_ggpo, &cb, "GGPOng", 2, sizeof(int), 1);
		GGPOErrorCode result = ggpo_start_session(&m_ggpo, &cb, "GGPOng", 2, sizeof(int), localIP.port);
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

		m_gs.init();

		_setConnectionState(CS_Connecting);
	}
	break;
	default:
		break;
	}
}

void GGPOngApplication::_setConnectionState(ConnectionState _state)
{
	if (_state == m_connectionState)
		return;

	// EXIT STATE
	switch (m_connectionState)
	{
	case CS_Disconnected:
		break;
	case CS_Connecting:
		break;
	case CS_Connected:
		break;
	default:
		break;
	}

	m_connectionState = _state;

	// ENTER STATE
	switch (m_connectionState)
	{
	case CS_None:
		break;
	case CS_Disconnected:
		_setStatus("Disconnected from peer.");
		break;
	case CS_Connecting:
		_setStatus("Waiting for peers...");
		break;
	case CS_Connected:
		break;
	case CS_Interrupted:
		_setStatus("Connection with peer interrupted...");
		break;
	default:
		break;
	}
}

void GGPOngApplication::_advanceFrame(int _inputs[2], int _disconnectFlags)
{
	m_gs.update(_inputs);

	ggpo_advance_frame(m_ggpo);
}



// GGPO CALLBACKS

bool GGPOngApplication::begin_game_callback(const char*, void* _userdata)
{
	printf("begin game\n");
	return true;
}

/*
 * on_event_callback --
 *
 * Notification from GGPO that something has happened.  Update the status
 * text at the bottom of the screen to notify the user.
 */
bool GGPOngApplication::on_event_callback(GGPOEvent* _info, void* _userdata)
{
	GGPOngApplication* application = reinterpret_cast<GGPOngApplication*>(_userdata);

	const char* eventString = "";
	switch (_info->code)
	{
	case GGPO_EVENTCODE_CONNECTED_TO_PEER: eventString = "CONNECTED_TO_PEER"; break;
	case GGPO_EVENTCODE_SYNCHRONIZING_WITH_PEER: eventString = "SYNCHRONIZING_WITH_PEER"; break;
	case GGPO_EVENTCODE_SYNCHRONIZED_WITH_PEER: eventString = "SYNCHRONIZED_WITH_PEER"; break;
	case GGPO_EVENTCODE_RUNNING: eventString = "RUNNING"; break;
	case GGPO_EVENTCODE_DISCONNECTED_FROM_PEER: eventString = "DISCONNECTED_FROM_PEER"; break;
	case GGPO_EVENTCODE_TIMESYNC: eventString = "TIMESYNC"; break;
	case GGPO_EVENTCODE_CONNECTION_INTERRUPTED: eventString = "CONNECTION_INTERRUPTED"; break;
	case GGPO_EVENTCODE_CONNECTION_RESUMED: eventString = "CONNECTION_RESUMED"; break;
	default: break;
	}
	printf("%s: ggpo event: %s(%d)\n", application->m_name, eventString, _info->code);

	switch (_info->code) {
	case GGPO_EVENTCODE_CONNECTED_TO_PEER:
		application->_setStatus("Synchronizing...");
		break;
	case GGPO_EVENTCODE_SYNCHRONIZING_WITH_PEER:
		char buf[256];
		sprintf(buf, "Synchronizing... (%.1f)", 100.f * float(_info->u.synchronizing.count / _info->u.synchronizing.total));
		application->_setStatus(buf);
		break;
	case GGPO_EVENTCODE_SYNCHRONIZED_WITH_PEER:
		application->_setStatus("Synchronized !");
		break;
	case GGPO_EVENTCODE_RUNNING:
		application->_setConnectionState(CS_Connected);
		application->_setStatus("");
		break;
	case GGPO_EVENTCODE_CONNECTION_INTERRUPTED:
		assert(application->m_connectionState == CS_Connected);
		application->_setConnectionState(CS_Interrupted);
		break;
	case GGPO_EVENTCODE_CONNECTION_RESUMED:
		assert(application->m_connectionState == CS_Interrupted);
		application->_setConnectionState(CS_Connected);
		break;
	case GGPO_EVENTCODE_DISCONNECTED_FROM_PEER:
		application->_setConnectionState(CS_Disconnected);
		break;
	case GGPO_EVENTCODE_TIMESYNC:
		application->m_framesToSkip = _info->u.timesync.frames_ahead;
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
bool GGPOngApplication::advance_frame_callback(int, void* _userdata)
{
	GGPOngApplication* application = reinterpret_cast<GGPOngApplication*>(_userdata);
	printf("%s:%d: advance frame\n", application->m_name, application->m_gs.frameNumber);

	int inputs[2] = { 0 };
	int disconnect_flags;

	// Make sure we fetch new inputs from GGPO and use those to update
	// the game state instead of reading from the keyboard.
	ggpo_synchronize_input(application->m_ggpo, (void*)inputs, sizeof(int) * 2, &disconnect_flags);
	application->_advanceFrame(inputs, disconnect_flags);
	return true;
}

/*
 * vw_load_game_state_callback --
 *
 * Makes our current state match the state passed in by GGPO.
 */
bool GGPOngApplication::load_game_state_callback(unsigned char* buffer, int len, void* _userdata)
{
	GGPOngApplication* application = reinterpret_cast<GGPOngApplication*>(_userdata);

	memcpy(&application->m_gs, buffer, len);
	return true;
}

/*
 * vw_save_game_state_callback --
 *
 * Save the current state to a buffer and return it to GGPO via the
 * buffer and len parameters.
 */
bool GGPOngApplication::save_game_state_callback(unsigned char** buffer, int* len, int* checksum, int, void* _userdata)
{
	GGPOngApplication* application = reinterpret_cast<GGPOngApplication*>(_userdata);

	*len = sizeof(application->m_gs);
	*buffer = (unsigned char*)malloc(*len);
	if (!*buffer) {
		return false;
	}
	memcpy(*buffer, &application->m_gs, *len);
	*checksum = fletcher32_checksum((short*)*buffer, *len / 2);
	return true;
}

/*
 * vw_log_game_state --
 *
 * Log the gamestate.  Used by the synctest debugging tool.
 */
bool GGPOngApplication::log_game_state(char* filename, unsigned char* buffer, int, void* _userdata)
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
void GGPOngApplication::free_buffer(void* buffer, void* _userdata)
{
	free(buffer);
}
