#include "clock_text.h"

const char *unitNumberNames[] = {
    "ZERO", "ONE", "TWO", "THREE", "FOUR", "FIVE",
    "SIX", "SEVEN", "EIGHT", "NINE", "TEN", "ELEVEN",
    "TWELVE", "THIRTEEN", "FOURTEEN", "FIFTEEN", "SIXTEEN", "SEVENTEEN",
    "EIGHTEEN", "NINETEEN"};

const char *tensNumberNames[] = {
    "TWENTY", "THIRTY", "FOURTH", "FIFTY"};

const char *minuteFiveSegmentNames[] = {
    "ZERO", "FIVE MINUTES", "TEN MINUTES", "FIFTEEN MINUTES", "TWENTY MINUTES",
    "TWENTY FIVE MINUTES", "THIRTY MINUTES"};

static std::string minuteToString(int minute)
{
  int minuteFiveSegment = minute / 5;
  std::string ret = minuteFiveSegmentNames[minuteFiveSegment];

  return ret;
}

static std::string numberToString(int number)
{
  if (number < 20)
  {
    return unitNumberNames[number];
  }
  else if (number < 60)
  {
    std::string ret;
    ret = tensNumberNames[(number / 10) - 2];
    if (number % 10)
    {
      ret += " ";
      ret += unitNumberNames[number % 10];
    }

    return ret;
  }
  else
  {
    return "Can't convert number";
  }
}

std::string timeToString(int hour, int minute)
{
  bool isTo = false;
  if (minute >= 35)
  {
    minute = 64 - minute;
    hour = (hour + 1) % 24;
    isTo = true;
  }

  std::string hourName =
      hour == 0 ? "MIDNIGHT" : hour == 12 ? "NOON"
                                          : numberToString(hour % 12);
  std::string minuteName = minuteToString(minute);

  if (minute < 5)
  {
    if (hour != 0 && hour != 12)
    {
      return "IT IS " + hourName + " OCLOCK";
    }
    else
    {
      return "IT IS " + hourName;
    }
  }
  else
  {
    return "IT IS " + minuteName + " " + (isTo ? "TO " : "PAST ") + hourName;
  }
}
