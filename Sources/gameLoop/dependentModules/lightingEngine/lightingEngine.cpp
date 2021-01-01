//
//  lightingEngine.cpp
//  Terralistic
//
//  Created by Jakob Zorz on 28/12/2020.
//

#include "lightingEngine.hpp"
#include "blockEngine.hpp"
#include "singleWindowLibrary.hpp"
#include "objectedGraphicsLibrary.hpp"

ogl::texture light_text;

void lightingEngine::init() {
    light_text.loadFromText("Initializing lights", {255, 255, 255});
    light_text.scale = 3;
}

void lightingEngine::removeNaturalLight(unsigned short x) {
    for(unsigned short y = 0; blockEngine::getBlock(x, y).getUniqueBlock().transparent; y++)
        removeLightSource(x, y);
}

void lightingEngine::setNaturalLight(unsigned short x) {
    for(unsigned short y = 0; blockEngine::getBlock(x, y).getUniqueBlock().transparent; y++)
        addLightSource(x, y, MAX_LIGHT);
}

lightingEngine::lightBlock& lightingEngine::getLightBlock(unsigned short x, unsigned short y) {
    return blockEngine::getChunk(x >> 4, y >> 4).light_blocks[x & 15][y & 15];
}

void lightingEngine::addLightSource(unsigned short x, unsigned short y, unsigned char power) {
    lightBlock& block = getLightBlock(x, y);
    block.source = true;
    block.level = power;
    block.update(x, y);
}

void lightingEngine::removeLightSource(unsigned short x, unsigned short y) {
    lightBlock& block = getLightBlock(x, y);
    block.source = false;
    block.level = 0;
    block.update(x, y);
}

void lightingEngine::prepareLights() {
    swl::setDrawColor(0, 0, 0);
    swl::clear();
    light_text.render();
    swl::update();
    
    for(unsigned short x = 0; x < blockEngine::world_width; x++)
        lightingEngine::setNaturalLight(x);
}
