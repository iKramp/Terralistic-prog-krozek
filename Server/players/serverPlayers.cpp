#include "serverPlayers.hpp"
#include "content.hpp"
#include <cstring>

static bool isBlockTree(Blocks* blocks, int x, int y) {
    return x >= 0 && y >= 0 && x < blocks->getWidth() && y < blocks->getHeight() && (blocks->getBlockType(x, y) == &BlockTypes::wood || blocks->getBlockType(x, y) == &BlockTypes::leaves);
}

static bool isBlockWood(Blocks* blocks, int x, int y) {
    return x >= 0 && y >= 0 && x < blocks->getWidth() && y < blocks->getHeight() && blocks->getBlockType(x, y) == &BlockTypes::wood;
}

static bool isBlockLeaves(Blocks* blocks, int x, int y) {
    return x >= 0 && y >= 0 && x < blocks->getWidth() && y < blocks->getHeight() && blocks->getBlockType(x, y) == &BlockTypes::leaves;
}

static void stoneUpdate(Blocks* blocks, int x, int y) {
    if(y < blocks->getHeight() - 1 && blocks->getBlockType(x, y + 1)->transparent)
        blocks->breakBlock(x, y);
}

static void treeUpdate(Blocks* blocks, int x, int y) {
    if(
       (!isBlockTree(blocks, x, y + 1) && !isBlockTree(blocks, x - 1, y) && !isBlockTree(blocks, x + 1, y)) ||
       (isBlockWood(blocks, x, y - 1) && isBlockWood(blocks, x + 1, y) && !isBlockTree(blocks, x - 1, y) && !isBlockTree(blocks, x, y + 1)) ||
       (isBlockWood(blocks, x, y - 1) && isBlockWood(blocks, x - 1, y) && !isBlockTree(blocks, x + 1, y) && !isBlockTree(blocks, x, y + 1)) ||
       (isBlockLeaves(blocks, x - 1, y) && !isBlockTree(blocks, x + 1, y) && !isBlockTree(blocks, x, y - 1) && !isBlockTree(blocks, x, y + 1)) ||
       (isBlockLeaves(blocks, x + 1, y) && !isBlockTree(blocks, x - 1, y) && !isBlockTree(blocks, x, y - 1) && !isBlockTree(blocks, x, y + 1)) ||
       (!isBlockTree(blocks, x, y + 1) && isBlockLeaves(blocks, x - 1, y) && isBlockLeaves(blocks, x + 1, y) && isBlockLeaves(blocks, x, y - 1))
       )
        blocks->breakBlock(x, y);
}

void ServerPlayers::init() {
    custom_block_events = new BlockEvents[blocks->getNumBlockTypes()];
    
    blocks->block_change_event.addListener(this);
    networking->new_connection_event.addListener(this);
    networking->connection_welcome_event.addListener(this);
    packet_event.addListener(this);
    networking->disconnect_event.addListener(this);
    
    custom_block_events[(int)BlockTypes::wood.id].onUpdate = &treeUpdate;

    custom_block_events[(int)BlockTypes::leaves.id].onUpdate = &treeUpdate;

    custom_block_events[(int)BlockTypes::grass_block.id].onLeftClick = [](Blocks* blocks_, int x, int y, ServerPlayer* player) {
        blocks_->setBlockType(x, y, &BlockTypes::dirt);
    };

    custom_block_events[(int)BlockTypes::air.id].onRightClick = [](Blocks* blocks_, int x, int y, ServerPlayer* player) {
        BlockType* type = player->inventory.getSelectedSlot().type->places;
        if(type != &BlockTypes::air && player->inventory.decreaseStack(player->inventory.selected_slot, 1)) {
            blocks_->setBlockType(x, y, type);
        }
    };

    custom_block_events[(int)BlockTypes::snowy_grass_block.id].onLeftClick = custom_block_events[(int)BlockTypes::grass_block.id].onLeftClick;
    
    custom_block_events[(int)BlockTypes::stone.id].onUpdate = &stoneUpdate;
}

