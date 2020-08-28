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

	void init(const char* _name, int _windowX, int _windowY);
	void shutdown();

	void update();
	void draw();

	void setStatus(const char* _status);

	bool isExitRequested() const;

private:
	enum ApplicationState
	{
		AS_Menu,
		AS_Game,
	};
	struct PlayerState
	{
		GGPOPlayer player = {};
		GGPOPlayerHandle handle = {};
	};

	void _setState(ApplicationState _state);

	sf::RenderWindow* m_window = nullptr;
	GGPOSession* m_ggpo = nullptr;
	ImGui::SFML::ImGuiSFMLContext m_imguiSFMLContext;
	ApplicationState m_state = AS_Menu;
	PlayerState m_playerStates[GGPO_MAX_SPECTATORS + GGPO_MAX_PLAYERS];

	GameplayState m_gs;

	sf::Clock m_clock;
	sf::Time m_dt;

	IP m_localIP;
	IP m_distantIP;
	
	unsigned int m_frameCount = 0u;

	sf::String m_status;
};
