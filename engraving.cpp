#include "audio.hpp"
#include <cmath>
#include "percent.hpp"
#include "diskinfo.hpp"

#define show(a) cout << #a << ":" << a << endl

void Audio::engrave(Disk disk)
{
 normalize((disk.maxGrooveWidth-disk.minGrooveWidth)/4); // /2: 2 canals ;  /2: signed values

 cout << "Engraving on disk...\n";
 int modCount=0;

 for (unsigned int i=1; i<samples[0].size(); i++)
 {
  showPercent(double(i)/samples[0].size());
  //double timeSec=double(i)/freq; // time elapsed since we began to write
  //double interGrooveDist=110*MICRO;
  //double rotPerSec=100.0/3/60; // rpm=100/3
  //double rad=15*CENTI-interGrooveDist*rotPerSec*timeSec; // current radius
  //double perimeter=2*M_PI*rad; // perimeter at current radius
  //double speed=perimeter*rotPerSec;
  //double forwardDist=speed/freq; // physical distance between 2 samples on the disk

  double forwardDist=disk.getInterEchDist(disk.getRad(double(i)/freq), freq);

  /*if (i==1)
  {
   show(timeSec);
   show(interGrooveDist);
   show(rotPerSec);
   show(rad);
   show(perimeter);
   show(forwardDist);
  }*/

  for (unsigned int c=0; c<samples.size(); c++)
  {
   //cout << forwardDist << endl;
   if (abs(samples[c][i]-samples[c][i-1])>forwardDist)
   {// engraving head would have moved with a > to 45° angle => impossible
    modCount++;
    if (samples[c][i]<samples[c][i-1])
    {
     samples[c][i]=samples[c][i-1]-forwardDist;
    }
    else
    {
     samples[c][i]=samples[c][i-1]+forwardDist;
    }
   }
  }
 }
 if (modCount>0)
 {
  cout << "Engraving produced " << modCount << " modification(s).\n";
 }
}
