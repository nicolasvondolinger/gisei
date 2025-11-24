#include "MainMenu.h"
#include "../../Game.h"
#include "../UIButton.h"
#include "../../Renderer/Renderer.h"
#include "../../Math.h"

MainMenu::MainMenu(class Game* game, const std::string& fontName)
    : UIScreen(game, fontName)
{
float centerX = Game::WINDOW_WIDTH / 2.0f;
    float centerY = Game::WINDOW_HEIGHT / 2.0f;

    // --- PALETA DE CORES (Mantida) ---
    const Vector4 corPreto(0.0f, 0.0f, 0.0f, 1.0f); // Ajustado para Vector3 se UIRect::SetColor pedir Vector3
    const Vector3 corBranco(1.0f, 1.0f, 1.0f);
    const Vector3 corVermelhoSangue(0.9f, 0.0f, 0.0f);
    const Vector4 corFundoInvisivel(0.0f, 0.0f, 0.0f, 0.0f);

    // 2. LOGO
    // IMPORTANTE: DrawOrder 100 garante que a logo fique na frente do fundo preto.
    Vector2 logoPos(centerX, 200.0f);
    AddImage("../Assets/UI/Logo.png", logoPos, 0.3f, 0.0f, 100); // <--- DrawOrder 100

    // 3. BOTÕES (DrawOrder 200)
    Vector2 playButtonPos(centerX, centerY + 50.0f);
    Vector2 exitButtonPos(centerX, centerY + 120.0f);
    const int FONT_SIZE_HD = 48;

    // --- Botão Play ---
    UIButton* playButton = AddButton("Start Game", [this]() {
        Close();
        mGame->SetScene(GameScene::Level1);
    }, playButtonPos, 1.0f, 0.0f, FONT_SIZE_HD, 900, 200); // <--- DrawOrder 200
    // Nota: adicionei 900 (wrapLength padrão) antes do drawOrder, verifique a assinatura do seu AddButton

    playButton->SetTextColor(corBranco);
    playButton->SetHoverColor(corVermelhoSangue);
    playButton->SetBackgroundColor(corFundoInvisivel);

    // --- Botão Exit ---
    UIButton* exitButton = AddButton("Quit", [this]() {
        mGame->Quit();
    }, exitButtonPos, 1.0f, 0.0f, FONT_SIZE_HD, 900, 200); // <--- DrawOrder 200

    exitButton->SetTextColor(corBranco);
    exitButton->SetHoverColor(corVermelhoSangue);
    exitButton->SetBackgroundColor(corFundoInvisivel);

}

void MainMenu::HandleKeyPress(int key)
{
    if (mButtons.empty()) return;

    int currentSelection = mSelectedButtonIndex;

    switch (key) {
        case SDLK_DOWN:
        case SDLK_s:
            mButtons[currentSelection]->SetHighlighted(false);
            mSelectedButtonIndex = (currentSelection + 1) % mButtons.size();
            mButtons[mSelectedButtonIndex]->SetHighlighted(true);
            break;

        case SDLK_UP:
        case SDLK_w:
            mButtons[currentSelection]->SetHighlighted(false);
            mSelectedButtonIndex = (currentSelection - 1 + mButtons.size()) % mButtons.size();
            mButtons[mSelectedButtonIndex]->SetHighlighted(true);
            break;

        case SDLK_RETURN:
        case SDLK_KP_ENTER:
        case SDLK_SPACE:
            if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < mButtons.size())
            {
                mButtons[mSelectedButtonIndex]->OnClick();
            }
            break;
    }
}