#include <cmath>
#include "exception.hpp"
#include "glfwAbstraction.hpp"

static const char* vertex_shader_code =
"#version 330 core\n"
"layout(location = 0) in vec2 vertex_position;"
"layout(location = 1) in vec4 vertex_color;"
"layout(location = 2) in vec2 vertex_uv;"
"out vec4 fragment_color;"
"out vec2 uv;"
"uniform int has_color_buffer;"
"uniform vec4 default_color;"
"uniform mat3 transform_matrix;"
"uniform mat3 texture_transform_matrix;"
"void main() {"
"   gl_Position = vec4(transform_matrix * vec3(vertex_position.xy, 1), 1);"
"   fragment_color = mix(default_color, vertex_color, has_color_buffer);"
"   uv = (texture_transform_matrix * vec3(vertex_uv, 1)).xy;"
"}";

static const char* fragment_shader_code =
"#version 330 core\n"
"in vec4 fragment_color;"
"in vec2 uv;"
"layout(location = 0) out vec4 color;"
"uniform sampler2D texture_sampler;"
"uniform int has_texture;"
"void main() {"
"   color = mix(vec4(1.f, 1.f, 1.f, 1.f), texture(texture_sampler, uv).rgba, has_texture) * fragment_color;"
"}";

static float global_scale = SYSTEM_SCALE;
static unsigned int default_framebuffer, window_width_min, window_height_min;
static float global_scale_x = 1, global_scale_y = 1;
static int system_scale_x = 1, system_scale_y = 1;
static GLFWwindow* glfw_window;

static gfx::Key translateKeyboardKey(int glfw_button) {
    switch (glfw_button) {
        case GLFW_KEY_A: return gfx::Key::A;
        case GLFW_KEY_B: return gfx::Key::B;
        case GLFW_KEY_C: return gfx::Key::C;
        case GLFW_KEY_D: return gfx::Key::D;
        case GLFW_KEY_E: return gfx::Key::E;
        case GLFW_KEY_F: return gfx::Key::F;
        case GLFW_KEY_G: return gfx::Key::G;
        case GLFW_KEY_H: return gfx::Key::H;
        case GLFW_KEY_I: return gfx::Key::I;
        case GLFW_KEY_J: return gfx::Key::J;
        case GLFW_KEY_K: return gfx::Key::K;
        case GLFW_KEY_L: return gfx::Key::L;
        case GLFW_KEY_M: return gfx::Key::M;
        case GLFW_KEY_N: return gfx::Key::N;
        case GLFW_KEY_O: return gfx::Key::O;
        case GLFW_KEY_P: return gfx::Key::P;
        case GLFW_KEY_Q: return gfx::Key::Q;
        case GLFW_KEY_R: return gfx::Key::R;
        case GLFW_KEY_S: return gfx::Key::S;
        case GLFW_KEY_T: return gfx::Key::T;
        case GLFW_KEY_U: return gfx::Key::U;
        case GLFW_KEY_V: return gfx::Key::V;
        case GLFW_KEY_W: return gfx::Key::W;
        case GLFW_KEY_X: return gfx::Key::X;
        case GLFW_KEY_Y: return gfx::Key::Y;
        case GLFW_KEY_Z: return gfx::Key::Z;
        case GLFW_KEY_0: return gfx::Key::NUM0;
        case GLFW_KEY_1: return gfx::Key::NUM1;
        case GLFW_KEY_2: return gfx::Key::NUM2;
        case GLFW_KEY_3: return gfx::Key::NUM3;
        case GLFW_KEY_4: return gfx::Key::NUM4;
        case GLFW_KEY_5: return gfx::Key::NUM5;
        case GLFW_KEY_6: return gfx::Key::NUM6;
        case GLFW_KEY_7: return gfx::Key::NUM7;
        case GLFW_KEY_8: return gfx::Key::NUM8;
        case GLFW_KEY_9: return gfx::Key::NUM9;
        case GLFW_KEY_SPACE: return gfx::Key::SPACE;
        case GLFW_KEY_ESCAPE: return gfx::Key::ESCAPE;
        case GLFW_KEY_ENTER: return gfx::Key::ENTER;
        case GLFW_KEY_LEFT_SHIFT: case GLFW_KEY_RIGHT_SHIFT: return gfx::Key::SHIFT;
        case GLFW_KEY_BACKSPACE: return gfx::Key::BACKSPACE;
        case GLFW_KEY_LEFT_CONTROL: case GLFW_KEY_RIGHT_CONTROL: return gfx::Key::CTRL;
        case GLFW_KEY_RIGHT: return gfx::Key::ARROW_RIGHT;
        case GLFW_KEY_LEFT: return gfx::Key::ARROW_LEFT;
        case GLFW_KEY_UP: return gfx::Key::ARROW_UP;
        case GLFW_KEY_DOWN: return gfx::Key::ARROW_DOWN;
        default: return gfx::Key::UNKNOWN;
    }
}

