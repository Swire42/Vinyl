#include "audio.hpp"
#include "percent.hpp"

using namespace std;

#define show(a) cout << #a << ": " << a << endl;

void Audio::derivate()
{
 cout << "Kinetic to electric convertion...\n";
 vector<vector<double> > result;

 for (unsigned int c=0; c<nbCanals; c++)
 {
  result.push_back(vector<double>());
  result[c].push_back(0);
  for (unsigned int i=1; i<samples[0].size(); i++)
  {
   showPercent((double(i+c*samples[c].size()))/(nbCanals*samples[0].size()-1));
   result[c].push_back(samples[c][i]-samples[c][i-1]);
  }
 }

 samples=result;
}

#define RECENTER_CONST 0.99993 // used to center integrate() output dynamicaly

void Audio::integrate()
{
 center();
 cout << "Electric to kinetic convertion...\n";
 vector<vector<double> > result;

 for (unsigned int c=0; c<nbCanals; c++)
 {
  result.push_back(vector<double>());
  result[c].push_back(samples[c][0]);
  for (unsigned int i=1; i<samples[0].size(); i++)
  {
   showPercent((double(i+c*samples[c].size()))/(nbCanals*samples[0].size()-1));
   result[c].push_back((result[c][i-1]+samples[c][i])*RECENTER_CONST);
  }
 }

 samples=result;
}
