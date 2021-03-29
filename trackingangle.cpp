#include "audio.hpp"
#include "percent.hpp"
#include <cmath>

#define show(a) cout << #a << ": " << a << endl;

void Audio::applyTrackingAngle(Disk disk)
{
 center();
 cout << "Applying tracking angle error...\n";
 vector<double> lateral;
 vector<double> depth;
 vector<double> deltaTime;

 // the following variables don't vqrry much between 2 samples, so we don't
 // need to update them each time, each 100 is suficient
 double time, rad, theta, speed;
 for (unsigned int i=0; i<samples[0].size(); i++)
 {
  showPercent(double(i)/samples[0].size()/2);
  if (i%100==0)
  {// update values
   time=double(i)/freq;
   rad=disk.getRad(time);
   theta=disk.getArmGrooveAngle(rad);
   speed=disk.getSpeed(rad);
  }
  lateral.push_back((samples[0][i]+samples[1][i])/2);
  double maxSignedAmp=(disk.maxGrooveWidth-disk.minGrooveWidth)/4;
  double rightMean=disk.minGrooveWidth+maxSignedAmp;
  double leftMean=-rightMean;
  double lValue=samples[0][i]+leftMean;
  double rValue=samples[1][i]+rightMean;
  depth.push_back((rValue-lValue)/2);

  // applying effect :
  double deltaLenght=lateral[i]*tan(theta);
  deltaTime.push_back(deltaLenght/speed);
  lateral[i]/=cos(theta);
 }

 samples[0].clear();
 samples[1].clear();

 double prevL=lateral[0], prevD=depth[0], prevT=deltaTime[0]-1/double(freq),
        curL =lateral[0], curD =depth[0], curT =deltaTime[0],
        nextL=lateral[1], nextD=depth[1], nextT=deltaTime[1]+1/double(freq);

 for (unsigned int i=0; i<lateral.size(); i++)
 {
  showPercent(double(i)/lateral.size()/2+0.5);
  if (i%100==0)
  {// update values
   time=double(i)/freq;
   rad=disk.getRad(time);
   theta=disk.getArmGrooveAngle(rad);
   speed=disk.getSpeed(rad);
  }
  // interpolation :
  double usedL, usedD, usedT;
  if (curT<0)
  {
   usedL=nextL;
   usedD=nextD;
   usedT=nextT;
  }
  else
  {
   usedL=prevL;
   usedD=prevD;
   usedT=prevT;
  }

  double reSmplLateral=curL-curT*(curL-usedL)/(curT-usedT);
  double reSmplDepth=curD-curT*(curD-usedD)/(curT-usedT);

  double lValue=reSmplLateral-reSmplDepth;
  double rValue=reSmplLateral+reSmplDepth;
  // don't need to recenter, as we're going to derivate
  samples[0].push_back(lValue);
  samples[1].push_back(rValue);

  // rotating values
  prevL=curL;
  prevD=curD;
  prevT=curT-1/double(freq);
  curL=nextL;
  curD=nextD;
  curT=nextT-1/double(freq);
  if (i!=lateral.size()-1)
  {
   nextL=lateral[i+1];
   nextD=depth[i+1];
   nextT=deltaTime[i+1]+1/double(freq);
  }
 }
}
