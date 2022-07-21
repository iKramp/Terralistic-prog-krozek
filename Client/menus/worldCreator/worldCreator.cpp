#include "worldCreator.hpp"
#include "game.hpp"

void WorldCreator::init() {
    back_button.scale = 3;
    back_button.loadFromText("Back");
    back_button.y = -SPACING;
    back_button.orientation = gfx::BOTTOM;
    
    new_world_title.loadFromText("Create new world:");
    new_world_title.scale = 3;
    new_world_title.y = SPACING;
    new_world_title.orientation = gfx::TOP;

    create_button.scale = 3;
    create_button.loadFromText("Create world");
    create_button.y = -SPACING;
    create_button.orientation = gfx::BOTTOM;

    back_button.x = (-create_button.getWidth() - back_button.getWidth() + back_button.getWidth() - SPACING) / 2;
    create_button.x = (create_button.getWidth() + back_button.getWidth() - create_button.getWidth() + SPACING) / 2;

    world_name.scale = 3;
    world_name.orientation = gfx::CENTER;
    world_name.setText("");
    world_name.active = true;
    world_name.textProcessing = [](char c, int length) {
        if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '_')
            return c;
        if(c == ' ')
            return '-';
        return '\0';
    };

    world_seed.scale = 3;
    world_seed.orientation = gfx::CENTER;
    world_seed.setText("");
    world_seed.active = false;
    world_seed.textProcessing = [](char c, int length) {
        if(c >= '0' && c <= '9')
            return c;
        return '\0';
    };

    world_name.y = - 16 - world_name.getHeight() / 2;
    world_seed.y = 16 + world_seed.getHeight() / 2;


    new_world_name.loadFromText("New world name");
    new_world_name.scale = 3;
    new_world_name.y = world_name.y;
    new_world_name.x = -world_name.getWidth() / 2 + new_world_name.getWidth() / 2 + 16;//with commenting out this line the text will go to the center, choice will be made later
    new_world_name.orientation = gfx::CENTER;

    new_world_seed.loadFromText("New world seed");
    new_world_seed.scale = 3;
    new_world_seed.y = world_seed.y;
    new_world_seed.x = -world_seed.getWidth() / 2 + new_world_seed.getWidth() / 2 + 16;//with commenting out this line the text will go to the center, choice will be made later
    new_world_seed.orientation = gfx::CENTER;


    world_name.def_color.a = TRANSPARENCY;
    world_seed.def_color.a = TRANSPARENCY;

    text_inputs = {&world_name, &world_seed};
}

bool WorldCreator::onKeyUp(gfx::Key key) {
    if(key == gfx::Key::MOUSE_LEFT && back_button.isHovered(getMouseX(), getMouseY())) {
        returnFromScene();
        return true;
    } else if((key == gfx::Key::MOUSE_LEFT && create_button.isHovered(getMouseX(), getMouseY())) || (key == gfx::Key::ENTER && can_create)) {
        int formed_seed = rand();
        if(!world_seed.getText().empty())
            formed_seed = std::stoi(world_seed.getText());
        startPrivateWorld(this, sago::getDataHome() + "/Terralistic/Worlds/" + world_name.getText() + ".world", menu_back, settings, formed_seed);
        returnFromScene();
        return true;
    }
    return false;
}

void WorldCreator::render() {
    menu_back->setBackWidth(world_name.getWidth() + 100);
    menu_back->renderBack();
    if(can_create != (!world_name.getText().empty() && !std::count(worlds.begin(), worlds.end(), world_name.getText()))) {
        can_create = !can_create;
        create_button.loadFromText("Create world", {(unsigned char)(can_create ? WHITE.r : GREY.r), (unsigned char)(can_create ? WHITE.g : GREY.g), (unsigned char)(can_create ? WHITE.b : GREY.b)});
        create_button.disabled = !can_create;
    }
    create_button.render(getMouseX(), getMouseY());
    back_button.render(getMouseX(), getMouseY());

    if(world_name.isHovered(getMouseX(), getMouseY()))
        new_world_name.setColor({GFX_DEFAULT_BUTTON_COLOR.r, GFX_DEFAULT_BUTTON_COLOR.g, GFX_DEFAULT_BUTTON_COLOR.b, TRANSPARENCY});
    else
        new_world_name.setColor(GREY);

    if(world_seed.isHovered(getMouseX(), getMouseY())) {
        new_world_seed.setColor({GFX_DEFAULT_BUTTON_COLOR.r, GFX_DEFAULT_BUTTON_COLOR.g, GFX_DEFAULT_BUTTON_COLOR.b, TRANSPARENCY});
    }
    else
        new_world_seed.setColor(GREY);

    new_world_title.render();
    world_name.render(getMouseX(), getMouseY());
    world_seed.render(getMouseX(), getMouseY());

    if(world_name.getText().empty())
        new_world_name.render();
    if(world_seed.getText().empty())
        new_world_seed.render();
}
