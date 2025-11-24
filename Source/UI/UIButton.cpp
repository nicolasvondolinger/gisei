#include "UIButton.h"
#include "../Renderer/Texture.h"
#include "../Renderer/Shader.h" // Adicione se estiver faltando

UIButton::UIButton(class Game* game, std::function<void()> onClick, const std::string& text, class Font* font,
                   const Vector2 &offset, float scale, float angle, int pointSize, const unsigned wrapLength, int drawOrder)
        :UIText(game, text, font, offset, scale, angle, pointSize, wrapLength, drawOrder)
        ,mOnClick(onClick)
        ,mHighlighted(false)
        ,mNormalColor(1.0f, 1.0f, 1.0f) // Branco
        ,mHoverColor(1.0f, 1.0f, 0.0f)  // Amarelo/Vermelho (será sobrescrito no MainMenu)
{
    // Garante que o texto comece Branco para poder ser tingido
    UIText::SetTextColor(mNormalColor);
}

UIButton::~UIButton()
{
}

void UIButton::OnClick()
{
    if (mOnClick) {
        mOnClick();
    }
}

void UIButton::Draw(class Shader* shader)
{
    // 1. Apenas atualiza a variável de cor do pai
    if(mHighlighted) {
        UIText::SetTextColor(mHoverColor);
    }
    else {
        UIText::SetTextColor(mNormalColor);
    }

    // 2. Garante fundo transparente
    mBackgroundColor.w = 0.0f;

    // 3. Manda desenhar (O UIText::Draw vai enviar mTextColor pro shader)
    UIText::Draw(shader);
}