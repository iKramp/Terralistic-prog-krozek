#include "clientLiquids.hpp"
#include "readOpa.hpp"

void ClientLiquids::onEvent(ClientPacketEvent &event) {
    switch(event.packet_type) {
        case ServerPacketType::LIQUID: {
            int x, y;
            int liquid_type;
            float liquid_level;
            event.packet >> x >> y >> liquid_type >> liquid_level;
            
            setLiquidType(x, y, getLiquidTypeById(liquid_type));
            setLiquidLevel(x, y, liquid_level);
            
            break;
        }
        default:;
    }
}

void ClientLiquids::onEvent(WelcomePacketEvent& event) {
    if(event.packet_type == WelcomePacketType::LIQUIDS) {
        std::vector<char> data;
        event.packet >> data;
        fromSerial(data);
    }
}

void ClientLiquids::onEvent(LiquidChangeEvent& event) {
    scheduleLiquidUpdate(event.x, event.y);
}

void ClientLiquids::init() {
    networking->packet_event.addListener(this);
    networking->welcome_packet_event.addListener(this);
    liquid_change_event.addListener(this);
    debug_menu->registerDebugLine(&render_time_line);
}

void ClientLiquids::loadTextures() {
    std::vector<gfx::Surface> liquid_surfaces(getNumLiquidTypes() - 1);

    for(int i = 1; i < getNumLiquidTypes(); i++)
        liquid_surfaces[i - 1] = readOpa(resource_pack->getFile("/liquids/" + getLiquidTypeById(i)->name + ".opa"));
    
    liquids_atlas.create(liquid_surfaces);
}

void ClientLiquids::postInit() {
    liquid_chunks = new RenderLiquidChunk[getWidth() / 16 * getHeight() / 16];
}

void ClientLiquids::stop() {
    networking->packet_event.removeListener(this);
    networking->welcome_packet_event.removeListener(this);
    liquid_change_event.removeListener(this);
    delete[] liquid_chunks;
}

void ClientLiquids::scheduleLiquidUpdate(int x, int y) {
    getRenderLiquidChunk(x / CHUNK_SIZE, y / CHUNK_SIZE)->has_update = true;
}

void ClientLiquids::updateParallel(float frame_length) {
    for(int y = blocks->getBlocksViewBeginY() / CHUNK_SIZE; y <= blocks->getBlocksViewEndY() / CHUNK_SIZE; y++)
        for(int x = blocks->getBlocksViewBeginX() / CHUNK_SIZE; x <= blocks->getBlocksViewEndX() / CHUNK_SIZE; x++) {
            if(!getRenderLiquidChunk(x, y)->isCreated())
                getRenderLiquidChunk(x, y)->create();
            
            if(getRenderLiquidChunk(x, y)->has_update)
                getRenderLiquidChunk(x, y)->update(this, x, y);
        }
}

void ClientLiquids::render() {
    gfx::Timer render_timer;
    for(int y = blocks->getBlocksViewBeginY() / CHUNK_SIZE; y <= blocks->getBlocksViewEndY() / CHUNK_SIZE; y++)
        for(int x = blocks->getBlocksViewBeginX() / CHUNK_SIZE; x <= blocks->getBlocksViewEndX() / CHUNK_SIZE; x++)
            getRenderLiquidChunk(x, y)->render(this, x * CHUNK_SIZE * BLOCK_WIDTH * 2 - camera->getX() + gfx::getWindowWidth() / 2, y * CHUNK_SIZE * BLOCK_WIDTH * 2 - camera->getY() + gfx::getWindowHeight() / 2);
    
    render_time_sum += render_timer.getTimeElapsed();
    fps_count++;
    if(line_refresh_timer.getTimeElapsed() > 1000) {
        render_time_line.text = std::to_string(render_time_sum / fps_count) + "ms liquids render";
        
        fps_count = 0;
        render_time_sum = 0;
        line_refresh_timer.reset();
    }
}

void ClientLiquids::RenderLiquidChunk::update(ClientLiquids* liquids, int x, int y) {
    has_update = false;
    int index = 0;
    for(int y_ = y * CHUNK_SIZE; y_ < (y + 1) * CHUNK_SIZE; y_++)
        for(int x_ = x * CHUNK_SIZE; x_ < (x + 1) * CHUNK_SIZE; x_++)
            if(liquids->getLiquidType(x_, y_) != &liquids->empty) {
                int texture_y = liquids->getLiquidRectInAtlas(liquids->getLiquidType(x_, y_)).y * 2;
                liquid_rects.setTextureCoords(index, {0, texture_y, BLOCK_WIDTH, BLOCK_WIDTH});
                int level = std::min((liquids->getLiquidLevel(x_, y_) * 1.02) / MAX_LIQUID_LEVEL, 1.0) * BLOCK_WIDTH * 2;
                liquid_rects.setRect(index, {(x_ % CHUNK_SIZE) * BLOCK_WIDTH * 2, (y_ % CHUNK_SIZE) * BLOCK_WIDTH * 2 + BLOCK_WIDTH * 2 - level, BLOCK_WIDTH * 2, level});
                liquid_rects.setColor(index, {255, 255, 255});
                index++;
            }
    liquid_count = index;
}

void ClientLiquids::RenderLiquidChunk::render(ClientLiquids* liquids, int x, int y) {
    if(liquid_count > 0)
        liquid_rects.render(&liquids->getLiquidsAtlasTexture(), x, y, liquid_count);
}

void ClientLiquids::RenderLiquidChunk::create() {
    liquid_rects.resize(CHUNK_SIZE * CHUNK_SIZE);
    is_created = true;
}

const gfx::Texture& ClientLiquids::getLiquidsAtlasTexture() {
    return liquids_atlas.getTexture();
}

gfx::RectShape ClientLiquids::getLiquidRectInAtlas(LiquidType* type) {
    return liquids_atlas.getRect(type->id - 1);
}

ClientLiquids::RenderLiquidChunk* ClientLiquids::getRenderLiquidChunk(int x, int y) {
    if(liquid_chunks == nullptr)
        throw Exception("Liquid chunks is null");
    return &liquid_chunks[y * getWidth() / 16 + x];
}