void ServerPlayers::stop() {
    blocks->block_change_event.removeListener(this);
    networking->new_connection_event.removeListener(this);
    networking->connection_welcome_event.removeListener(this);
    packet_event.removeListener(this);
    networking->disconnect_event.removeListener(this);
    
    for(ServerPlayerData* i : all_players)
        delete i;
    
    delete[] custom_block_events;
}

ServerPlayer* ServerPlayers::getPlayerByName(const std::string& name) {
    for(Entity* entity : entities->getEntities())
        if(entity->type == EntityType::PLAYER) {
            ServerPlayer* player = (ServerPlayer*)entity;
            if(player->name == name)
                return player;
        }
    return nullptr;
}

ServerPlayer* ServerPlayers::addPlayer(const std::string& name) {
    ServerPlayerData* player_data = getPlayerData(name);
    
    if(!player_data) {
        int spawn_x = blocks->getWidth() / 2 * BLOCK_WIDTH * 2;
        
        int spawn_y = 0;
        for(int y = 0; y < blocks->getHeight(); y++) {
            if(!blocks->getBlockType(blocks->getWidth() / 2, y)->transparent || !blocks->getBlockType(blocks->getWidth() / 2 + 1, y)->transparent)
                break;
            spawn_y += BLOCK_WIDTH * 2;
        }
        spawn_y -= PLAYER_HEIGHT * 2;
        
        player_data = new ServerPlayerData(items, recipes);
        player_data->name = name;
        player_data->x = spawn_x;
        player_data->y = spawn_y;
        all_players.emplace_back(player_data);
    }
    
    ServerPlayer* player = new ServerPlayer(*player_data);
    entities->registerEntity(player);
    return player;
}

void ServerPlayers::savePlayer(ServerPlayer* player) {
    ServerPlayerData* player_data = getPlayerData(player->name);
    
    player_data->name = player->name;
    player_data->x = player->getX();
    player_data->y = player->getY();
    player_data->inventory = player->inventory;
}

ServerPlayerData* ServerPlayers::getPlayerData(const std::string& name) {
    for(ServerPlayerData* data : all_players)
        if(data->name == name)
            return data;
    return nullptr;
}

void ServerPlayers::leftClickEvent(ServerPlayer* player, int x, int y) {
    while(custom_block_events[blocks->getBlockType(x, y)->id].onLeftClick)
        custom_block_events[blocks->getBlockType(x, y)->id].onLeftClick(blocks, x, y, player);
    
    if(blocks->getBlockType(x, y)->break_time != UNBREAKABLE)
        blocks->startBreakingBlock(x, y);
}

void ServerPlayers::rightClickEvent(ServerPlayer* player, int x, int y) {
    if(custom_block_events[blocks->getBlockType(x, y)->id].onRightClick)
        custom_block_events[blocks->getBlockType(x, y)->id].onRightClick(blocks, x, y, player);
}

char* ServerPlayers::addPlayerFromSerial(char* iter) {
    all_players.emplace_back(new ServerPlayerData(items, recipes, iter));
    return iter;
}

ServerPlayerData::ServerPlayerData(Items* items, Recipes* recipes, char*& iter) : inventory(items, recipes) {
    iter = inventory.loadFromSerial(iter);
    
    memcpy(&x, iter, sizeof(int));
    iter += sizeof(int);
    
    memcpy(&y, iter, sizeof(int));
    iter += sizeof(int);
    
    while(*iter)
        name.push_back(*iter++);
    iter++;
}

void ServerPlayerData::serialize(std::vector<char>& serial) const {
    inventory.serialize(serial);
    
    serial.insert(serial.end(), {0, 0, 0, 0});
    memcpy(&serial[serial.size() - 4], &x, sizeof(int));
    
    serial.insert(serial.end(), {0, 0, 0, 0});
    memcpy(&serial[serial.size() - 4], &y, sizeof(int));
    
    serial.insert(serial.end(), name.begin(), name.end());
    serial.insert(serial.end(), 0);
}

