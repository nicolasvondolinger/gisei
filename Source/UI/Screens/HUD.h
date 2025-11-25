#pragma once

#include <string>

#include "UIScreen.h"

class HUD : public UIScreen {
public:
    HUD(class Game *game, const std::string &fontName);

    void SetHealth(int health);

    void SetScore(int score);

private:
    UIImage *mHealth1;
    UIImage *mHealth2;
    UIImage *mHealth3;
    UIText *mScore;
};