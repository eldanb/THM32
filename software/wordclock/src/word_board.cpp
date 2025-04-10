#include "word_board.h"
#include <sstream>

// #define FIXED_ANIMATION_ID 3

using AnimationStep = LedAnimationController::AnimationStep;

WordBoard::WordBoard(const char *boardContents, LedAnimationController &animationController)
    : _boardContents(boardContents), _animationController(animationController)
{
}

void WordBoard::displayText(const char *text, int startFrame, int *endFrameOutput)
{
#ifndef FIXED_ANIMATION_ID
  int animation = rand() % 5;
#else
  int animation = FIXED_ANIMATION_ID;
#endif

  switch (animation)
  {
  case 0:
    displayTextRainbow(text, startFrame);
    break;

  case 1:
    displayTextEnterFromRight(text, startFrame);
    break;

  case 2:
    displayTextRandomOrder(text, startFrame);
    break;

  case 3:
    displayTextSnakeRandom(text, startFrame);
    break;

  case 4:
  default:
    displayTextFadeType(text, startFrame);
    break;
  }

  if (endFrameOutput)
  {
    *endFrameOutput = startFrame;
  }
}

void WordBoard::enumerateTextCoordinates(const char *text, const std::function<void(int, int)> &fn)
{
  std::stringstream ss(text);
  std::string token;
  std::list<std::pair<int, int>> ret;

  int index = 0;
  while (ss >> token)
  {
    int tokLen = token.length();

    auto nCols = _animationController.getColumnCount();
    auto nLeds = _animationController.getRowCount() * nCols;
    while (index < nLeds)
    {
      if (index % nCols < nCols - (tokLen - 1) &&
          !memcmp(token.c_str(), (_boardContents + index), tokLen))
      {
        for (int letter = 0; letter < tokLen; letter++)
        {
          fn(index % nCols + letter, index / nCols);
        }

        index += tokLen;
        break;
      }

      index++;
    }
  }
}

void WordBoard::displayTextRainbow(const char *text, int &startFrame)
{
  enumerateTextCoordinates(
      text,
      [this, &startFrame](int col, int row)
      {
        _animationController.addAnimation(col, row, AnimationStep{CRGB{0, 0, 0}, startFrame});
        _animationController.addAnimation(col, row, AnimationStep{CRGB{32, 0, 0}, startFrame + 200});
        _animationController.addAnimation(col, row, AnimationStep{CRGB{32, 0, 32}, startFrame + 400});
        _animationController.addAnimation(col, row, AnimationStep{CRGB{32, 32, 32}, startFrame + 600});
      });
  startFrame += 600;
}

void WordBoard::displayTextEnterFromRight(const char *text, int &startFrame)
{
  enumerateTextCoordinates(
      text,
      [this, &startFrame](int col, int row)
      {
        int nCols = _animationController.getColumnCount();
        for (int tCol = nCols - 1; tCol > col; tCol--)
        {
          _animationController.addAnimation(tCol, row, AnimationStep{CRGB{0, 0, 0}, startFrame});
          _animationController.addAnimation(tCol, row, AnimationStep{CRGB{32, 32, 32}, startFrame + 25});
          _animationController.addAnimation(tCol, row, AnimationStep{CRGB{0, 0, 0}, startFrame + 50});
          startFrame += 25;
        }

        _animationController.addAnimation(col, row, AnimationStep{CRGB{0, 0, 0}, startFrame});
        _animationController.addAnimation(col, row, AnimationStep{CRGB{32, 32, 32}, startFrame + 25});

        startFrame += 25;
      });
}

void WordBoard::displayTextRandomOrder(const char *text, int &startFrame)
{
  enumerateTextCoordinates(
      text,
      [this, text, &startFrame](int col, int row)
      {
        int randomStartFrame = startFrame + (rand() % strlen(text)) * 50;

        _animationController.addAnimation(col, row, AnimationStep{CRGB{0, 0, 0}, randomStartFrame});
        _animationController.addAnimation(col, row, AnimationStep{CRGB{32, 32, 32}, randomStartFrame + 500});
      });
  startFrame += strlen(text) * 50;
}

void WordBoard::displayTextSnakeRandom(const char *text, int &startFrame)
{
  enumerateTextCoordinates(
      text,
      [this, text, &startFrame](int col, int row)
      {
        _animationController.addAnimation(col, row, AnimationStep{CRGB{0, 0, 0}, startFrame});
        _animationController.addAnimation(col, row, AnimationStep{CRGB{32, 0, 0}, startFrame + 100});
        _animationController.addAnimation(col, row, AnimationStep{CRGB{32, 32, 0}, startFrame + 200});
        _animationController.addAnimation(col, row, AnimationStep{CRGB{0, 32, 0}, startFrame + 300});
        _animationController.addAnimation(col, row, AnimationStep{CRGB{0, 32, 32}, startFrame + 400});
        _animationController.addAnimation(col, row, AnimationStep{CRGB{0, 0, 32}, startFrame + 500});
        _animationController.addAnimation(col, row, AnimationStep{CRGB{32, 0, 32}, startFrame + 600});
        _animationController.addAnimation(col, row, AnimationStep{CRGB{32, 32, 32}, startFrame + 700});

        startFrame += 50;
      });
}

void WordBoard::displayTextFadeType(const char *text, int &startFrame)
{
  enumerateTextCoordinates(
      text,
      [this, &startFrame](int col, int row)
      {
        _animationController.addAnimation(col, row, AnimationStep{CRGB{0, 0, 0}, startFrame});
        _animationController.addAnimation(col, row, AnimationStep{CRGB{32, 32, 32}, startFrame + 500});

        startFrame += 50;
      });
}