void ServerPlayers::onEvent(BlockChangeEvent& event) {
    int neighbors[5][2] = {{event.x, event.y}, {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}};
    
    if(event.x != 0) {
        neighbors[1][0] = event.x - 1;
        neighbors[1][1] = event.y;
    }
    if(event.x != blocks->getWidth() - 1) {
        neighbors[2][0] = event.x + 1;
        neighbors[2][1] = event.y;
    }
    if(event.y != 0) {
        neighbors[3][0] = event.x;
        neighbors[3][1] = event.y - 1;
    }
    if(event.y != blocks->getHeight() - 1) {
        neighbors[4][0] = event.x;
        neighbors[4][1] = event.y + 1;
    }
    
    for(auto neighbor : neighbors)
        if(neighbor[0] != -1 && custom_block_events[(int)blocks->getBlockType(neighbor[0], neighbor[1])->id].onUpdate)
            custom_block_events[(int)blocks->getBlockType(neighbor[0], neighbor[1])->id].onUpdate(blocks, neighbor[0], neighbor[1]);
}

void ServerPlayers::onEvent(ServerNewConnectionEvent& event) {
    ServerPlayer* player = nullptr;
    
    for(Entity* entity : entities->getEntities())
        if(entity->type == EntityType::PLAYER) {
            ServerPlayer* curr_player = (ServerPlayer*)entity;
            if(curr_player->getConnection() == event.connection) {
                player = curr_player;
                break;
            } else {
                sf::Packet join_packet;
                join_packet << ServerPacketType::PLAYER_JOIN << curr_player->getX() << curr_player->getY() << curr_player->id << curr_player->name << (int)curr_player->moving_type;
                event.connection->send(join_packet);
            }
        }
    
    if(player == nullptr)
        throw Exception("Could not find the player.");
    
    sf::Packet join_packet;
    join_packet << ServerPacketType::PLAYER_JOIN << player->getX() << player->getY() << player->id << player->name << (int)player->moving_type;
    networking->sendToEveryone(join_packet);
}

void ServerPlayers::onEvent(ServerConnectionWelcomeEvent& event) {
    std::string player_name;
    event.client_welcome_packet >> player_name;
    
    ServerPlayer* already_joined_player = getPlayerByName(player_name);
    if(already_joined_player)
        networking->kickConnection(already_joined_player->getConnection(), "You logged in from another location!");
    
    ServerPlayer* player = addPlayer(player_name);
    player->setConnection(event.connection);
    
    sf::Packet packet;
    packet << WelcomePacketType::INVENTORY;
    event.connection->send(packet);
    
    std::vector<char> data;
    player->inventory.serialize(data);
    event.connection->send(data);
}

void ServerPlayer::setConnection(Connection* connection_) {
    if(connection)
        throw Exception("Overwriting connection, which has already been set");
    connection = connection_;
}

Connection* ServerPlayer::getConnection() {
    return connection;
}

void ServerPlayers::update(float frame_length) {
    for(Entity* entity : entities->getEntities())
        if(entity->type == EntityType::PLAYER) {
            ServerPlayer* player = (ServerPlayer*)entity;
            while(player->getConnection()->hasPacketInBuffer()) {
                auto result = player->getConnection()->getPacket();
                
                ServerPacketEvent event(result.first, result.second, player);
                packet_event.call(event);
            }
        }

    for(Entity* entity : entities->getEntities())
        if(entity->type == EntityType::ITEM) {
            Item* item = (Item*)entity;
            for(Entity* entity2 : entities->getEntities())
                if(entity2->type == EntityType::PLAYER) {
                    ServerPlayer* player = (ServerPlayer*)entity2;
                    if(abs(item->getX() + BLOCK_WIDTH - player->getX() - 14) < 50 && abs(item->getY() + BLOCK_WIDTH - player->getY() - 25) < 50 &&
                       player->inventory.addItem(item->getType(), 1) != -1
                       ) {
                        entities->removeEntity(item);
                    }
                }
        }
}

