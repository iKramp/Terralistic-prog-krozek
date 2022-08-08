#pragma once
#include "sprite.hpp"
#include "timer.hpp"
#include "theme.hpp"

namespace gfx {

enum class Key {MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9, SPACE, ESCAPE, ENTER, SHIFT, BACKSPACE, CTRL, ARROW_UP, ARROW_DOWN, ARROW_LEFT, ARROW_RIGHT, UNKNOWN};

class Button : public Texture, public _OrientedObject {
    gfx::Timer timer;
    int timer_counter = 0;
    float scale = 1;
    int margin = GFX_DEFAULT_BUTTON_MARGIN;
    
    using gfx::_OrientedObject::w;
    using gfx::_OrientedObject::h;
    
    void updateSize();
public:
    Color def_color = GFX_DEFAULT_BUTTON_COLOR, def_border_color = GFX_DEFAULT_BUTTON_BORDER_COLOR, hover_color = GFX_DEFAULT_HOVERED_BUTTON_COLOR, border_hover_color = GFX_DEFAULT_HOVERED_BUTTON_BORDER_COLOR;
    bool isHovered(int mouse_x, int mouse_y) const;
    bool disabled = false;
    float hover_progress = 0;
    void render(int mouse_x, int mouse_y);
    
    void loadFromSurface(const Surface& surface);
    
    void setScale(float scale);
    float getScale() const;
    
    void setMargin(int margin);
    int getMargin() const;
    
    int getWidth() const;
    int getHeight() const;
};

};

#ifndef GRAPHICS_PUBLIC

namespace gfx {

inline bool key_states[(int)gfx::Key::UNKNOWN];
inline bool absolute_key_states[(int)gfx::Key::UNKNOWN];

}

#endif
