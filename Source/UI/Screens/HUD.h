#pragma once
#include <string>
#include "UIScreen.h"
#include "../UIRect.h"

class HUD : public UIScreen {
public:
    HUD(class Game *game, const std::string &fontName);

    void UpdateHUD(class Ninja* ninja);

private:
    UIRect* mHealthBarBG;
    UIRect* mEnergyBarBG;

    UIRect* mHealthBarFG;
    UIRect* mEnergyBarFG;

    // Configurações Visuais
    const float mLeftMargin = 100.0f; // Empurra tudo para a direita (antes era 60)
    
    const float mHealthMaxWidth = 200.0f; // Largura da Vida
    const float mEnergyMaxWidth = 140.0f; // Largura da Energia (Menor)
    
    const float mBarHeight = 20.0f;
};