#include "background.hpp"
#include "readOpa.hpp"

void GameBackground::loadTextures() {
    background.loadFromSurface(readOpa(resource_pack->getFile("/misc/background.opa")));
}

void GameBackground::render() {
    float scale = (float)gfx::getWindowHeight() / background.getTextureHeight();
    int position_x = -int(camera->getX() * scale / 20) % int(background.getTextureWidth() * scale);
    for(int i = 0; i < gfx::getWindowWidth() / (background.getTextureWidth() * scale) + 2; i++)
        background.render(scale, position_x + i * background.getTextureWidth() * scale, 0);
}
