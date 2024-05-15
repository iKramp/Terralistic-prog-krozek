#pragma once
#include "inventory.hpp"
#include "serverPlayers.hpp"

class ServerInventory : public Inventory {
ServerPlayers* server_players;

public:
    ServerInventory(Items* items, Recipes* recipes, ServerPlayers* server_players): Inventory(items, recipes), server_players(server_players) {}
    ServerInventory(Items *items, Recipes *recipes, const ServerInventory &inventory);
};
