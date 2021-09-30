#include <cassert>
#include <cmath>
#include "entities.hpp"

void Entities::updateAllEntities(float frame_length) {
    for(Entity* entity : entities)
        entity->updateEntity(blocks, frame_length);
}

const std::vector<Entity*>& Entities::getEntities() {
    return entities;
}

void Entities::registerEntity(Entity* entity) {
    entities.push_back(entity);
}

void Entities::removeEntity(Entity* entity) {
    entities.erase(std::find(entities.begin(), entities.end(), entity));
}

Entity* Entities::getEntityById(unsigned short id) {
    for(Entity* entity : entities)
        if(entity->id == id)
            return entity;
    assert(false);
    return nullptr;
}

void Entity::updateEntity(Blocks *blocks, float frame_length) {
    if(friction) {
        velocity_y *= std::pow(0.995f, frame_length);
        velocity_x *= std::pow(isTouchingGround(blocks) ? 0.99f : 0.9995f, frame_length);
    }
    
    if(gravity)
        velocity_y += frame_length / 5.f;
    
    float y_to_be = y + float(velocity_y * frame_length) / 100;
    float move_y = y_to_be - y;
    int y_factor = move_y > 0 ? 1 : -1;
    for(int i = 0; i < std::abs(move_y); i++) {
        y += y_factor;
        if(isColliding(blocks)) {
            y -= y_factor;
            velocity_y = 0;
            break;
        }
    }
    if(velocity_y)
        y = y_to_be;
    
    float prev_x = x;
    float x_to_be = x + float(velocity_x * frame_length) / 100;
    float move_x = x_to_be - x;
    int x_factor = move_x > 0 ? 1 : -1;
    bool has_collided_x = false;
    for(int i = 0; i < std::abs(move_x); i++) {
        x += x_factor;
        if(isColliding(blocks)) {
            x -= x_factor;
            has_collided_x = true;
            break;
        }
    }
    if(!has_collided_x)
        x = x_to_be;
    has_moved_x = prev_x != x;
}

bool Entity::isTouchingGround(Blocks* blocks) {
    return isCollidingWithBlocks(blocks, x, y + 1) && velocity_y == 0;
}

bool Entity::isCollidingWithBlocks(Blocks* blocks) {
    return isCollidingWithBlocks(blocks, x, y);
}

bool Entity::isCollidingWithBlocks(Blocks* blocks, float colliding_x, float colliding_y) {
    if(colliding_x < 0 || colliding_y < 0 ||
       colliding_y >= blocks->getHeight() * BLOCK_WIDTH * 2 - getHeight() ||
       colliding_x >= blocks->getWidth() * BLOCK_WIDTH * 2 - getWidth())
        return true;

    unsigned short starting_x = colliding_x / (BLOCK_WIDTH * 2);
    unsigned short starting_y = colliding_y / (BLOCK_WIDTH * 2);
    unsigned short ending_x = (colliding_x + getWidth() - 1) / (BLOCK_WIDTH * 2);
    unsigned short ending_y = (colliding_y + getHeight() - 1) / (BLOCK_WIDTH * 2);
    
    for(unsigned short x_ = starting_x; x_ <= ending_x; x_++)
        for(unsigned short y_ = starting_y; y_ <= ending_y; y_++)
            if(!blocks->getBlockInfo(x_, y_).ghost)
                return true;
    
    return false;
}
