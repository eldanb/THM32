#include "led_animations.h"

LedAnimationController::LedAnimationController(CRGB *leds, int numRows, int numCols)
    : _leds(leds), _numRows(numRows), _numCols(numCols)
{
  _animatorsForLeds = (LedAnimator **)malloc(sizeof(LedAnimator *) * numRows * numCols);
  memset(_animatorsForLeds, 0, sizeof(LedAnimator *) * numRows * numCols);
}

LedAnimationController::~LedAnimationController()
{
  clearAnimations();
  delete[] _animatorsForLeds;
}

void LedAnimationController::addAnimation(int col, int row, const LedAnimationController::AnimationStep &animationStep)
{
  auto &animator = index(_animatorsForLeds, col, row);
  if (!animator)
  {
    animator = new LedAnimationController::LedAnimator(&index(_leds, col, row), animator);
    _animators.push_back(animator);
  }

  animator->addStep(animationStep);
}

void LedAnimationController::clearAnimations()
{
  while (_animators.size())
  {
    removeAnimator(_animators.front());
  }
}

void LedAnimationController::clearAnimation(int col, int row, CRGB targetColor)
{
  clearAnimation(col, row);
  index(_leds, col, row) = targetColor;
}

void LedAnimationController::clearAnimation(int col, int row)
{
  auto animator = index(_animatorsForLeds, col, row);
  if (animator)
  {
    removeAnimator(animator);
  }
}

void LedAnimationController::animate(int targetFrame)
{
  std::list<LedAnimationController::LedAnimator *> toRemove;

  for (auto iter = _animators.begin(); iter != _animators.end(); iter++)
  {
    auto animator = *iter;
    if (!animator->animate(targetFrame))
    {
      toRemove.push_back(animator);
    }
  }

  for (auto iter = toRemove.begin(); iter != toRemove.end(); iter++)
  {
    removeAnimator(*iter);
  }
}

void LedAnimationController::removeAnimator(LedAnimationController::LedAnimator *animator)
{
  animator->backRef() = nullptr;
  _animators.remove(animator);
}

void LedAnimationController::setAllToColor(CRGB targetColor, int frame)
{
  clearAnimations();

  for (int row = 0; row < _numRows; row++)
  {
    for (int col = 0; col < _numCols; col++)
    {
      if (index(_leds, col, row) != targetColor)
      {
        addAnimation(col, row, AnimationStep{targetColor : targetColor, targetFrame : frame});
      }
    }
  }
}

int LedAnimationController::getRowCount()
{
  return _numRows;
}

int LedAnimationController::getColumnCount()
{
  return _numCols;
}

LedAnimationController::LedAnimator::LedAnimator(CRGB *led, LedAnimator *&backRef)
    : _led(led), _backRef(backRef), _nominatorCurrent{0, 0, 0}, _nominatorStep{0, 0, 0}, _denominator(1), _endFrame(0), _currentFrame(millis())
{
}

void LedAnimationController::LedAnimator::addStep(const AnimationStep &step)
{
  _steps.push_back(step);
}

inline void interpolateChannel(uint8_t &channel, int &nominatorCurrent, int nominatorStep, int denominator)
{
  nominatorCurrent += nominatorStep;
  while (nominatorCurrent > denominator)
  {
    nominatorCurrent -= denominator;
    channel++;
  }

  while (nominatorCurrent < -denominator)
  {
    nominatorCurrent += denominator;
    channel--;
  }
}

bool LedAnimationController::LedAnimator::animate(int targetFrame)
{
  while (_currentFrame < targetFrame)
  {
    if (_currentFrame < _endFrame)
    {
      if (_endFrame > targetFrame)
      {
        int framesToAdvance = targetFrame - _currentFrame;

        interpolateChannel(_led->red, _nominatorCurrent[0], _nominatorStep[0] * framesToAdvance, _denominator);
        interpolateChannel(_led->green, _nominatorCurrent[1], _nominatorStep[1] * framesToAdvance, _denominator);
        interpolateChannel(_led->blue, _nominatorCurrent[2], _nominatorStep[2] * framesToAdvance, _denominator);

        _currentFrame = targetFrame;
      }
      else
      {
        *_led = _targetColor;
        _currentFrame = _endFrame;
      }
    }
    else
    {
      if (_steps.empty())
      {
        return false;
      }

      auto step = _steps.front();
      _steps.pop_front();

      _endFrame = step.targetFrame;
      _denominator = _endFrame - _currentFrame;

      memset(&_nominatorCurrent, 0, sizeof(_nominatorCurrent));

      _nominatorStep[0] = step.targetColor.red - _led->red;
      _nominatorStep[1] = step.targetColor.green - _led->green;
      _nominatorStep[2] = step.targetColor.blue - _led->blue;
      _targetColor = step.targetColor;
    }
  }

  return true;
}

LedAnimationController::LedAnimator *&LedAnimationController::LedAnimator::backRef()
{
  return _backRef;
}
