#include "pauseScreen.hpp"
#include "settingsMenu.hpp"
#include "modManager.hpp"

void PauseScreen::init() {
    resume_button.setScale(3);
    resume_button.loadFromSurface(gfx::textToSurface("Resume"));
    resume_button.y = SPACING;
    resume_button.x = SPACING;
    resume_button.parent_containter = &back_rect;

    settings_button.setScale(3);
    settings_button.loadFromSurface(gfx::textToSurface("Settings"));
    settings_button.y = resume_button.y + resume_button.getHeight() + SPACING;
    settings_button.x = SPACING;
    settings_button.parent_containter = &back_rect;
    
    mods_button.setScale(3);
    mods_button.loadFromSurface(gfx::textToSurface("Mods"));
    mods_button.y = settings_button.y + settings_button.getHeight() + SPACING;
    mods_button.x = SPACING;
    mods_button.parent_containter = &back_rect;
    
    quit_button.setScale(3);
    quit_button.loadFromSurface(gfx::textToSurface("Leave Game"));
    quit_button.y = mods_button.y + mods_button.getHeight() + SPACING;
    quit_button.x = SPACING;
    quit_button.parent_containter = &back_rect;
    
    back_rect.fill_color.a = TRANSPARENCY;
    back_rect.shadow_intensity = SHADOW_INTENSITY;
    back_rect.border_color = BORDER_COLOR;
    back_rect.border_color.a = TRANSPARENCY;
    back_rect.x = -(quit_button.getWidth() + 2 * SPACING) - 200;
    back_rect.smooth_factor = 3;
    back_rect.blur_radius = BLUR;
}

bool PauseScreen::onKeyDown(gfx::Key key) {
    if(key == gfx::Key::ESCAPE) {
        returnToGame();
        return true;
    }
    return false;
}

bool PauseScreen::onKeyUp(gfx::Key key) {
    if(key == gfx::Key::MOUSE_LEFT) {
        if(resume_button.isHovered(getMouseX(), getMouseY(), getMouseVel())) {
            returnToGame();
            return true;
        } else if(settings_button.isHovered(getMouseX(), getMouseY(), getMouseVel())) {
            SettingsMenu settings_menu(this, settings);
            settings_menu.run();
            return true;
        } else if(mods_button.isHovered(getMouseX(), getMouseY(), getMouseVel())) {
            ModManager mod_manager(this);
            mod_manager.run();
            if(mod_manager.changed_mods)
                changed_mods = true;
            return true;
        } else if(quit_button.isHovered(getMouseX(), getMouseY(), getMouseVel())) {
            exitToMenu();
            return true;
        }
    }
    return false;
}

void PauseScreen::render() {
    back_width = quit_button.getWidth() + 2 * SPACING;
    back_rect.w = back_width;
    
    renderBackground();
    renderButtons();
}

void PauseScreen::renderBackground() {
    bool blur_enabled = gfx::blur_enabled;
    gfx::blur_enabled = false;
    background->renderBack();
    gfx::blur_enabled = blur_enabled;
    
    if(back_rect.h != gfx::getWindowHeight()) {
        back_rect.h = gfx::getWindowHeight();
        back_rect.jumpToTarget();
    }
    
    fade_rect.w = gfx::getWindowWidth();
    fade_rect.h = gfx::getWindowHeight();
    int back_x = std::min(back_rect.x, 0);
    fade_rect.fill_color.a = float(back_width + back_x + 200) / (float)(back_width + 200) * 70;
    fade_rect.blur_radius = float(back_width + back_x + 200) / (float)(back_width + 200) * BLUR / 2;
    if(fade_rect.blur_radius < 0.5f)
        fade_rect.blur_radius = 0;
    fade_rect.render();
    back_rect.render();
}

void PauseScreen::renderButtons() {
    
    resume_button.render(getMouseX(), getMouseY(), getMouseVel(), getKeyState(gfx::Key::MOUSE_LEFT));
    settings_button.render(getMouseX(), getMouseY(), getMouseVel(), getKeyState(gfx::Key::MOUSE_LEFT));
    mods_button.render(getMouseX(), getMouseY(), getMouseVel(), getKeyState(gfx::Key::MOUSE_LEFT));
    quit_button.render(getMouseX(), getMouseY(), getMouseVel(), getKeyState(gfx::Key::MOUSE_LEFT));
    
    if(returning_to_game) {
        if(back_rect.x == -back_width - 200)
            returnFromScene();
    } else
        back_rect.x = 0;
}

void PauseScreen::returnToGame() {
    returning_to_game = true;
    back_rect.x = -back_width - 200;
}

void PauseScreen::exitToMenu() {
    exited_to_menu = true;
    returnFromScene();
}

bool PauseScreen::hasExitedToMenu() const {
    return exited_to_menu;
}

void PauseScreen::renderBack() {
    renderBackground();
    back_rect.x = gfx::getWindowWidth() / 2 - getBackWidth() / 2;
}

void PauseScreen::setBackWidth(int width) {
    back_rect.w = width;
}

int PauseScreen::getBackWidth() {
    return back_rect.w;
}

gfx::Container* PauseScreen::getBackContainer() {
    return &back_rect;
}
