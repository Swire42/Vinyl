#include "audio.hpp"

using namespace std;

void Audio::testEffect()
{
 for (unsigned int c=0; c<nbCanals; c++)
 {
  for (unsigned int i=0; i<samples[0].size(); i++)
  {
   samples[c][i]/=10;
  }
 }
}

void Audio::applyEffects()
{
 Disk disk=Disk(DISK_33T);
 //disk.maxRad=7*CENTI;
 apply(Filter(FILTER_RIAA_ENRG, freq));
 integrate();
 engrave(disk);
 readWith(Needle(NEEDLE_ELLIPT), disk);
 applyTrackingAngle(disk);
 derivate();
 apply(Filter(FILTER_RIAA_LECT, freq));
}

void Audio::apply(Filter f)
{
 for (unsigned int c=0; c<nbCanals; c++)
 {
  samples[c]=f.applyOn(samples[c]);
 }
}
