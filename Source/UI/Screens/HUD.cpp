//
// Created by Lucas N. Ferreira on 08/12/23.
//

#include "HUD.h"
#include "../../Game.h"
#include <string>

#include "../../Actors/Mario.h"

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
            mHealth1->SetIsVisible(true); // Laranja
            mHealth2->SetIsVisible(true); // Vermelho
            mHealth3->SetIsVisible(true); // Azul
            break;
        case 2:
            // Vida 2: Mostra Laranja e Vermelho
            mHealth1->SetIsVisible(true); // Laranja
            mHealth2->SetIsVisible(true); // Vermelho
            mHealth3->SetIsVisible(false);
            break;
        case 1:
            // Vida 1: Mostra Apenas Vermelho
            mHealth1->SetIsVisible(false);
            mHealth2->SetIsVisible(true); // Vermelho
            mHealth3->SetIsVisible(false);
            break;
        case 0:
        default:
            // Vida 0 ou padrão: Desativa todas
            mHealth1->SetIsVisible(false);
            mHealth2->SetIsVisible(false);
            mHealth3->SetIsVisible(false);
            break;
    }
}

void HUD::SetScore(int score)
{
    // Atualiza o texto do placar convertendo o 'int' para 'std::string'
    // (Presume que UIText tem um método SetText)
    mScore->SetText(std::to_string(score));
}