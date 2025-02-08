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
    _animators.pop_front();
  }
}

void LedAnimationController::clearAnimation(int col, int row, CRGB targetColor)
{
  auto animator = index(_animatorsForLeds, col, row);
  if (animator)
  {
    removeAnimator(animator);
  }

  index(_leds, col, row) = targetColor;
}

void LedAnimationController::animate(int framesToAdvance)
{
  std::list<LedAnimationController::LedAnimator *> toRemove;

  for (auto iter = _animators.begin(); iter != _animators.end(); iter++)
  {
    auto animator = *iter;
    if (!animator->animate(framesToAdvance))
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

LedAnimationController::LedAnimator::LedAnimator(CRGB *led, LedAnimator *&backRef)
    : _led(led), _backRef(backRef), _nominatorCurrent{0, 0, 0}, _nominatorStep{0, 0, 0}, _denominator(1), _framesLeft(0)
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

bool LedAnimationController::LedAnimator::animate(int framesToAdvance)
{
  // Serial.printf("Advance %d frames @%d \n", framesToAdvance, millis());

  while (framesToAdvance)
  {
    if (_framesLeft)
    {
      if (_framesLeft > framesToAdvance)
      {
        // Serial.printf("(%d,%d,%d) [%d, %d, %d] / %d + [%d, %d, %d] / %d * %d  ==> ",
        //               _led->red, _led->green, _led->blue,
        //               _nominatorCurrent[0], _nominatorCurrent[1], _nominatorCurrent[2],
        //               _denominator,
        //               _nominatorStep[0], _nominatorStep[1], _nominatorStep[2],
        //               _denominator,
        //               framesToAdvance);

        interpolateChannel(_led->red, _nominatorCurrent[0], _nominatorStep[0] * framesToAdvance, _denominator);
        interpolateChannel(_led->green, _nominatorCurrent[1], _nominatorStep[1] * framesToAdvance, _denominator);
        interpolateChannel(_led->blue, _nominatorCurrent[2], _nominatorStep[2] * framesToAdvance, _denominator);

        _framesLeft -= framesToAdvance;
        framesToAdvance = 0;
      }
      else
      {
        // Serial.printf("End of step (%d,%d,%d)  ==> ", _led->red, _led->green, _led->blue);

        *_led = _targetColor;

        framesToAdvance -= _framesLeft;
        _framesLeft = 0;
      }

      // Serial.printf("(%d,%d,%d)\n",
      //               _led->red, _led->green, _led->blue);
    }
    else
    {
      if (_steps.empty())
      {
        // Serial.printf("Animatino done\n");
        return false;
      }

      // Serial.printf("Next step\n");

      auto step = _steps.front();
      _steps.pop_front();

      _framesLeft = step.lenFrames;
      _denominator = step.lenFrames;

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
