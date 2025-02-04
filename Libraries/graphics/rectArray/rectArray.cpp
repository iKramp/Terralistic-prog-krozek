#include "rectArray.hpp"
#include "glfwAbstraction.hpp"
#include "exception.hpp"

gfx::RectArray::~RectArray() {
    if(vertex_buffer != -1) {
        glDeleteBuffers(1, &vertex_buffer);
        glDeleteBuffers(1, &color_buffer);
        glDeleteBuffers(1, &texture_pos_buffer);
    }
}

void gfx::RectArray::setVertex(int index, int x, int y) {
    vertex_array[index * 2] = (float)x;
    vertex_array[index * 2 + 1] = (float)y;
}

void gfx::RectArray::setVertexColor(int index, Color color) {
    color_array[index * 4] = color.r * (1 / 255.f);
    color_array[index * 4 + 1] = color.g * (1 / 255.f);
    color_array[index * 4 + 2] = color.b * (1 / 255.f);
    color_array[index * 4 + 3] = color.a * (1 / 255.f);
}

void gfx::RectArray::setVertexTextureCoord(int index, int x, int y) {
    texture_pos_array[index * 2] = x;
    texture_pos_array[index * 2 + 1] = y;
}

void gfx::RectArray::setRect(int index, RectShape rect) {
    if(index < 0 || index >= length)
        throw Exception("Rect index must be positive and in bounds.");
    
    int x1 = rect.x, y1 = rect.y, x2 = rect.x + rect.w, y2 = rect.y + rect.h;
    setVertex(index * 6, x1, y1);
    setVertex(index * 6 + 1, x2, y1);
    setVertex(index * 6 + 2, x1, y2);
    setVertex(index * 6 + 3, x2, y1);
    setVertex(index * 6 + 4, x1, y2);
    setVertex(index * 6 + 5, x2, y2);
    
    update_vertex = true;
}

void gfx::RectArray::setColor(int index, Color color) {
    setColor(index, color, color, color, color);
}

void gfx::RectArray::setColor(int index, Color color1, Color color2, Color color3, Color color4) {
    if(index < 0 || index >= length)
        throw Exception("Color index must be positive and in bounds.");
    
    setVertexColor(index * 6, color1);
    setVertexColor(index * 6 + 1, color2);
    setVertexColor(index * 6 + 2, color3);
    setVertexColor(index * 6 + 3, color2);
    setVertexColor(index * 6 + 4, color3);
    setVertexColor(index * 6 + 5, color4);
    
    update_color = true;
}

void gfx::RectArray::setTextureCoords(int index, RectShape texture_coordinates) {
    if(index < 0 || index >= length)
        throw Exception("Texture coord index must be positive and in bounds.");
    
    int x1 = texture_coordinates.x, y1 = texture_coordinates.y, x2 = texture_coordinates.x + texture_coordinates.w, y2 = texture_coordinates.y + texture_coordinates.h;
    setVertexTextureCoord(index * 6, x1, y1);
    setVertexTextureCoord(index * 6 + 1, x2, y1);
    setVertexTextureCoord(index * 6 + 2, x1, y2);
    setVertexTextureCoord(index * 6 + 3, x2, y1);
    setVertexTextureCoord(index * 6 + 4, x1, y2);
    setVertexTextureCoord(index * 6 + 5, x2, y2);
    
    update_texture_vertex = true;
}

void gfx::RectArray::resize(int size) {
    if(size < 0)
        throw Exception("RectArray size must be positive.");
    
    vertex_array.resize(size * 6 * 2);
    color_array.resize(size * 6 * 4);
    texture_pos_array.resize(size * 6 * 2);
    length = size;
}

void gfx::RectArray::render(const Texture* image, int x, int y, int num_rects) {
    if(vertex_buffer == -1) {
        glGenBuffers(1, &vertex_buffer);
        glGenBuffers(1, &color_buffer);
        glGenBuffers(1, &texture_pos_buffer);
    }
    
    if(num_rects == -1)
        num_rects = length;

    glEnableVertexAttribArray(SHADER_COLOR_BUFFER);
    
    glUniform1i(uniform_texture_sampler, 0);
    glUniform1i(uniform_has_color_buffer, 1);
    
    _Transformation transform = normalization_transform;
    transform.translate(x, y);
    glUniformMatrix3fv(uniform_transform_matrix, 1, GL_FALSE, transform.getArray());
    
    if(update_vertex) {
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, vertex_array.size() * sizeof(float), &vertex_array[0], GL_STATIC_DRAW);
        
        update_vertex = false;
    }
    
    if(update_color) {
        glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
        glBufferData(GL_ARRAY_BUFFER, color_array.size() * sizeof(float), &color_array[0], GL_STATIC_DRAW);
        
        update_color = false;
    }
    
    if(update_texture_vertex) {
        glBindBuffer(GL_ARRAY_BUFFER, texture_pos_buffer);
        glBufferData(GL_ARRAY_BUFFER, texture_pos_array.size() * sizeof(float), &texture_pos_array[0], GL_STATIC_DRAW);
        
        update_texture_vertex = false;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glVertexAttribPointer(SHADER_VERTEX_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glVertexAttribPointer(SHADER_COLOR_BUFFER, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    
    if(image == nullptr)
        glUniform1i(uniform_has_texture, 0);
    else {
        glEnableVertexAttribArray(SHADER_TEXTURE_COORD_BUFFER);
        
        glUniform1i(uniform_has_texture, 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, image->_getGlTexture());
        
        glBindBuffer(GL_ARRAY_BUFFER, texture_pos_buffer);
        glVertexAttribPointer(SHADER_TEXTURE_COORD_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        
        glUniformMatrix3fv(uniform_texture_transform_matrix, 1, GL_FALSE, image->_getNormalizationTransform().getArray());
    }

    glDrawArrays(GL_TRIANGLES, 0, num_rects * 6);
    
    glDisableVertexAttribArray(SHADER_COLOR_BUFFER);
    if(image != nullptr)
        glDisableVertexAttribArray(SHADER_TEXTURE_COORD_BUFFER);
}
