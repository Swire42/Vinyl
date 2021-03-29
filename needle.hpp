#pragma once
#include <iostream>

using namespace std;

#define NEEDLE_NONE 0
#define NEEDLE_ROUND 1
#define NEEDLE_ELLIPT 2

struct Needle
{
 double xmax, ymax, zmax;

 Needle();
 Needle(int type);
 Needle(double x, double y, double z);

 double z(double theta, double k, double d);

 double a(double theta, double k);
 double b(double theta, double k);
 double c(double theta, double k);
};
