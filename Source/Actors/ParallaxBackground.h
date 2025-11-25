#pragma once
#include "Actor.h"

class ParallaxBackground : public Actor {
public:
    ParallaxBackground(class Game* game);

private:
    class ParallaxComponent* mParallax;
};
