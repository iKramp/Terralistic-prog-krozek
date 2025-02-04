#pragma once
#include "clientBlocks.hpp"

class ClientLiquids : public Liquids, public ClientModule, EventListener<ClientPacketEvent>, EventListener<WelcomePacketEvent>, EventListener<LiquidChangeEvent> {
    class RenderLiquidChunk {
        gfx::RectArray liquid_rects;
        int liquid_count = 0;
        bool is_created = false;
    public:
        bool isCreated() const { return is_created; }
        void create();
        bool has_update = true;
        void update(ClientLiquids* liquids, int x, int y);
        void render(ClientLiquids* liquids, int x, int y);
    };
    
    RenderLiquidChunk* liquid_chunks = nullptr;
    
    gfx::TextureAtlas liquids_atlas;
    
    ClientBlocks* blocks;
    ResourcePack* resource_pack;
    ClientNetworking* networking;
    Camera* camera;
    
    RenderLiquidChunk* getRenderLiquidChunk(int x, int y);
    
    DebugMenu* debug_menu;
    gfx::Timer line_refresh_timer;
    int fps_count = 0;
    float render_time_sum = 0;
    DebugLine render_time_line;
    
    void onEvent(ClientPacketEvent& event) override;
    void onEvent(WelcomePacketEvent& event) override;
    void onEvent(LiquidChangeEvent& event) override;
    
    void init() override;
    void postInit() override;
    void loadTextures() override;
    void updateParallel(float frame_length) override;
    void render() override;
    void stop() override;

    void scheduleLiquidUpdate(int x, int y);
public:
    ClientLiquids(DebugMenu* debug_menu, ClientBlocks* blocks, ResourcePack* resource_pack, ClientNetworking* networking, Camera* camera) : ClientModule("ClientLiquids"), debug_menu(debug_menu), Liquids(blocks), resource_pack(resource_pack), networking(networking), blocks(blocks), camera(camera) {}
    
    const gfx::Texture& getLiquidsAtlasTexture();
    gfx::RectShape getLiquidRectInAtlas(LiquidType* type);
};
