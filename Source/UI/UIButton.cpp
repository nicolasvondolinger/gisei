#include "UIButton.h"
#include "../Renderer/Texture.h"
#include "../Renderer/Shader.h"

UIButton::UIButton(class Game *game, std::function<void()> onClick, const std::string &text, class Font *font,
                   const Vector2 &offset, float scale, float angle, int pointSize, const unsigned wrapLength,
                   int drawOrder)
    : UIText(game, text, font, offset, scale, angle, pointSize, wrapLength, drawOrder)
      , mOnClick(onClick)
      , mHighlighted(false)
      , mNormalColor(1.0f, 1.0f, 1.0f)
      , mHoverColor(1.0f, 1.0f, 0.0f) {
    UIText::SetTextColor(mNormalColor);
}

UIButton::~UIButton() {
}

void UIButton::OnClick() {
    if (mOnClick) {
        mOnClick();
    }
}

void UIButton::Draw(class Shader *shader) {
    if (mHighlighted) {
        UIText::SetTextColor(mHoverColor);
    } else {
        UIText::SetTextColor(mNormalColor);
    }


    mBackgroundColor.w = 0.0f;


    UIText::Draw(shader);
}