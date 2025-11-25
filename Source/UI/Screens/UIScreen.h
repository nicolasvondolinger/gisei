#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <list>

#include "../../Math.h"
#include "../UIText.h"
#include "../UIButton.h"
#include "../UIImage.h"
#include "../UIRect.h"

class UIScreen {
public:
    enum class UIState {
        Active,
        Closing
    };

    UIScreen(class Game *game, const std::string &fontName);

    virtual ~UIScreen();


    virtual void Update(float deltaTime);

    virtual void HandleKeyPress(int key);


    void Close();


    UIState GetState() const { return mState; }


    class Game *GetGame() { return mGame; }
    bool GetIsTransparent() const { return mIsTransparent; }


    UIButton *AddButton(const std::string &name, std::function<void()> onClick, const Vector2 &offset,
                        float scale = 1.0f, float angle = 0.0f, const int pointSize = 40,
                        const int unsigned wrapLength = 1024, int drawOrder = 100);

    UIText *AddText(const std::string &name, const Vector2 &offset, float scale = 1.0f, float angle = 0.0f,
                    const int pointSize = 40, const int unsigned wrapLength = 1024, int drawOrder = 100);

    UIImage *AddImage(const std::string &imagePath, const Vector2 &offset, float scale = 1.0f, float angle = 0.0f,
                      int drawOrder = 100);

    UIRect *AddRect(const Vector2 &offset, const Vector2 &size, float scale = 1.0f, float angle = 0.0f,
                    int drawOrder = 100);

protected:
    class Game *mGame;
    class Font *mFont;
    bool mIsTransparent;


    Vector2 mPos;
    Vector2 mSize;


    UIState mState;


    int mSelectedButtonIndex;
    std::vector<UIButton *> mButtons;
    std::vector<UIText *> mTexts;
    std::vector<UIImage *> mImages;
    std::vector<UIRect *> mRects;
};