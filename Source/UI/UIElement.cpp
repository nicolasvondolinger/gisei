#include "UIElement.h"
#include "../Game.h"
#include "../Renderer/Renderer.h"

UIElement::UIElement(class Game* game, const Vector2 &offset, const float scale, const float angle, int drawOrder)
        :mGame(game)
        ,mOffset(offset)
        ,mScale(scale)
        ,mAngle(angle)
        ,mIsVisible(true)
        ,mDrawOrder(drawOrder)
{
    mGame->GetRenderer()->AddUIElement(this);
}

UIElement::~UIElement()
{
    mGame->GetRenderer()->RemoveUIElement(this);
}