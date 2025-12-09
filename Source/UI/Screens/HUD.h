#pragma once
#include <string>
#include "UIScreen.h"
#include "../UIRect.h"

class KarasuTengu;

class HUD : public UIScreen {
public:
    HUD(class Game *game, const std::string &fontName);

    void UpdateHUD(class Ninja* ninja, class KarasuTengu* boss);

private:
    UIRect* mHealthBarBG;
    UIRect* mEnergyBarBG;

    UIRect* mHealthBarFG;
    UIRect* mEnergyBarFG;

    UIRect* mBossBarBG;
    UIRect* mBossBarFG;
    class UIText* mBossNameText;
    std::string mCurrentBossName;

    // Configurações Visuais
    const float mLeftMargin = 100.0f; // Empurra tudo para a direita (antes era 60)
    
    const float mHealthMaxWidth = 200.0f; // Largura da Vida
    const float mEnergyMaxWidth = 140.0f; // Largura da Energia (Menor)

    const float mBossBarWidth = 380.0f;
    const float mBossBarHeight = 18.0f;
    const float mBossBarY = 90.0f;
    const float mBossNameY = 60.0f;
    float mBossBarLeft;
    float mBossBarCenterX;

    const float mBarHeight = 20.0f;
};
