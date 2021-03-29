#include "diskinfo.hpp"
#include <cmath>

Disk::Disk()
{
 Disk(DISK_NONE);
}

Disk::Disk(int type)
{
 if (type==DISK_NONE)
 {
  rpm=1;
  maxRad=1;
  minRad=1;
  interGroovDist=1;
  armBase2CenterDist=1;
  armLenght=1;
  armBent=0;
  minGrooveWidth=1;
  maxGrooveWidth=2;
 }
 else if (type==DISK_33T)
 {
  rpm=100/3.0;
  maxRad=15*CENTI;
  minRad=5*CENTI;
  interGroovDist=110*MICRO;
  armBase2CenterDist=21*CENTI;
  armLenght=22*CENTI;
  armBent=22*M_PI/180;
  minGrooveWidth=25*MICRO;
  maxGrooveWidth=100*MICRO;
 }
 else
 {
  cout << "Warning : attempting to create a Disk object with no preset !\n";
 }
}

Disk::Disk(double rotpm, double maxR, double minR, double iGD, double aB2CD, double aL, double minGW, double maxGW)
{
 rpm=rotpm;
 maxRad=maxR;
 minRad=minR;
 interGroovDist=iGD;
 armBase2CenterDist=aB2CD;
 armLenght=aL;
 minGrooveWidth=minGW;
 maxGrooveWidth=maxGW;
}

double Disk::maxTime()
{
 double nbTurns=(maxRad-minRad)/interGroovDist;
 return 1/(rpm/60)*nbTurns;
}

double Disk::getRad(double sec)
{
 if (sec>maxTime())
 {
  cout << "Warning : attempting to calculate a radius outside of usable space\n";
  return 0;
 }
 return maxRad-(rpm/60)*sec*interGroovDist;
}

double Disk::getInterEchDist(double rad, double fs)
{
 double perim=2*M_PI*rad;
 return perim/fs;
}

double Disk::getArmGrooveAngle(double rad)
{
 return asin(rad/(2*armLenght)+(armLenght*armLenght-armBase2CenterDist*armBase2CenterDist)/(2*armLenght*rad))-armBent;
}

double Disk::getSpeed(double rad)
{
 double perim=2*M_PI*rad;
 double rotPerSec=rpm/60;
 return rotPerSec/perim;
}
