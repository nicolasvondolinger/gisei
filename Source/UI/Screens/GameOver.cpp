#include "GameOver.h"
#include "../../Game.h"

GameOver::GameOver(class Game* game, const std::string& fontName)
        :UIScreen(game, fontName)
{
        float width = static_cast<float>(mGame->WINDOW_WIDTH);
        float height = static_cast<float>(mGame->WINDOW_HEIGHT);
        float centerX = width / 2.0f;
        float centerY = height / 2.0f;

        const Vector3 titleColor(0.96f, 0.90f, 0.83f);
        const Vector3 buttonText(0.93f, 0.82f, 0.76f);
        const Vector3 buttonHover(0.66f, 0.80f, 0.70f); // quase branco
        const Vector4 buttonBg(0.18f, 0.05f, 0.05f, 0.82f); // vinho escuro translúcido
        const Vector4 overlay(0.02f, 0.02f, 0.02f, 0.55f);
        const Vector4 transparent(0.0f, 0.0f, 0.0f, 0.0f);

        AddImage("../Assets/UI/GameOverBackground.png", Vector2(centerX, centerY), 0.8f, 0.0f, 0);
        AddRect(Vector2(centerX, centerY), Vector2(width, height), 1.0f, 0.0f, 5)->SetColor(overlay);

        auto& lang = mGame->GetLanguage();

        UIText* title = AddText(lang.Get("gameover.title"), Vector2(centerX, centerY - 140.0f), 2.7f, 0.0f, 60, 900, 20);
        title->SetBackgroundColor(transparent);
        title->SetTextColor(titleColor);

        Vector2 retryPos(centerX, centerY + 20.0f);
        Vector2 menuPos(centerX, centerY + 90.0f);
        Vector2 quitPos(centerX, centerY + 160.0f);

        AddButton(lang.Get("gameover.retry"), [this]() {
                this->Close(); 
                mGame->SetScene(GameScene::Level1);
        }, retryPos, 1.0f, 0.0f, 46, 900, 30);
        AddButton(lang.Get("gameover.menu"), [this]() {
                this->Close();
                mGame->SetScene(GameScene::MainMenu); 
        }, menuPos, 1.0f, 0.0f, 46, 900, 31);
        AddButton(lang.Get("gameover.quit"), [this]() {
                this->Close(); 
                mGame->Quit();
        }, quitPos, 1.0f, 0.0f, 46, 900, 32);

        for (auto button : mButtons) {
                button->SetTextColor(buttonText);
                button->SetHoverColor(buttonHover);
                button->SetBackgroundColor(buttonBg);
        }
}

void GameOver::HandleKeyPress(int key) {
        if (mButtons.empty()) return;
    
        if(key == SDLK_DOWN || key == SDLK_s) {
                mButtons[mSelectedButtonIndex]->SetHighlighted(false);
                mSelectedButtonIndex++;
                mSelectedButtonIndex%=mButtons.size();
                mButtons[mSelectedButtonIndex]->SetHighlighted(true);
        }else if(key == SDLK_UP || key == SDLK_w){
                mButtons[mSelectedButtonIndex]->SetHighlighted(false);
                mSelectedButtonIndex--;
                mSelectedButtonIndex%=mButtons.size();
                mButtons[mSelectedButtonIndex]->SetHighlighted(true);
        } else if (key==SDLK_RETURN){
                mButtons[mSelectedButtonIndex]->OnClick();
        }
}
