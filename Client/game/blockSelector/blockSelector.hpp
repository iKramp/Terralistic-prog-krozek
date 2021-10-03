#ifndef blockSelector_hpp
#define blockSelector_hpp

#include "graphics.hpp"
#include "blockRenderer.hpp"
#include "clientInventory.hpp"
#include "clientPlayers.hpp"

class BlockSelector : public gfx::SceneModule {
    void init() override;
    void render() override;

    bool onKeyDown(gfx::Key key) override;
    
    int prev_selected_x = 0, prev_selected_y = 0, selected_block_x = 0, selected_block_y = 0;
    gfx::Rect select_rect;
    
    bool is_left_button_pressed = false;
    
    NetworkingManager* manager;
    Blocks* blocks;
    BlockRenderer* client_blocks;
    ClientPlayers* player_handler;
public:
    BlockSelector(NetworkingManager* manager, Blocks* blocks, BlockRenderer* client_blocks, ClientPlayers* player_handler) : manager(manager), blocks(blocks), client_blocks(client_blocks), player_handler(player_handler) {}
};

#endif
