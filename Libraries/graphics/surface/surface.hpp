#pragma once
#include <vector>
#include "color.hpp"
#include "exception.hpp"

namespace gfx {

class Surface {
    std::vector<unsigned char> data;
    int width = 0, height = 0;
public:
    void loadFromBuffer(const std::vector<unsigned char>& buffer, int w, int h);
    void createEmpty(int w, int h);
    
    Color getPixel(int x, int y) const;
    void setPixel(int x, int y, Color color);
    
    const std::vector<unsigned char>& getData() const;
    int getWidth() const;
    int getHeight() const;
};

}

EXCEPTION_TYPE(BufferSizeError)
EXCEPTION_TYPE(PixelOutOfBoundsError)
