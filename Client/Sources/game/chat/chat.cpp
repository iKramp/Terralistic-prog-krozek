//
//  chat.cpp
//  Terralistic
//
//  Created by Jakob Zorz on 13/06/2021.
//

#include "chat.hpp"

#define PADDING 5

void chat::init() {
    chat_box.scale = 2;
    chat_box.setText("");
    chat_box.orientation = gfx::BOTTOM_LEFT;
    chat_box.y = -PADDING;
    chat_box.x = PADDING;
    
    text_inputs = {&chat_box};
}

void chat::update() {
    int target_width = chat_box.active ? 300 : 100;
    chat_box.width += (target_width - (int)chat_box.width) / 3;
    disable_events = chat_box.active;
    
    for(chatLine* i : chat_lines)
        i->text_sprite.y += (i->y_to_be - i->text_sprite.y) / 2;
}

void chat::render() {
    for(chatLine* i : chat_lines) {
        if(!i->text.empty()) {
            i->text_sprite.renderText(i->text, {255, 255, 255});
            i->text_sprite.scale = 2;
            i->text_sprite.y = chat_box.y;
            i->text_sprite.x = PADDING;
            i->text_sprite.orientation = gfx::BOTTOM_LEFT;
            i->y_to_be = chat_box.y - chat_box.getHeight();
            i->text.clear();
            
            for(chatLine* line : chat_lines)
                if(line != i)
                    line->y_to_be -= i->text_sprite.getHeight();
        }
        
        if(i->time_created + 10500 > gfx::getTicks() || chat_box.active) {
            int alpha = i->time_created + 10500 - gfx::getTicks();
            if(alpha < 0 || alpha >= 500)
                alpha = 500;
            i->text_sprite.setAlpha((float)alpha / 500.0f * 255);
            i->text_sprite.render();
        }
    }
            
    chat_box.render();
}

void chat::onKeyDown(gfx::Key key) {
    if(key == gfx::Key::ENTER && chat_box.active) {
        if(!chat_box.getText().empty()) {
            sf::Packet chat_packet;
            chat_packet << PacketType::CHAT << chat_box.getText();
            manager->sendPacket(chat_packet);
            chat_box.setText("");
        }
        chat_box.active = false;
    } else if(key == gfx::Key::T && !chat_box.active) {
        chat_box.active = true;
        chat_box.ignore_one_input = true;
    } else if(key == gfx::Key::ESCAPE && chat_box.active) {
        chat_box.setText("");
        chat_box.active = false;
    }
}

void chat::onEvent(ClientPacketEvent &event) {
    switch(event.packet_type) {
        case PacketType::CHAT: {
            chatLine* new_line = new chatLine;
            event.packet >> new_line->text;
            new_line->time_created = gfx::getTicks();
            chat_lines.push_back(new_line);
            break;
        }
        default:;
    }
}

void chat::stop() {
    for(chatLine* i : chat_lines)
        delete i;
}
