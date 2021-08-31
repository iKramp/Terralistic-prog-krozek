#include "mainMenu.hpp"
#include "worldSelector.hpp"
#include "multiplayerSelector.hpp"

#define BUTTON_SPACING 1

void MainMenu::init() {
    singleplayer_button.scale = 3;
    singleplayer_button.renderText("Singleplayer");
    singleplayer_button.orientation = gfx::TOP;
    
    multiplayer_button.scale = 3;
    multiplayer_button.renderText("Multiplayer");
    multiplayer_button.orientation = gfx::TOP;
    
    settings_button.scale = 3;
    settings_button.renderText("Settings");
    settings_button.orientation = gfx::TOP;

    exit_button.scale = 3;
    exit_button.renderText("Exit");
    exit_button.orientation = gfx::TOP;
    
#ifdef DEVELOPER_MODE
    debug_title.renderText("DEBUG MODE", GREY);
    debug_title.orientation = gfx::TOP;
    debug_title.scale = 2;
    debug_title.y = SPACING / 4;
#endif
    
    title.renderText("Terralistic");
    title.scale = 4;
    title.orientation = gfx::TOP;
    title.y = debug_title.y + debug_title.getHeight() + SPACING / 2;
    
    version.renderText(
#include "version.hpp"
                                       , GREY);
    version.orientation = gfx::BOTTOM;
    version.scale = 2;
    version.y = -5;
    
    menu_back.init();
}

void MainMenu::onKeyDown(gfx::Key key) {
    if(key == gfx::Key::MOUSE_LEFT) {
        if(exit_button.isHovered())
            gfx::returnFromScene();
        else if(singleplayer_button.isHovered())
            WorldSelector(&menu_back).run();
        else if(multiplayer_button.isHovered())
            MultiplayerSelector(&menu_back).run();
    }
}

void MainMenu::render() {
    int height = singleplayer_button.getHeight() + multiplayer_button.getHeight() + settings_button.getHeight() + exit_button.getHeight() + 3 * BUTTON_SPACING;
    
    singleplayer_button.y = gfx::getWindowHeight() / 2 - height / 2;
    multiplayer_button.y = singleplayer_button.y + singleplayer_button.getHeight() + BUTTON_SPACING;
    settings_button.y = multiplayer_button.y + multiplayer_button.getHeight() + BUTTON_SPACING;
    exit_button.y = settings_button.y + settings_button.getHeight() + BUTTON_SPACING;
    
    menu_back.setBackWidth(singleplayer_button.getWidth() + 100);
    menu_back.renderBack();

    title.render();
#ifdef DEVELOPER_MODE
    debug_title.render();
#endif
    version.render();
    singleplayer_button.render();
    multiplayer_button.render();
    settings_button.render();
    exit_button.render();
}
