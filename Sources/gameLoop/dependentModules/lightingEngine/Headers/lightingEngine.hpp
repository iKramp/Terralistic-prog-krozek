//
//  lightingEngine.hpp
//  Terralistic
//
//  Created by Jakob Zorz on 28/12/2020.
//

#ifndef lightingEngine_hpp
#define lightingEngine_hpp

#define MAX_LIGHT 100

namespace lightingEngine {

void init();
void prepareLights();

struct lightBlock {
    unsigned char level = 0;
    bool source = false;
    void render(unsigned short x, unsigned short y) const;
    void update(unsigned short x, unsigned short y, bool update=true);
};

void removeNaturalLight(unsigned short x);
void setNaturalLight(unsigned short x);

lightBlock& getLightBlock(unsigned short x, unsigned short y);

void addLightSource(unsigned short x, unsigned short y, unsigned char power);
void removeLightSource(unsigned short x, unsigned short y);

}

#endif /* lightingEngine_hpp */