static gfx::Key translateMouseKey(int glfw_button) {
    switch(glfw_button) {
        case GLFW_MOUSE_BUTTON_LEFT: return gfx::Key::MOUSE_LEFT;
        case GLFW_MOUSE_BUTTON_RIGHT: return gfx::Key::MOUSE_RIGHT;
        case GLFW_MOUSE_BUTTON_MIDDLE: return gfx::Key::MOUSE_MIDDLE;
        default: return gfx::Key::UNKNOWN;
    }
}

static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    using namespace gfx;
    
    normalization_transform = _Transformation();
    normalization_transform.stretch(2 / (float)getWindowWidth(), -2 / (float)getWindowHeight());
    normalization_transform.translate(-(float)getWindowWidth() / 2, -(float)getWindowHeight() / 2);
    
    glBindTexture(GL_TEXTURE_2D, window_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, getWindowWidth() * global_scale_x, getWindowHeight() * global_scale_y, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    glBindTexture(GL_TEXTURE_2D, window_texture_back);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, getWindowWidth() * global_scale_x, getWindowHeight() * global_scale_y, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    glViewport(0, 0, getWindowWidth() * global_scale_x, getWindowHeight() * global_scale_y);
    
    _ScreenRefreshEvent event;
    screen_refresh_event_sender.call(event);
}

static void windowContentScaleCallback(GLFWwindow* window, float scale_x, float scale_y) {
    using namespace gfx;
    
    system_scale_x = std::round(scale_x);
    system_scale_y = std::round(scale_y);
    
    if(global_scale == SYSTEM_SCALE) {
        global_scale_x = system_scale_x;
        global_scale_y = system_scale_y;
    } else {
        global_scale_x = global_scale;
        global_scale_y = global_scale;
    }
    
    framebufferSizeCallback(glfw_window, 0, 0);
    
    setMinimumWindowSize(window_width_min, window_height_min);
}

static void windowFocusCallback(GLFWwindow* window, int focused) {
    using namespace gfx;
    
    is_window_focused = focused;
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    using namespace gfx;
    
    if(action == GLFW_PRESS) {
        _KeyPressEvent event(translateKeyboardKey(key));
        key_press_event_sender.call(event);
    } else if(action == GLFW_RELEASE) {
        _KeyReleaseEvent event(translateKeyboardKey(key));
        key_release_event_sender.call(event);
    }
}

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    using namespace gfx;
    
    if(action == GLFW_PRESS) {
        _KeyPressEvent event(translateMouseKey(button));
        key_press_event_sender.call(event);
    } else if(action == GLFW_RELEASE) {
        _KeyReleaseEvent event(translateMouseKey(button));
        key_release_event_sender.call(event);
    }
}

static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    using namespace gfx;
    
    _ScrollEvent event(yoffset);
    scroll_event_sender.call(event);
}

static void characterCallback(GLFWwindow* window, unsigned int codepoint) {
    using namespace gfx;
    
    _CharInputEvent event(codepoint);
    char_input_event_sender.call(event);
}

bool getKeyState(gfx::Key key);
bool getAbsoluteKeyState(gfx::Key key);

void gfx::setMinimumWindowSize(int width, int height) {
    window_width_min = width;
    window_height_min = height;
    
#ifdef __APPLE__
    glfwSetWindowSizeLimits(glfw_window, width / system_scale_x * global_scale_x, height / system_scale_y * global_scale_y, -1, -1);
#else
    glfwSetWindowSizeLimits(glfw_window, width * global_scale_x * system_scale_x, height * global_scale_y * system_scale_y, -1, -1);
#endif
}

