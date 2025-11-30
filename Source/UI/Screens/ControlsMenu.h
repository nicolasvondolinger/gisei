#pragma once

#include "UIScreen.h"

class UIButton;

class ControlsMenu : public UIScreen {
public:
    ControlsMenu(class Game* game, const std::string& fontName);
    ~ControlsMenu() override = default;

    void HandleKeyPress(int key) override;

private:
    UIButton* mBackButton;
};
