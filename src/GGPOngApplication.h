#pragma once

#include <ggponet.h>
#include <GameplayState.h>
#include <SFML/System/String.hpp>
#include <SFML/System/Clock.hpp>
#include <imgui-SFML.h>

struct GGPOSession;
struct ImGuiContext;
namespace sf { class RenderWindow; }

struct IP
{
	char address[32] = { '1', '2', '7', '.', '0', '.', '0', '.', '1', 0 };
	unsigned short port = 7500;
};

class GGPOngApplication
{
public:
	GGPOngApplication();

	void init(const char* _name, int _windowX, int _windowY, bool _vsyncEnabled = true);
	void shutdown();

	void idle();
	void update();
	void draw();

	bool isExitRequested() const;

	IP localIP;
	IP distantIP;

private:

	struct PlayerState
	{
		GGPOPlayer player = {};
		GGPOPlayerHandle handle = {};
	};
	enum ApplicationState
	{
		AS_Menu,
		AS_Game,
	};
	enum ConnectionState
	{
		CS_None,
		CS_Connecting,
		CS_Connected,
		CS_Interrupted,
		CS_Disconnected,
	};

	void _setStatus(const char* _status);
	void _setApplicationState(ApplicationState _state);
	void _setConnectionState(ConnectionState _state);

	void _advanceFrame(int _inputs[2], int _disconnectFlags);
	
	static bool begin_game_callback(const char*, void* _userdata);
	static bool on_event_callback(GGPOEvent* _info, void* _userdata);
	static bool advance_frame_callback(int, void* _userdata);
	static bool load_game_state_callback(unsigned char* buffer, int len, void* _userdata);
	static bool save_game_state_callback(unsigned char** buffer, int* len, int* checksum, int, void* _userdata);
	static bool log_game_state(char* filename, unsigned char* buffer, int, void* _userdata);
	static void free_buffer(void* buffer, void* _userdata);

	sf::RenderWindow* m_window = nullptr;
	GGPOSession* m_ggpo = nullptr;
	ImGui::SFML::ImGuiSFMLContext m_imguiSFMLContext;
	ApplicationState m_applicationState = AS_Menu;
	ConnectionState m_connectionState = CS_Disconnected;

	int m_localPlayerId = -1;
	PlayerState m_playerStates[GGPO_MAX_SPECTATORS + GGPO_MAX_PLAYERS];
	GameplayState m_gs;

	sf::Clock m_clock;
	sf::Time m_dt;

	bool m_upPressed = false;
	bool m_downPressed = false;

	int m_framesToSkip = 0;

	sf::String m_status;
	const char* m_name;
};
