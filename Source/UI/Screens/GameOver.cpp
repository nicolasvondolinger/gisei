#include "GameOver.h"
#include "../../Game.h"

GameOver::GameOver(class Game* game, const std::string& fontName)
        :UIScreen(game, fontName)
{
        int height = mGame->WINDOW_HEIGHT, width = mGame->WINDOW_WIDTH;
        AddText("Game Over", Vector2(width/2.0f, height/2.0f - 50), 2.5f);
        AddButton("Retry", [this]() {
                this->Close(); 
                mGame->SetScene(GameScene::Level1);
        }, Vector2(width/2.0f, height/2.0f + 50));
        AddButton("Main Menu", [this]() {
                this->Close();
                mGame->SetScene(GameScene::MainMenu); 
        }, Vector2(width/2.0f, height/2.0f + 100));
        AddButton("Quit", [this]() {
                this->Close(); 
                mGame->Quit();
        }, Vector2(width/2.0f, height/2.0f + 150));
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