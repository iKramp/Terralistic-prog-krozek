//
//  blockSelector.cpp
//  Terralistic
//
//  Created by Jakob Zorz on 13/07/2020.
//

#include "core.hpp"

#include "blockSelector.hpp"
#include "playerHandler.hpp"
#include "networkingModule.hpp"
#include "gameLoop.hpp"
#include "blockRenderer.hpp"

// this is a rectangle with which you select which block to break or where to place selected block

gfx::rect select_rect;
bool left_button_pressed = false;
unsigned short prev_selected_x = 0, prev_selected_y = 0;

INIT_SCRIPT
    //selectRect.fill = false;
    select_rect.c = {255, 0, 0};
    select_rect.w = BLOCK_WIDTH;
    select_rect.h = BLOCK_WIDTH;
INIT_SCRIPT_END

void rightClickEvent(unsigned short x, unsigned short y) {
    if(gameLoop::online) {
        packets::packet packet(packets::RIGHT_CLICK);
        packet << x << y;
        networking::sendPacket(packet);
    } else {
        blockEngine::block* block = &blockEngine::getBlock(x, y);
        if(blockSelector::click_events[block->block_id].rightClickEvent)
            blockSelector::click_events[block->block_id].rightClickEvent(block);
    }
}

void leftClickEvent(unsigned short x, unsigned short y) {
    blockEngine::block* block = &blockEngine::getBlock(x, y);
    if(blockSelector::click_events[block->block_id].leftClickEvent)
        blockSelector::click_events[block->block_id].leftClickEvent(block);
    else {
        block->setBreakProgress(block->break_progress_ms + gfx::frame_length);
        if(!gameLoop::online && block->break_progress_ms >= block->getUniqueBlock().break_time)
            block->break_block();
    }
}

void blockSelector::render() {
    if((prev_selected_y != blockSelector::selected_block_y || prev_selected_x != blockSelector::selected_block_x) && left_button_pressed) {
        packets::packet packet(packets::STARTED_BREAKING);
        packet << blockSelector::selected_block_x << blockSelector::selected_block_y;
        networking::sendPacket(packet);
        prev_selected_x = blockSelector::selected_block_x;
        prev_selected_y = blockSelector::selected_block_y;
    }
    
    if(left_button_pressed && !gameLoop::online)
        leftClickEvent(blockSelector::selected_block_x, blockSelector::selected_block_y);
    
    if(!playerHandler::hovered) {
        selected_block_x = (unsigned short)(gfx::getMouseX() + playerHandler::view_x - gfx::getWindowWidth() / 2) / BLOCK_WIDTH;
        selected_block_y = (unsigned short)(gfx::getMouseY() + playerHandler::view_y - gfx::getWindowHeight() / 2) / BLOCK_WIDTH;
        select_rect.x = -playerHandler::view_x + gfx::getWindowWidth() / 2 + selected_block_x * BLOCK_WIDTH;
        select_rect.y = -playerHandler::view_y + gfx::getWindowHeight() / 2 + selected_block_y * BLOCK_WIDTH;
        gfx::render(select_rect, false);
    }
}

bool blockSelector::collidingWithPlayer() {
    return gfx::colliding(playerHandler::player.getTranslatedRect(), select_rect.getTranslatedRect());
}

void blockSelector::onKeyDown(gfx::key key) {
    if(key == gfx::KEY_MOUSE_LEFT && !playerHandler::hovered) {
        left_button_pressed = true;
        prev_selected_x = blockEngine::world_width;
        prev_selected_y = blockEngine::world_height;
    } else if(key == gfx::KEY_MOUSE_RIGHT && !blockSelector::collidingWithPlayer() && !playerHandler::hovered)
        rightClickEvent(blockSelector::selected_block_x, blockSelector::selected_block_y);
}

void blockSelector::onKeyUp(gfx::key key) {
    if(key == gfx::KEY_MOUSE_LEFT && !playerHandler::hovered) {
        left_button_pressed = false;
        packets::packet packet(packets::STOPPED_BREAKING);
        networking::sendPacket(packet);
    }
}
