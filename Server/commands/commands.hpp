#pragma once

#include "serverPlayers.hpp"
#include "string"
#include "worldGenerator.hpp"
#include "serverEntities.hpp"

class Commands {
    ServerBlocks* blocks;
    ServerPlayers* players;
    ServerItems* items;
    ServerEntities* entities;
public:
    Commands(ServerBlocks* blocks, ServerPlayers* players, ServerItems* items, ServerEntities* entities) : blocks(blocks), players(players), items(items), entities(entities){}
    void startCommand(std::string message, std::string player);
    void changeBlock(std::vector<std::string>& message);
    void formatLocation(std::vector<std::string>& message, const std::string& player, unsigned char start_format);
    void formatBlockType(std::string& type);
    void formatItemType(std::string& type);
    void teleport(const std::string& player, unsigned int x, unsigned int y);
    void giveItem(std::vector<std::string>& message, const std::string& player);
};
