#include "nameChooser.hpp"
#include "platform_folders.h"
#include "game.hpp"
#include <random>

std::string generateRandomUsername();

void NameChooser::init() {
    config = ConfigFile(sago::getDataHome() + "/Terralistic/servers.txt");
    config.setDefaultStr("username", "");
    
    back_button.setScale(3);
    back_button.loadFromSurface(gfx::textToSurface("Back"));
    back_button.y = -SPACING;
    back_button.orientation = gfx::BOTTOM;
    
    choose_name_title.loadFromSurface(gfx::textToSurface("Choose your username:"));
    choose_name_title.setScale(3);
    choose_name_title.y = SPACING;
    choose_name_title.orientation = gfx::TOP;
    
    join_button.setScale(3);
    join_button.loadFromSurface(gfx::textToSurface("Join server"));
    join_button.y = -SPACING;
    join_button.orientation = gfx::BOTTOM;

    back_button.x = (-join_button.getWidth() - back_button.getWidth() + back_button.getWidth() - SPACING) / 2;
    join_button.x = (join_button.getWidth() + back_button.getWidth() - join_button.getWidth() + SPACING) / 2;

    name_input.setScale(3);
    name_input.orientation = gfx::CENTER;
    name_input.setText(config.getStr("username"));
    name_input.active = true;
    name_input.textProcessing = [](char c, int length) {
        if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')
            return c;
        if(c == ' ')
            return '_';
        return '\0';
    };
    
    name_input.def_color.a = TRANSPARENCY;
    
    text_inputs = {&name_input};

    random_button.setScale(3);
    random_button.loadFromSurface(gfx::textToSurface("Random"));
    random_button.y = SPACING+100;
    random_button.orientation = gfx::CENTER;

    buttons = {&random_button};
}

bool NameChooser::onKeyUp(gfx::Key key) {
    if(key == gfx::Key::MOUSE_LEFT && back_button.isHovered(getMouseX(), getMouseY(), getMouseVel())) {
        returnFromScene();
        return true;
    } else if((key == gfx::Key::MOUSE_LEFT && join_button.isHovered(getMouseX(), getMouseY(), getMouseVel())) || (key == gfx::Key::ENTER && can_join)) {
        Game(menu_back, settings, name_input.getText(), server_ip).start();
        returnFromScene();
        return true;
    } else if (key == gfx::Key::MOUSE_LEFT && random_button.isHovered(getMouseX(), getMouseY(), getMouseVel())) {
        name_input.setText(generateRandomUsername());
        return true;
    }
    return false;
}

void NameChooser::render() {
    menu_back->setBackWidth(name_input.getWidth() + 100);
    menu_back->renderBack();
    if(can_join != !name_input.getText().empty()) {
        can_join = !can_join;
        join_button.loadFromSurface(gfx::textToSurface("Join server", {(unsigned char)(can_join ? WHITE.r : GREY.r), (unsigned char)(can_join ? WHITE.g : GREY.g), (unsigned char)(can_join ? WHITE.b : GREY.b)}));
        join_button.disabled = !can_join;
    }
    join_button.render(getMouseX(), getMouseY(), getMouseVel(), getKeyState(gfx::Key::MOUSE_LEFT));
    back_button.render(getMouseX(), getMouseY(), getMouseVel(), getKeyState(gfx::Key::MOUSE_LEFT));
    choose_name_title.render();
    name_input.render(getMouseX(), getMouseY(), getMouseVel());
    random_button.render(getMouseX(), getMouseY(), getMouseVel(), getKeyState(gfx::Key::MOUSE_LEFT));
}

std::string generateRandomUsername() {
    static const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
    static std::random_device rd;
    static std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, charset.size() - 1);
    std::string username;
    for (int i = 0; i < 8; ++i) {
        username += charset[distribution(generator)];
    }
    return username;
}

void NameChooser::stop() {
    config.setStr("username", name_input.getText());
}
