#include "MainMenu.h"
#include "../../Game.h"       // Caminho para o Game.h do Mario
#include "../UIButton.h"
#include "../../Renderer/Renderer.h" // Para as constantes de tela
#include "../../Math.h"             // Necessário para Vector3/Vector4

MainMenu::MainMenu(class Game* game, const std::string& fontName)
    : UIScreen(game, fontName)
{
    // Posições (usando as constantes estáticas do Game)
    float centerX = Game::WINDOW_WIDTH / 2.0f;
    float centerY = Game::WINDOW_HEIGHT / 2.0f;

    Vector2 playButtonPos(centerX, centerY - 40.0f); // Posição do "Play"
    Vector2 exitButtonPos(centerX, centerY + 40.0f); // Posição do "Exit"


    // --- Criar Botão Play ---
    UIButton* playButton = AddButton("Play", [this]() {
        Close(); // Fecha o MainMenu
        mGame->SetScene(GameScene::Level1); // Carrega a cena do Nível (Mario)
    }, playButtonPos);

    Vector4 blue(0.0f, 0.0f, 1.0f, 1.0f);
    const Vector3 white(1.0f, 1.0f, 1.0f);

    // *** DEFINA AS CORES AQUI ***
    playButton->SetTextColor(white);
    playButton->SetBackgroundColor(blue);

    // --- Criar Botão Exit ---
    UIButton* exitButton = AddButton("Exit", [this]() {
        mGame->Quit(); // Fecha o jogo
    }, exitButtonPos);

    // *** DEFINA AS CORES AQUI ***
    exitButton->SetTextColor(white);
    exitButton->SetBackgroundColor(blue);

    // O primeiro botão (Play) será destacado automaticamente
    // (Assumindo que sua classe base UIScreen cuida disso)
}

void MainMenu::HandleKeyPress(int key)
{
    // Assume que mButtons e mSelectedButtonIndex vêm da classe base UIScreen
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

        case SDLK_RETURN: // Tecla Enter
        case SDLK_KP_ENTER: // Tecla Enter (teclado numérico)
            if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < mButtons.size())
            {
                // Chama a função lambda associada ao botão
                mButtons[mSelectedButtonIndex]->OnClick();
            }
            break;
        default:
            break;
    }
}