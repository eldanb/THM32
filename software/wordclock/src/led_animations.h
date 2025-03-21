#pragma once

#include <list>
#include <algorithm>
#include <memory>
#include <FastLED.h>

class LedAnimationController
{
public:
  struct AnimationStep
  {
    CRGB targetColor;
    int targetFrame;
  };

public:
  LedAnimationController(CRGB *leds, int numRows, int numCols);
  ~LedAnimationController();

  void addAnimation(int col, int row, const AnimationStep &animationStep);
  void clearAnimation(int col, int row, CRGB targetColor);
  void clearAnimation(int col, int row);
  void clearAnimations();
  void animate(int targetFrame);
  void setAllToColor(CRGB targettColor, int frame);

  int getRowCount();
  int getColumnCount();

private:
  class LedAnimator
  {
  public:
    LedAnimator(CRGB *led, LedAnimator *&backRef);

    void addStep(const AnimationStep &step);
    bool animate(int targetFrame);
    LedAnimator *&backRef();

  private:
    int _nominatorCurrent[3];
    int _nominatorStep[3];
    CRGB _targetColor;
    int _denominator;

    int _currentFrame;
    int _endFrame;

    CRGB *_led;
    LedAnimator *&_backRef;

    std::list<AnimationStep> _steps;
  };

private:
  void removeAnimator(LedAnimator *);

  template <class T>
  inline T &index(T *array, int col, int row)
  {
    return array[(row / 2) * (2 * _numCols) +
                 ((row % 2 == 0)
                      ? _numCols - 1 - col
                      : _numCols + col)];
  }

  CRGB *_leds;
  int _numRows;
  int _numCols;

  LedAnimator **_animatorsForLeds;
  std::list<LedAnimator *> _animators;
};