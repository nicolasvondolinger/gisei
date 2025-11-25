#include "ParallaxBackground.h"
#include "../Components/Drawing/ParallaxComponent.h"

ParallaxBackground::ParallaxBackground(class Game* game)
    : Actor(game) {
    
    mParallax = new ParallaxComponent(this, 10);
    
    mParallax->AddLayer("../Assets/Sprites/Battleground1/Pale/sky.png", 0.1f);
    mParallax->AddLayer("../Assets/Sprites/Battleground1/Pale/ruins_bg.png", 0.2f);
    mParallax->AddLayer("../Assets/Sprites/Battleground1/Pale/hills&trees.png", 0.3f);
    mParallax->AddLayer("../Assets/Sprites/Battleground1/Pale/ruins.png", 0.5f);
    mParallax->AddLayer("../Assets/Sprites/Battleground1/Pale/ruins2.png", 0.6f);
    mParallax->AddLayer("../Assets/Sprites/Battleground1/Pale/statue.png", 0.7f);
    mParallax->AddLayer("../Assets/Sprites/Battleground1/Pale/stones&grass.png", 0.9f);
}
