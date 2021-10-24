#pragma once

#include "graphics.hpp"
#include "clientBlocks.hpp"
#include "liquids.hpp"
#include "lights.hpp"
#include "naturalLight.hpp"

#define MINIMAP_SIZE 100
#define MINIMAP_SCALE 2

class Minimap : public ClientModule {
    gfx::Rect back_rect;
    gfx::PixelGrid block_pixels, liquid_pixels, light_pixels;
    Liquids* liquids;
    Lights* lights;
    ClientBlocks* blocks;
    NaturalLight* natural_light;
    gfx::Color block_colors[(int)BlockType::NUM_BLOCKS];
    gfx::Color liquid_colors[(int)LiquidType::NUM_LIQUIDS];
    sf::Texture minimap_texture;
    
    void init() override;
    void render() override;
public:
    Minimap(ClientBlocks* blocks, Liquids* liquids, Lights* lights, NaturalLight* natural_light) : blocks(blocks), liquids(liquids), lights(lights), natural_light(natural_light), block_pixels(MINIMAP_SIZE, MINIMAP_SIZE), liquid_pixels(MINIMAP_SIZE, MINIMAP_SIZE), light_pixels(MINIMAP_SIZE, MINIMAP_SIZE) {}
};
