#pragma once
#include <thread>
#include "clientItems.hpp"
#include "menuBack.hpp"
#include "clientEntities.hpp"
#include "clientInventory.hpp"
#include "blockSelector.hpp"
#include "debugMenu.hpp"
#include "chat.hpp"
#include "clientLiquids.hpp"
#include "clientLights.hpp"
#include "clientHealth.hpp"
#include "content.hpp"
#include "naturalLight.hpp"
#include "camera.hpp"
#include "background.hpp"
#include "clientWalls.hpp"
#include "respawnScreen.hpp"
#include "backgroundRect.hpp"

void startPrivateWorld(const std::string& world_name, BackgroundRect* menu_back, Settings* settings, int world_seed);

class Game : gfx::Scene, public Background {
    void preInit() override;
    void init() override;
    bool onKeyDown(gfx::Key key) override;
    void update(float frame_length) override;
    void stop() override;
    
    void initializeGame();
    std::string username;
    
    ClientNetworking networking;
    ResourcePack resource_pack;
    Camera camera;
    GameBackground background;
    ClientBlocks blocks;
    ClientWalls walls;
    Particles particles;
    ClientLiquids liquids;
    ClientLights lights;
    NaturalLight natural_light;
    ClientEntities entities;
    ClientItems items;
    ClientPlayers players;
    BlockSelector block_selector;
    ClientInventory inventory;
    Chat chat;
    DebugMenu debug_menu;
    ClientHealth player_health;
    RespawnScreen respawn_screen;
    Recipes recipes;
    GameContent content;
    
    Settings* settings;
    BackgroundRect* background_rect;
    std::string kick_reason;
    
    DebugLine fps_debug_line, frame_length_line;
    int fps_count = 0, ms_timer_counter = 0;
    float frame_length_sum = 0;
    gfx::Timer line_refresh_counter, ms_timer;
    
    std::thread parallel_update_thread;
    
    void parallelUpdateLoop();
public:
    Game(BackgroundRect* background_rect, Settings* settings, const std::string& username, const std::string& ip_address, int port=33770);
    
    using gfx::Scene::isInitialized;
    void renderBack() override;
    
    bool interrupt = false;
    std::string interrupt_message;
    
    void start();
};
