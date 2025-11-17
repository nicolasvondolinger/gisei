//
// Created by Lucas N. Ferreira on 06/11/25.
//

#include "MainMenu.h"
#include "../../Game.h"
#include "../UIButton.h"

MainMenu::MainMenu(class Game* game, const std::string& fontName)
        :UIScreen(game, fontName)
{
        Vector2 logoPos(0.0f, 150.0f);
        Vector2 startButtonPos(0.0f, 0.0f);
        Vector2 closeButtonPos(0.0f, -75.0f);

        Vector4 blue(0.0f, 0.0f, 1.0f, 1.0f);
        const Vector3 white(1.0f, 1.0f, 1.0f);

        AddImage("../Assets/Logo.png", logoPos, 0.75f);

        UIButton* startButton = AddButton("Start Game", [this]() {
            Close();
            mGame->SetScene(GameScene::Level1);
        }, startButtonPos);

        startButton->SetBackgroundColor(blue);
        startButton->SetTextColor(white);

        UIButton* closeButton = AddButton("Exit", [this]() {
            mGame->Quit();
        }, closeButtonPos);

        closeButton->SetBackgroundColor(blue);
        closeButton->SetTextColor(white);
}

void MainMenu::HandleKeyPress(int key)
{
    if (mButtons.empty())
    {
        return;
    }

    int currentSelection = mSelectedButtonIndex;
    switch (key) {
        case SDLK_DOWN:
            mButtons[currentSelection]->SetHighlighted(false);
            mSelectedButtonIndex = (currentSelection + 1) % mButtons.size();
            mButtons[mSelectedButtonIndex]->SetHighlighted(true);
            break;

        case SDLK_UP:
            mButtons[currentSelection]->SetHighlighted(false);
            mSelectedButtonIndex = (currentSelection - 1 + mButtons.size()) % mButtons.size();
            mButtons[mSelectedButtonIndex]->SetHighlighted(true);
            break;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < mButtons.size())
            {
                mButtons[mSelectedButtonIndex]->OnClick();
            }
            break;
        default:
            break;
    }
}