#include "console.hpp"
#include <set>
#include <platform_folders.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include "configManager.hpp"

static const std::set<char> allowed_chars = {'!', ':', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '{', '}', '"', '|', '~', '<', '>', '?', '-', '=', ',', '.', '/', '[', ']', ';', '\'', '\\', '`', ' '};

Console::Console(std::string resource_path): LauncherModule("console", std::move(resource_path)) {
    min_width = 300;
    min_height = 90;

    auto t = std::time(nullptr);
    auto tm = *localtime(&t);
    std::stringstream timestamped_text;
    timestamped_text << std::put_time(&tm, "log_@%Y.%m.%d_%H:%M:%S.txt");
    log_file_name = timestamped_text.str();
    ConfigFile file(sago::getDataHome() + "/Terralistic-Server/ServerSettings/console.config");
    file.setDefaultStr("enable_log", "false");
    enable_log = file.getStr("enable_log") == "true";
}

void Console::init() {
    input_box.setScale(1.8);//smaller than this and the text texture becomes funky when you type
    input_box.textProcessing = [](char c, int length) {
        if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || allowed_chars.find(c) != allowed_chars.end())
            return c;
        return '\0';
    };

    input_box.parent_containter = &base_container;
    input_box.orientation = gfx::BOTTOM_LEFT;
    input_box.x = 10;
    input_box.y = -10;
    input_box.def_color.a = 255;
    input_box.setBlurIntensity(1);
    input_box.setBorderColor(BORDER_COLOR);
    input_box.setPassthroughKeys({gfx::Key::ARROW_UP, gfx::Key::ARROW_DOWN});
    text_inputs = {&input_box};
    server->getPrint()->print_event.addListener(this);
}



void Console::render() {
    if(!enabled)
        return;

    input_box.width = -10 + (int)(((float)base_container.w - 10) / input_box.getScale());
    input_box.setWidth(input_box.width);

    for(auto & chat_line : chat_lines)
        chat_line->text_sprite.y += (chat_line->y_to_be - chat_line->text_sprite.y) / 2;

    gfx::RectShape(base_container.x + 2, base_container.y + 2, base_container.w - 4, base_container.h - 4).render(GREY);
    input_box.render(getMouseX(), getMouseY(), getMouseVel());


    for(auto & chat_line : chat_lines) {
        if(!chat_line->text.empty()) {
            chat_line->text_sprite.setScale(1);
            chat_line->text_sprite.orientation = gfx::BOTTOM_LEFT;
            chat_line->text_sprite.parent_containter = &base_container;
            chat_line->text_sprite.loadFromSurface(gfx::textToSurface(chat_line->text));
            chat_line->text_sprite.y = -10;
            chat_line->text_sprite.x = 10;
            chat_line->y_to_be = (int)input_box.y - input_box.getHeight() - 5;
            chat_line->text.clear();
            chat_line->text.shrink_to_fit();

            for(auto & i2 : chat_lines)
                if(i2 != chat_line)
                    i2->y_to_be -= chat_line->text_sprite.getHeight();
        }
        chat_line->text_sprite.render();
    }

    if(!chat_lines.empty())
        if(chat_lines[0]->text_sprite.y < -1 * base_container.h + 17)
            chat_lines.erase(chat_lines.begin());

}

bool Console::onKeyDown(gfx::Key key) {
    if(key == gfx::Key::ENTER && input_box.active) {
        if(!input_box.getText().empty()) {
            server->getPrint()->info("[Server] " + input_box.getText());
            if(input_box.getText().substr(0, 2) == "//"){//when more // commands get added code inside this block will be moved to its own sub-function
                std::string command = input_box.getText();
                command.erase(0, 2);
                if(command.substr(0, 13) == "module_config"){
                    module_manager->moduleConfig(command);
                }
            }else {
                std::string command = input_box.getText();
                if(command.at(0) != '/')
                    command.insert(command.begin(), '/');
                ServerChatEvent event(nullptr, command);
                server->getChat()->chat_event.call(event);
            }
            saved_lines.insert(saved_lines.begin() + 1, input_box.getText());
            selected_saved_line = 0;
            if (saved_lines.size() > 20)
                saved_lines.erase(saved_lines.end());
            input_box.setText("");
        }
        return true;
    } else if(key == gfx::Key::ESCAPE && input_box.active) {
        input_box.setText("");
        selected_saved_line = 0;
        input_box.active = false;
        return true;
    }
    if(key == gfx::Key::ARROW_UP && input_box.active) {
        if(selected_saved_line < saved_lines.size() - 1) {
            selected_saved_line++;
            input_box.setText(saved_lines[selected_saved_line]);
            input_box.setCursor((int)input_box.getText().size());
        }
        return true;
    } else if(key == gfx::Key::ARROW_DOWN && input_box.active) {
        if(selected_saved_line > 0){
            selected_saved_line--;
            input_box.setText(saved_lines[selected_saved_line]);
            input_box.setCursor((int)input_box.getText().size());
        }
        return true;
    }
    return false;
}

void Console::onEvent(PrintEvent &event) {
    while(chat_lines.size() > 100)
        chat_lines.erase(chat_lines.begin());

    std::string curr_line, whole_message;
    whole_message = event.message;
    if(!whole_message.ends_with("\n"))
        whole_message.push_back('\n');
    if(enable_log)
        saveToLog(whole_message);
    while(!whole_message.empty()) {
        curr_line.push_back(whole_message[0]);
        whole_message.erase(whole_message.begin());
        if(whole_message[0] == '\n') {
            auto* new_line = new ChatLine;
            new_line->text = curr_line;
            chat_lines.push_back(new_line);
            whole_message.erase(whole_message.begin());
            curr_line = "";

            auto line = chat_lines[chat_lines.size() - 1];
            if(event.type == MessageType::WARNING)
                line->text_sprite.setColor({255, 255, 0});
            else if(event.type == MessageType::ERROR)
                line->text_sprite.setColor({255, 0, 0});
        }
    }
}

void Console::saveToLog(const std::string& line) {
    std::ofstream file;
    file.open(sago::getDataHome() + "/Terralistic-Server/serverLogFiles/" + log_file_name, std::ios::out | std::ios::app);
    file << line;
    file.close();
}


void Console::stop() {
    server->getPrint()->print_event.removeListener(this);
}
