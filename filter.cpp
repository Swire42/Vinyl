#include "filter.hpp"
#include "percent.hpp"

#define MICRO 0.000001

Filter::Filter()
{
 Filter(FILTER_NO_OUTPUT, 44100);
}

Filter::Filter(vector<double> _a, vector<double> _b)
{
 a=_a;
 b=_b;
}

Filter::Filter(int filterType, double freqEch)
{
 a.clear();
 b.clear();
 type=filterType;

 if (type==FILTER_LINEAR)
 {
  a.push_back(1);
 }
 else if (type==FILTER_RIAA_LECT)
 {
  double
  t1=3180*MICRO,
  t2=318*MICRO,
  t3=75*MICRO,
  p1=-1/t1,
  p2=-1/t3,
  z1=-1/t2,
  d=4*freqEch*freqEch-2*(p1+p2)*freqEch+p1*p2;

  a.push_back((p2*(z1-2*freqEch))/d);
  a.push_back((2*p2*z1)/d);
  a.push_back((p2*(z1+2*freqEch))/d);

  b.push_back((2*p1*p2-8*freqEch*freqEch)/d);
  b.push_back((4*freqEch*freqEch+2*(p1+p2)*freqEch+p1*p2)/d);
 }
 else if (type==FILTER_RIAA_ENRG)
 {
  double
  t1=3180*MICRO,
  t2=318*MICRO,
  t3=75*MICRO,
  t4=3.18*MICRO,
  p1=-1/t2,
  p2=-1/t4,
  z1=-1/t1,
  z2=-1/t3,
  d=z2*(4*freqEch*freqEch-2*(p1+p2)*freqEch+p1*p2);

  a.push_back(p2*(4*freqEch*freqEch-2*(z1+z2)*freqEch+z1*z2)/d);
  a.push_back(p2*(2*z1*z2-8*freqEch*freqEch)/d);
  a.push_back(p2*(4*freqEch*freqEch+2*(z1+z2)*freqEch+z1*z2)/d);

  b.push_back(z2*(2*p1*p2-8*freqEch*freqEch)/d);
  b.push_back(z2*(4*freqEch*freqEch+2*(p1+p2)*freqEch+p1*p2)/d);
 }
}

vector<double> Filter::applyOn(vector<double> input)
{
 if (type==FILTER_RIAA_LECT)
 {
  cout << "Applying RIAA read filter...\n";
 }
 else if (type==FILTER_RIAA_ENRG)
 {
  cout << "Applying RIAA engraving filter...\n";
 }
 else
 {
  cout << "Applying a filter...\n";
 }

 vector<double> output;
 for (unsigned int i=0; i<input.size(); i++)
 {
  showPercent(double(i)/(input.size()-1));
  output.push_back(0);
  for (unsigned int k=0; k<a.size(); k++)
  {
   if (int(i)-int(k)<0) break;
   output[i]+=a[k]*input[i-k];
  }
  for (unsigned int k=0; k<b.size(); k++)
  {
   if (int(i)-int(k+1)<0) break;
   output[i]-=b[k]*output[i-(k+1)];
  }
 }
 return output;
}
