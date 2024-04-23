#pragma once
#include <utility>

#include "menuBack.hpp"
#include "settings.hpp"

class NameChooser : public gfx::Scene {
private:
    ConfigFile config; // Declare config as a member variable
    gfx::Button random_button; // Declare random_button as a member variable
    std::vector<gfx::Button*> buttons; // Declare buttons as a member variable

    gfx::Button back_button, join_button;
    gfx::Sprite choose_name_title;
    gfx::TextInput name_input;
    BackgroundRect* menu_back;
    Settings* settings;
    std::string server_ip;
    bool can_join = true;
    void init() override;
    bool onKeyUp(gfx::Key key) override;
    void render() override;
    void stop() override;
public:
    // void init();
    // bool onKeyUp(gfx::Key key);
    // void render();
    // void stop();
    NameChooser(BackgroundRect* menu_back, Settings* settings, std::string server_ip) : gfx::Scene("NameChooser"), menu_back(menu_back), settings(settings), server_ip(std::move(server_ip)) {}
};
