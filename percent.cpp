#include "percent.hpp"

int lastPercent=-1;

void showPercent(double value)
{
 int percent=value*100;
 if (percent!=lastPercent)
 {
  lastPercent=percent;
  cout << percent << "%\r" << flush << "    \r";
 }
}
