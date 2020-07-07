//
//  main.cpp
//  Terralistic
//
//  Created by Jakob Zorz on 06/07/2020.
//

#include "startMenu.hpp"
#include "singleWindowLibrary.hpp"
#include "UIKit.hpp"
#include "framerateRegulator.hpp"

int startMenu::main() {
    bool running = true;
    SDL_Event event;
    
    std::cout << sizeof(ogl::texture) << std::endl;
    
    ui::button play_button;
    play_button.setY(-50);
    play_button.setColor(0, 0, 0);
    play_button.setHoverColor(100, 100, 100);
    play_button.setTextColor(255, 255, 255);
    play_button.setScale(3);
    play_button.setText("Play");
    
    ui::button exit_button;
    exit_button.setY(50);
    exit_button.setColor(0, 0, 0);
    exit_button.setHoverColor(100, 100, 100);
    exit_button.setTextColor(255, 255, 255);
    exit_button.setScale(3);
    exit_button.setText("Exit");
    
    while(running) {
        framerateRegulator::regulateFramerate();
        while(SDL_PollEvent(&event)) {
            if(swl::handleBasicEvents(event, &running));
            else if(play_button.isPressed(event))
                return 1;
            else if(exit_button.isPressed(event))
                return 0;
        }
        
        swl::setDrawColor(0, 0, 0);
        swl::clear();
        
        play_button.render();
        exit_button.render();
        
        swl::update();
    }
    return 0;
}
