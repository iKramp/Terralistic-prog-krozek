#ifndef playerHandler_hpp
#define playerHandler_hpp

#include <string>
#include <utility>
#include "graphics.hpp"
#include "clientBlocks.hpp"
#include "resourcePack.hpp"

enum class MovingType {STANDING, WALKING, SNEAKING, SNEAK_WALKING, RUNNING};

class ClientPlayer {
public:
    ClientPlayer(std::string name, int x, int y) : name(std::move(name)), x(x), y(y) {}
    float x, y;
    bool flipped = false;
    unsigned char texture_frame = 0;
    const std::string name;
};

class MainPlayer : public ClientPlayer {
public:
    explicit MainPlayer(int x, int y, std::string name) : ClientPlayer(std::move(name), x, y) {}
    short velocity_x = 0, velocity_y = 0;
    unsigned int started_moving = 0;
    bool has_jumped = false;
    MovingType moving_type = MovingType::STANDING;
};

class OtherPlayer : public ClientPlayer {
public:
    explicit OtherPlayer(std::string name, int x, int y, unsigned short id) : ClientPlayer(std::move(name), x, y), id(id) {}
    const unsigned short id;
    gfx::Image name_text;
};

class ClientPlayers : public gfx::GraphicalModule, EventListener<ClientPacketEvent> {
    bool walking_left = false, walking_right = false, sneaking_left = false, sneaking_right = false, running_left = false, running_right = false;
    
    void render(ClientPlayer& player_to_draw);
    void render(OtherPlayer& player_to_draw);

    MainPlayer main_player;
    std::vector<OtherPlayer*> other_players;
    OtherPlayer* getPlayerById(unsigned short id);
    bool isPlayerColliding();
    bool isPlayerTouchingGround();
    
    void init() override;
    void update() override;
    void onEvent(ClientPacketEvent& event) override;
    
    ClientBlocks* blocks;
    NetworkingManager* manager;
    ResourcePack* resource_pack;
public:
    ClientPlayers(NetworkingManager* manager, ClientBlocks* world_map, ResourcePack* resource_pack, int x, int y, std::string username);
    
    unsigned short getPlayerWidth();
    unsigned short getPlayerHeight();
    
    void renderPlayers();
    
    const MainPlayer& getMainPlayer() { return main_player; }
};

#endif
