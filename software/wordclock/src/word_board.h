#pragma once

#include "led_animations.h"

#include <list>

class WordBoard
{
public:
  WordBoard(const char *boardContents, LedAnimationController &animationController);
  void displayText(const char *text, int startFrame, int *endFrameOutput);

private:
  void enumerateTextCoordinates(const char *text, const std::function<void(int, int)> &fn);

  const char *_boardContents;
  LedAnimationController &_animationController;

private:
  void displayTextFadeType(const char *text, int &startFrame);
  void displayTextSnakeRandom(const char *text, int &startFrame);
  void displayTextRandomOrder(const char *text, int &startFrame);
  void displayTextEnterFromRight(const char *text, int &startFrame);
  void displayTextRainbow(const char *text, int &startFrame);
};