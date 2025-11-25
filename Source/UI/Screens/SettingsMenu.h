// SettingsMenu.h
#pragma once
#include <string>

#include "UIScreen.h"

class SettingsMenu : public UIScreen {
public:
    SettingsMenu(class Game* game, const std::string& fontName);
    ~SettingsMenu() override;

    void HandleKeyPress(int key) override;

    void UpdateVolumeText();
    void UpdateDifficultyText();
    void UpdateFullscreenText();
    void UpdateSelectColor();

private:
    int mVolumeLevel;
    int mDifficultyLevel;
    bool mIsFullscreen;

    class UIText* mVolumeText;
    class UIText* mFullscreenText;
    class UIText* mDifficultyText;
    class UIButton* mBackButton;
};