#include "ControlsMenu.h"
#include "../../Game.h"
#include "../UIButton.h"
#include "../UIText.h"
#include "../UIRect.h"
#include <vector>
#include <SDL2/SDL.h>

ControlsMenu::ControlsMenu(class Game* game, const std::string& fontName)
    : UIScreen(game, fontName)
    , mBackButton(nullptr)
{
    mIsTransparent = false;

    float centerX = Game::WINDOW_WIDTH / 2.0f;
    float centerY = Game::WINDOW_HEIGHT / 2.0f;

    auto& lang = mGame->GetLanguage();

    const Vector3 textMain(0.91f, 0.94f, 0.91f);      // #E8EFE8
    const Vector3 textHover(0.66f, 0.80f, 0.70f);    // #A9CDB2
    const Vector3 buttonBg(0.11f, 0.14f, 0.12f);     // #1C241F
    const Vector4 darkOverlay(0.11f, 0.14f, 0.12f, 0.95f);
    const Vector4 transparent(0.0f, 0.0f, 0.0f, 0.0f);

    AddRect(Vector2(centerX, centerY), Vector2(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT), 1.0f, 0.0f, 1000);
    mRects[0]->SetColor(darkOverlay);

    UIText* title = AddText(lang.Get("controls.title"), Vector2(centerX, 150.0f), 1.2f, 0.0f, 56, 900, 1010);
    title->SetTextColor(textHover);
    title->SetBackgroundColor(transparent);

    float listStartY = centerY - 120.0f;
    float lineSpacing = 45.0f;
    const int pointSize = 40;
    const unsigned wrapLength = 900;
    int drawOrderBase = 1020;

    std::vector<std::string> commandLines = {
        lang.Get("controls.move"),
        lang.Get("controls.crouch"),
        lang.Get("controls.jump"),
        lang.Get("controls.attack"),
        lang.Get("controls.dash"),
        lang.Get("controls.shoot"),
        lang.Get("controls.pause")
    };

    for (size_t i = 0; i < commandLines.size(); ++i) {
        float y = listStartY + static_cast<float>(i) * lineSpacing;
        UIText* line = AddText(commandLines[i], Vector2(centerX, y), 0.9f, 0.0f, pointSize, wrapLength, drawOrderBase + static_cast<int>(i));
        line->SetTextColor(textMain);
        line->SetBackgroundColor(transparent);
    }

    UIText* hint = AddText(lang.Get("controls.hint"), Vector2(centerX, Game::WINDOW_HEIGHT - 60.0f), 0.6f, 0.0f, 22, 900, 1050);
    hint->SetTextColor(Vector3(0.5f, 0.6f, 0.52f));
    hint->SetBackgroundColor(transparent);

    mBackButton = AddButton(lang.Get("controls.back"), [this]() {
        // Reativa a tela anterior e fecha a atual
        auto& stack = mGame->GetUIStack();
        if (stack.size() >= 2) {
            stack[stack.size() - 2]->SetState(UIState::Active);
        }
        Close();
    }, Vector2(centerX, centerY + 260.0f), 1.0f, 0.0f, 42, 900, 1060);

    mBackButton->SetTextColor(textMain);
    mBackButton->SetHoverColor(textHover);
    mBackButton->SetBackgroundColor(transparent);
}

void ControlsMenu::HandleKeyPress(int key) {
    switch (key) {
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
        case SDLK_SPACE:
        case SDLK_ESCAPE:
            if (mBackButton) {
                mBackButton->OnClick();
            }
            break;
    }
}
