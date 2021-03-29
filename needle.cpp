#include "audio.hpp"
#include "diskinfo.hpp"
#include "percent.hpp"
#include <cmath>
#include <iomanip>

Needle::Needle()
{
 Needle(NEEDLE_NONE);
}

Needle::Needle(int type)
{
 if (type==NEEDLE_NONE)
 {
  xmax=1*MICRO;
  ymax=1*MICRO;
  zmax=1*MICRO;
 }
 else if (type==NEEDLE_ROUND)
 {
  xmax=18*MICRO;
  ymax=18*MICRO;
  zmax=18*MICRO;
 }
 else if (type==NEEDLE_ELLIPT)
 {
  xmax=25*MICRO;
  ymax=13*MICRO;
  zmax=18*MICRO;
 }
 else
 {
  cout << "Warning : attempting to create a Needle object with no preset !\n";
 }
}

Needle::Needle(double x, double y, double z)
{
 xmax=x;
 ymax=y;
 zmax=z;
}

double Needle::z(double theta, double k, double d)
{
 double temp1=d*cos(theta)-k*sin(theta);
 double temp2=k*cos(theta)+d*sin(theta);
 return zmax-zmax*sqrt(1-(temp1*temp1)/(xmax*xmax)-(temp2*temp2)/(ymax*ymax));
}

double Needle::a(double theta, double k)
{
 double C=cos(theta);
 double S=sin(theta);

 double C2=C*C;
 double S2=S*S;
 double X2=xmax*xmax;
 double Y2=ymax*ymax;
 double Z2=zmax*zmax;

 double temp=C2/X2+S2/Y2;

 return 4*Z2*temp*temp+4*temp;
}

double Needle::b(double theta, double k)
{
 double C=cos(theta);
 double S=sin(theta);
 double X2=xmax*xmax;
 double Y2=ymax*ymax;
 double Z2=zmax*zmax;

 double C2=C*C;
 double S2=S*S;

 return 8*Z2*k*C*S*(S2/(Y2*Y2)-C2/(X2*X2)+(C2-S2)/(X2*Y2))-8*k*C*S*(1/X2-1/Y2);
}

double Needle::c(double theta, double k)
{
 double C=cos(theta);
 double S=sin(theta);

 double k2=k*k;
 double C2=C*C;
 double S2=S*S;
 double X2=xmax*xmax;
 double Y2=ymax*ymax;
 double Z2=zmax*zmax;

 double temp=1/X2-1/Y2;
 return 4*Z2*k2*C2*S2*temp*temp+4*k2*(S2/X2+C2/Y2)-4;
}

#define show(a) cout << #a << ":" << a << endl

struct SliceInfo
{
 double a,b,c; // coef for finding roots
 double delta;
 double sqrtOfDelta;
 int j; // sample number delta
 double k; // silce position

 SliceInfo(Needle n, double theta, double interSampleDist, int _j)
 {
  j=_j;
  k=interSampleDist*j;
  a=n.a(theta, k);
  b=n.b(theta, k);
  c=n.c(theta, k);
  delta=b*b-4*a*c;
  if (delta>=0) sqrtOfDelta=sqrt(delta);
 }

 double getD1()
 {
  if (a>0) return (-b-sqrtOfDelta)/(2*a);
  else return (-b+sqrtOfDelta)/(2*a);
 }

 double getD2()
 {
  if (a>0) return (-b+sqrtOfDelta)/(2*a);
  else return (-b-sqrtOfDelta)/(2*a);
 }
};

void Audio::readWith(Needle needle, Disk disk)
{
 cout << "Reading disk with needle...\n";

 vector<vector<double> > result;

 result.push_back(vector<double>());
 result.push_back(vector<double>());

 // the following variables don't vary much between two samples, and so are
 // only updated every 100 samples
 double time;
 double rad;
 double theta;
 vector<SliceInfo> slices;

 for (unsigned int i=0; i<samples[0].size(); i++)
 {
  showPercent(double(i)/samples[0].size());

  if (i%100==0)
  {// update values
   time=double(i)/freq;
   rad=disk.getRad(time);
   theta=disk.getArmGrooveAngle(rad);

   double interSampleDist=disk.getInterEchDist(rad, freq);

   int needleEchLenght=round(needle.xmax/interSampleDist);

   slices.clear();

   for (int j=-needleEchLenght+1; j<needleEchLenght; j++)
   {
    slices.push_back(SliceInfo(needle, theta, interSampleDist, j));
   }
  }

  double maxL=-1000, minR=1000;

  for (unsigned int iSlice=0; iSlice<slices.size(); iSlice++)
  {
   int curSmpl=i+slices[iSlice].j;
   if (curSmpl>=0)
   {
    if (slices[iSlice].delta>=0)
    {
     double d1=slices[iSlice].getD1();
     double d2=slices[iSlice].getD2();

     double lk=-d1-needle.z(theta, slices[iSlice].k, d1);
     //if (i==10) show(lk);
     double rk=needle.z(theta, slices[iSlice].k, d2)-d2;
     //if (i==10) show(rk);
     double dlk=samples[0][curSmpl]-samples[0][i];
     double drk=samples[1][curSmpl]-samples[1][i];
     double Lk=lk-dlk;
     //cout << dlk << endl;
     double Rk=rk-drk;
     //show(dlk);

     if (Lk>maxL)
     {
      maxL=Lk;
     }
     if (Rk<minR) minR=Rk;
    }
   }
  }

  result[0].push_back(-maxL+samples[0][i]);
  result[1].push_back(-minR+samples[1][i]);
 }

 samples=result;
}
