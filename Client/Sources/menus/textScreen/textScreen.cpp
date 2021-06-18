//
//  textScreen.cpp
//  Terralistic
//
//  Created by Jakob Zorz on 26/03/2021.
//

#ifdef __APPLE__

#ifdef DEVELOPER_MODE
#include <Graphics_Debug/graphics.hpp>
#else
#include <Graphics/graphics.hpp>
#endif

#else
#include "graphics.hpp"
#endif


#include "textScreen.hpp"

void renderTextScreen(const std::string &text) {
    gfx::sprite text_image;
    text_image.setTexture(gfx::renderText(text, {255, 255, 255}));
    text_image.orientation = gfx::center;
    text_image.scale = 3;

    gfx::clearWindow();
    gfx::render(text_image);
    gfx::updateWindow();
}
