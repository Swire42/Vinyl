#pragma once
#include <iostream>

using namespace std;

#define MICRO 0.000001
#define CENTI 0.01

#define DISK_NONE 0
#define DISK_33T 1

struct Disk
{
 double rpm;
 double maxRad;
 double minRad;
 double interGroovDist;
 double armBase2CenterDist;
 double armLenght;
 double armBent;
 double minGrooveWidth;
 double maxGrooveWidth;

 Disk();
 Disk(int type);
 Disk(double rotpm, double maxR, double minR, double iGD, double aB2CD, double aL, double minGW, double maxGW);

 double maxTime();
 double getRad(double sec);
 double getInterEchDist(double rad, double fs);
 double getArmGrooveAngle(double rad);
 double getSpeed(double rad);
};
