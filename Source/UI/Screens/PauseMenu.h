#pragma once

#include "UIScreen.h"

class UIButton;

class PauseMenu : public UIScreen {
public:
    PauseMenu(class Game* game, const std::string& fontName);

    void HandleKeyPress(int key) override;

private:
    void ChangeSelection(int newIndex);
    void ActivateSelection();

    UIButton* mResumeButton;
    UIButton* mSettingsButton;
    UIButton* mMainMenuButton;
    UIButton* mQuitButton;
};
