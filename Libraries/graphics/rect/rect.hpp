#pragma once
#include "orientedObject.hpp"
#include "timer.hpp"

namespace gfx {

class Rect : public _OrientedObject {
    using _OrientedObject::x;
    using _OrientedObject::y;
    using _OrientedObject::w;
    using _OrientedObject::h;
    
    int target_x = 0, target_y = 0, target_width = 1, target_height = 1;
    Timer approach_timer;
    int ms_counter = 0;
    
    bool first_time = true;
public:
    int getWidth() const;
    void setWidth(int width_);
    
    int getHeight() const;
    void setHeight(int height_);
    
    int getX() const;
    void setX(int x_);
    
    int getY() const;
    void setY(int y_);
    
    int getTargetX() const;
    int getTargetY() const;
    
    void jumpToTarget();
    
    int smooth_factor = 1;
    float blur_radius = 0;
    unsigned char shadow_intensity = 0;
    
    Color fill_color = {0, 0, 0, 0};
    Color border_color = {0, 0, 0, 0};
    
    void render();
};

};
