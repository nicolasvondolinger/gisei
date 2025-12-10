#pragma once
#include "UIScreen.h"

class YouWin : public UIScreen {
public:
    YouWin(class Game* game, const std::string& fontName);
    void HandleKeyPress(int key) override;
};
