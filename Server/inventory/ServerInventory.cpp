#include "ServerInventory.hpp"

ServerInventory::ServerInventory(Items *items, Recipes *recipes, const ServerInventory &inventory) : Inventory(items, recipes) {
  *this = inventory;
}