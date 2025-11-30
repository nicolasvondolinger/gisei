#pragma once

#include "UIScreen.h"

class UIButton;
class UIText; // ADICIONE ISSO (Forward Declaration)

class PauseMenu : public UIScreen {
public:
    PauseMenu(class Game* game, const std::string& fontName);

    void HandleKeyPress(int key) override;
    void OnLanguageChanged() override;

private:
    void ChangeSelection(int newIndex);
    void ActivateSelection();

    // ADICIONE O TÍTULO AQUI
    UIText* mTitle;

    // Seus botões já existentes
    UIButton* mResumeButton;
    UIButton* mSettingsButton;
    UIButton* mControlsButton;
    UIButton* mMainMenuButton;
    UIButton* mQuitButton;
};