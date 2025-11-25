#include "HUD.h"
#include "../../Game.h"
#include <string>

#include "../../Actors/Ninja.h"

HUD::HUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
{
    mHealth1 = AddImage("../Assets/ShieldOrange.png", Vector2(0.75f));
    mHealth2 = AddImage("../Assets/ShieldRed.png", Vector2(0.75f));
    mHealth3 = AddImage("../Assets/ShieldBlue.png", Vector2(0.75f));
    AddImage("../Assets/ShieldBar.png", Vector2(0.75f));
    AddText("score: ", Vector2(0.75f));
    mScore = AddText("0", Vector2(0.75f));
}

void HUD::SetHealth(int health) {
    switch (health) {
        case 3:
            mHealth1->SetIsVisible(true); 
            mHealth2->SetIsVisible(true); 
            mHealth3->SetIsVisible(true); 
            break;
        case 2:
            
            mHealth1->SetIsVisible(true); 
            mHealth2->SetIsVisible(true); 
            mHealth3->SetIsVisible(false);
            break;
        case 1:
            
            mHealth1->SetIsVisible(false);
            mHealth2->SetIsVisible(true); 
            mHealth3->SetIsVisible(false);
            break;
        case 0:
        default:
            
            mHealth1->SetIsVisible(false);
            mHealth2->SetIsVisible(false);
            mHealth3->SetIsVisible(false);
            break;
    }
}

void HUD::SetScore(int score)
{
    
    
    mScore->SetText(std::to_string(score));
}