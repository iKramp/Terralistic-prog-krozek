#pragma once
#include <utility>

#include "player.hpp"
#include "particles.hpp"

#define PLAYER_WIDTH 14
#define PLAYER_HEIGHT 24

class ClientPlayer : public Player {
public:
    ClientPlayer(const std::string& name, int x, int y, int health, int id);
    void loadSkin(const gfx::Surface& skin, ResourcePack* resource_pack);
    int texture_frame = 0;
    gfx::Texture name_text;
    gfx::Texture player_texture;
    gfx::Surface player_surface;
    bool has_created_text = false, has_created_texture = false, has_created_surface = false;
    int started_moving = 0;
    bool has_jumped = false;
};

class PlayerCreationEvent {
public:
    PlayerCreationEvent(ClientPlayer* player) : player(player) {}
    ClientPlayer* player;
};

class PlayerDeletionEvent {
public:
    PlayerDeletionEvent(ClientPlayer* player) : player(player) {}
    ClientPlayer* player;
};

class ClientPlayers : public ClientModule, EventListener<ClientPacketEvent> {
    bool walking_left = false, walking_right = false, sneaking_left = false, sneaking_right = false, running_left = false, running_right = false;
    
    void render(ClientPlayer& player_to_draw);

    std::string username;
    ClientPlayer* main_player = nullptr;
    gfx::Texture player_texture;
    
    void init() override;
    void loadTextures() override;
    void update(float frame_length) override;
    void onEvent(ClientPacketEvent& event) override;
    void render() override;
    void stop() override;
    
    gfx::Timer timer;
    
    ClientBlocks* blocks;
    Liquids* liquids;
    ClientNetworking* networking;
    ResourcePack* resource_pack;
    Entities* entities;
    Particles* particles;
    Camera* camera;
public:
    ClientPlayers(ClientNetworking* networking, ClientBlocks* blocks, Liquids* liquids, ResourcePack* resource_pack, Entities* entities, Particles* particles, Camera* camera, std::string  username) : ClientModule("ClientPlayers"), networking(networking), blocks(blocks), liquids(liquids), resource_pack(resource_pack), entities(entities), particles(particles), camera(camera), username(std::move(username)) {}
    
    ClientPlayer* getMainPlayer() { return main_player; }
    void loadPlayerTexture(ClientPlayer& player);
    ClientPlayer* getPlayerById(int id);
    
    EventSender<PlayerCreationEvent> player_creation_event;
    EventSender<PlayerDeletionEvent> player_deletion_event;
};
