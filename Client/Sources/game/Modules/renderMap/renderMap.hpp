//
//  renderMap.hpp
//  Terralistic
//
//  Created by Jakob Zorz on 08/04/2021.
//

#ifndef renderMap_hpp
#define renderMap_hpp

#ifdef __WIN32__
#include "graphics.hpp"
#else
#include <Graphics/graphics.hpp>
#endif

#include "map.hpp"
#include "networkingModule.hpp"

class renderMap : public map, public gfx::sceneModule, packetListener {
    struct uniqueRenderItem {
        gfx::image texture;
        gfx::sprite text_texture;
    };
    
    struct uniqueRenderBlock {
        void loadFromUniqueBlock(map::uniqueBlock* unique_block);
        gfx::image texture;
        std::vector<map::blockType> connects_to;
        bool single_texture;
    };

    struct renderBlock {
        unsigned char orientation{0};
        bool update = true;
    };

    struct renderChunk {
        bool update = true;
        gfx::image texture;
        void createTexture();
    };
    
    static gfx::image breaking_texture;
    renderChunk* render_chunks = nullptr;
    renderBlock* render_blocks = nullptr;
    static uniqueRenderBlock* unique_render_blocks;
    
    renderBlock& getRenderBlock(unsigned short x, unsigned short y);
    renderChunk& getRenderChunk(unsigned short x, unsigned short y);
    
    void drawBlock(unsigned short x, unsigned short y);
    void updateChunkTexture(unsigned short x, unsigned short y);
    void scheduleTextureUpdateForBlock(unsigned short x, unsigned short y);
    void drawChunk(unsigned short x, unsigned short y);
    void updateBlockOrientation(unsigned short x, unsigned short y);
    uniqueRenderBlock& getUniqueRenderBlock(unsigned short x, unsigned short y);
    
    void updateBlock(unsigned short x, unsigned short y);
    
    static uniqueRenderItem* unique_render_items;
    
    void renderBlocks();
    void renderItems();
    
    void init() override;
    void render() override { renderBlocks(); renderItems(); }
    void stop() override;
    void onPacket(packets::packet packet) override;
    
    networkingManager* networking_manager;
public:
    renderMap(networkingManager* manager) : packetListener(manager), networking_manager(manager) {}
    static void loadBlocks();
    static void loadItems();
    
    static uniqueRenderItem& getUniqueRenderItem(map::itemType type);
};

#endif /* renderMap_hpp */