void ServerPlayers::onEvent(ServerDisconnectEvent& event) {
    ServerPlayer* player = nullptr;
    for(Entity* entity : entities->getEntities())
        if(entity->type == EntityType::PLAYER) {
            ServerPlayer* player_ = (ServerPlayer*)entity;
            if(player_->getConnection() == event.connection) {
                player = player_;
                break;
            }
        }
    
    if(player == nullptr)
        throw Exception("Could not find the player.");
    
    savePlayer(player);
    entities->removeEntity(player);
}

void ServerPlayers::onEvent(ServerPacketEvent& event) {
    switch(event.packet_type) {
        case ClientPacketType::STARTED_BREAKING: {
            int breaking_x, breaking_y;
            event.packet >> breaking_x >> breaking_y;
            
            if(event.player->breaking)
                blocks->stopBreakingBlock(event.player->breaking_x, event.player->breaking_y);
            
            event.player->breaking_x = breaking_x;
            event.player->breaking_y = breaking_y;
            event.player->breaking = true;
            leftClickEvent(event.player, breaking_x, breaking_y);
            break;
        }

        case ClientPacketType::STOPPED_BREAKING: {
            event.player->breaking = false;
            blocks->stopBreakingBlock(event.player->breaking_x, event.player->breaking_y);
            break;
        }

        case ClientPacketType::RIGHT_CLICK: {
            int x, y;
            event.packet >> x >> y;
            rightClickEvent(event.player, x, y);
            break;
        }

        case ClientPacketType::PLAYER_VELOCITY: {
            float velocity_x, velocity_y;
            event.packet >> velocity_x >> velocity_y;
            entities->setVelocityX(event.player, velocity_x);
            entities->setVelocityY(event.player, velocity_y);
            break;
        }

        case ClientPacketType::INVENTORY_SWAP: {
            int pos;
            event.packet >> pos;
            event.player->inventory.swapWithMouseItem(pos);
            break;
        }

        case ClientPacketType::HOTBAR_SELECTION: {
            event.packet >> event.player->inventory.selected_slot;
            break;
        }
            
        case ClientPacketType::CRAFT: {
            int craft_index;
            event.packet >> craft_index;
            const Recipe* recipe_crafted = event.player->inventory.getAvailableRecipes()[(int)craft_index];
            event.player->inventory.addItem(recipe_crafted->result.type, recipe_crafted->result.stack);
            
            for(auto ingredient : recipe_crafted->ingredients)
                event.player->inventory.removeItem(ingredient.first, ingredient.second);
        }
            
        case ClientPacketType::PLAYER_MOVING_TYPE: {
            int moving_type;
            event.packet >> moving_type;
            event.player->moving_type = (MovingType)moving_type;
            sf::Packet moving_packet;
            moving_packet << ServerPacketType::PLAYER_MOVING_TYPE << moving_type << event.player->id;
            networking->sendToEveryone(moving_packet);
        }

        case ClientPacketType::PLAYER_JUMPED: {
            sf::Packet jumped_packet;
            jumped_packet << ServerPacketType::PLAYER_JUMPED << event.player->id;
            networking->sendToEveryone(jumped_packet);
        }
        default:;
    }
}

void ServerPlayer::onEvent(InventoryItemChangeEvent& event) {
    ItemStack item = inventory.getItem(event.item_pos);
    sf::Packet packet;
    packet << ServerPacketType::INVENTORY << item.stack << item.type->id << (int)event.item_pos;
    connection->send(packet);
}

ServerPlayer::~ServerPlayer() {
    inventory.item_change_event.removeListener(this);
}