void gfx::initGlfw(int window_width_, int window_height_, const std::string& window_title) {
    if(!glfwInit())
        throw Exception("Failed to initialize GLFW");

    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfw_window = glfwCreateWindow(window_width_, window_height_, window_title.c_str(), nullptr, nullptr);
    glfwSetFramebufferSizeCallback(glfw_window, framebufferSizeCallback);
    glfwSetWindowContentScaleCallback(glfw_window, windowContentScaleCallback);
    glfwSetKeyCallback(glfw_window, keyCallback);
    glfwSetScrollCallback(glfw_window, scrollCallback);
    glfwSetCharCallback(glfw_window, characterCallback);
    glfwSetMouseButtonCallback(glfw_window, mouseButtonCallback);
    glfwSetWindowFocusCallback(glfw_window, windowFocusCallback);

    if(!glfw_window)
        throw Exception("Failed to open GLFW window.");

    glfwMakeContextCurrent(glfw_window);
    
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        throw Exception("Failed to initialize OpenGL context");
    
    unsigned int vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    glfwSetInputMode(glfw_window, GLFW_STICKY_KEYS, GL_TRUE);

    shader_program = compileShaders(vertex_shader_code, fragment_shader_code);
    uniform_has_texture = glGetUniformLocation(shader_program, "has_texture");
    uniform_default_color = glGetUniformLocation(shader_program, "default_color");
    uniform_texture_sampler = glGetUniformLocation(shader_program, "texture_sampler");
    uniform_has_color_buffer = glGetUniformLocation(shader_program, "has_color_buffer");
    uniform_transform_matrix = glGetUniformLocation(shader_program, "transform_matrix");
    uniform_texture_transform_matrix = glGetUniformLocation(shader_program, "texture_transform_matrix");

    glEnable(GL_BLEND);
    setBlendMode(BlendMode::BLEND_ALPHA);
    
    glUseProgram(shader_program);
    
    glGenTextures(1, &window_texture);
    glGenTextures(1, &window_texture_back);
    glGenFramebuffers(1, &default_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, default_framebuffer);

    float scale_x, scale_y;
    glfwGetWindowContentScale(glfw_window, &scale_x, &scale_y);
    windowContentScaleCallback(glfw_window, scale_x, scale_y);

    GLfloat rect_outline_vertex_array[] = {
        0.f, 0.f,
        1.f, 0.f,
        1.f, 1.f,
        0.f, 1.f,
    };
    
    glGenBuffers(1, &rect_outline_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, rect_outline_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect_outline_vertex_array), rect_outline_vertex_array, GL_STATIC_DRAW);
    
    GLfloat rect_vertex_array[] = {
        0.f, 0.f,
        1.f, 0.f,
        0.f, 1.f,
        1.f, 0.f,
        0.f, 1.f,
        1.f, 1.f,
    };
    
    glGenBuffers(1, &rect_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, rect_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect_vertex_array), rect_vertex_array, GL_STATIC_DRAW);
    
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);
    
    glEnableVertexAttribArray(SHADER_VERTEX_BUFFER);
}

void gfx::setGlobalScale(float scale) {
    global_scale = scale;
    windowContentScaleCallback(glfw_window, system_scale_x, system_scale_y);
}

unsigned int gfx::compileShaders(const char* vertex_code, const char* fragment_code) {
    GLuint vertex_id = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_id = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertex_id, 1, &vertex_code, nullptr);
    glCompileShader(vertex_id);

    glShaderSource(fragment_id, 1, &fragment_code, nullptr);
    glCompileShader(fragment_id);

    GLuint program_id = glCreateProgram();

    glAttachShader(program_id, vertex_id);
    glAttachShader(program_id, fragment_id);
    glLinkProgram(program_id);

    glDetachShader(program_id, vertex_id);
    glDetachShader(program_id, fragment_id);

    glDeleteShader(vertex_id);
    glDeleteShader(fragment_id);

    return program_id;
}

void gfx::enableVsync(bool enabled) {
    if(enabled)
        glfwSwapInterval(1);
    else
        glfwSwapInterval(0);
}

int gfx::getWindowWidth() {
    int width;
    glfwGetWindowSize(glfw_window, &width, nullptr);
#ifdef __APPLE__
    if(system_scale_x == 1)
        return width / global_scale_x;
    else
        return width / global_scale_x * 2;
#else
    return width / global_scale_x;
#endif
}

int gfx::getWindowHeight() {
    int height;
    glfwGetWindowSize(glfw_window, nullptr, &height);
#ifdef __APPLE__
    if(system_scale_y == 1)
        return height / global_scale_y;
    else
        return height / global_scale_y * 2;
#else
    return height / global_scale_y;
#endif
}

void gfx::updateWindow() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, default_framebuffer);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, window_texture, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    
    glBlitFramebuffer(0, 0, getWindowWidth() * global_scale_x, getWindowHeight() * global_scale_y, 0, 0, getWindowWidth() * global_scale_x, getWindowHeight() * global_scale_y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    
    glfwSwapBuffers(glfw_window);
    
    glBindFramebuffer(GL_FRAMEBUFFER, default_framebuffer);
}

void gfx::quitGlfw() {
    glfwTerminate();
}

std::string gfx::getClipboard() {
    return glfwGetClipboardString(glfw_window);
}

void gfx::setClipboard(const std::string& data) {
    glfwSetClipboardString(glfw_window, data.c_str());
}

int gfx::getMouseX() {
    double mouse_x_normalized;
    glfwGetCursorPos(glfw_window, &mouse_x_normalized, nullptr);
#ifdef __APPLE__
    return mouse_x_normalized * system_scale_x / global_scale_x;
#else
    return mouse_x_normalized / global_scale_x;
#endif
}

int gfx::getMouseY() {
    double mouse_y_normalized;
    glfwGetCursorPos(glfw_window, nullptr, &mouse_y_normalized);
#ifdef __APPLE__
    return mouse_y_normalized * system_scale_y / global_scale_y;
#else
    return mouse_y_normalized / global_scale_y;
#endif
}

bool gfx::isWindowClosed() {
    return glfwWindowShouldClose(glfw_window) != 0;
}

void gfx::setBlendMode(BlendMode blend_mode) {
    switch (blend_mode) {
        case BlendMode::BLEND_ALPHA:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
            
        case BlendMode::BLEND_MULTIPLY:
            glBlendFunc(GL_DST_COLOR, GL_ZERO);
            break;
    }
}
