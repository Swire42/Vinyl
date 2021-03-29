#pragma once

#include <iostream>
#include <vector>
#include "filter.hpp"
#include "diskinfo.hpp"
#include "needle.hpp"

using namespace std;

struct Audio
{
 uint32_t size;
 uint32_t fmtSize;
 uint32_t dataSize;

 uint16_t audioFormat;
 uint16_t nbCanals;
 uint32_t freq;
 uint32_t bytePerSec;
 uint16_t bytePerBloc;
 uint16_t bitsPerSample;

 vector<vector<double> > samples;

 // in_out.cpp
 int readFromFile(string filename);
 void center(); // auto-used by normalize(.)
 void normalize(double maxAmp); // auto-used by writeToFile(.)
 void writeToFile(string filename);
 void showInfo();

 // bobine.cpp
 void derivate();
 void integrate();

 // effects.cpp
 void testEffect();
 void applyEffects();
 void apply(Filter f);

 // engraving.cpp
 void engrave(Disk disk);

 // needle.cpp
 void readWith(Needle needle, Disk disk);

 // trackingangle.cpp
 void applyTrackingAngle(Disk disk);
};
