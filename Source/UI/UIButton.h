#pragma once
#include "UIText.h"
#include <functional>

class UIButton : public UIText {
public:
    UIButton(class Game *game, std::function<void()> onClick, const std::string &text, class Font *font,
             const Vector2 &offset, float scale, float angle, int pointSize, const unsigned wrapLength,
             int drawOrder = 100);

    ~UIButton();

    void SetHighlighted(bool sel) { mHighlighted = sel; }

    void OnClick();


    void SetHoverColor(const Vector3 &color) { mHoverColor = color; }


    void SetTextColor(const Vector3 &color) override {
        UIText::SetTextColor(color);
        mNormalColor = color;
    }

    void Draw(class Shader *shader) override;

private:
    std::function<void()> mOnClick;
    bool mHighlighted;


    Vector3 mNormalColor;
    Vector3 mHoverColor;
};