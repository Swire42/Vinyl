#include "audio.hpp"
#include <fstream>
#include <cmath>
#include "percent.hpp"

using namespace std;

#define show(a) cout << #a << ": " << a << endl

int Audio::readFromFile(string filename)
{
 cout << "Reading file \"" << filename << "\"...\n";
 ifstream file(filename.c_str(), ios::in|ios::binary);
 if (!file.good())
 {
  cout << "Unable to open file \"" << filename << "\" !\n";
  return 1;
 }

 // reading head
 char riff[4];
 char wave[4];
 file.read(riff, 4);
 file.read(reinterpret_cast<char *>(&size), 4);
 file.read(wave, 4);
 if ((riff[0]!='R') || (riff[1]!='I') || (riff[2]!='F') || (riff[3]!='F'))
 {
  cout << "\"" << filename << "\" is not a valid WAVE file !\n";
  return 1;
 }
 if ((wave[0]!='W') || (wave[1]!='A') || (wave[2]!='V') || (wave[3]!='E'))
 {
  cout << "\"" << filename << "\" is not a valid WAVE file !\n";
  return 1;
 }
 // reading fmt block
 char blockName[4];
 uint32_t blockSize;
 while (!file.eof())
 {
  file.read(blockName, 4);
  file.read(reinterpret_cast<char *>(&blockSize), 4);
  if ((blockName[0]!='f') || (blockName[1]!='m') || (blockName[2]!='t') || (blockName[3]!=' '))
  {
   cout << "Skipping block.\n";
   file.seekg(blockSize, ios_base::cur);
  }
  else break;
 }
 fmtSize=blockSize;

 file.read(reinterpret_cast<char *>(&audioFormat), 2);
 file.read(reinterpret_cast<char *>(&nbCanals), 2);
 file.read(reinterpret_cast<char *>(&freq), 4);
 if (freq<44100)
 {
  cout << "Frequency should not be inferior to 44100 Hz !\n";
  return 2;
 }
 file.read(reinterpret_cast<char *>(&bytePerSec), 4);
 file.read(reinterpret_cast<char *>(&bytePerBloc), 2);
 file.read(reinterpret_cast<char *>(&bitsPerSample), 2);
 if (audioFormat!=1)
 {
  cout << "Audio format is not PCM !\n";
  return 2;
 }
 if ((nbCanals!=1) && (nbCanals!=2))
 {
  cout << "Audio sould be mono or stereo (it has " << nbCanals << " canals)\n";
  return 2;
 }
 samples.resize(nbCanals, vector<double>());
 if (bitsPerSample!=16)
 {
  cout << "Bites/sample must be 16 !\n";
  return 2;
 }

 // reading data block
 while (!file.eof())
 {
  file.read(blockName, 4);
  file.read(reinterpret_cast<char *>(&blockSize), 4);
  if ((blockName[0]!='d') || (blockName[1]!='a') || (blockName[2]!='t') || (blockName[3]!='a'))
  {
   cout << "skipping block.\n";
   file.seekg(blockSize, ios_base::cur);
  }
  else break;
 }
 dataSize=blockSize;
 unsigned int i=0;
 while (i<dataSize)
 {
  showPercent(double(i)/dataSize);
  for (unsigned int c=0; c<nbCanals; c++)
  {
   int16_t buf;
   file.read(reinterpret_cast<char *>(&buf), 2);
   samples[c].push_back(buf);
  }
  i+=bytePerBloc;
 }

 if (nbCanals==1)
 {
  samples.push_back(samples[0]);
  nbCanals++;
  dataSize*=2;
  bytePerSec*=2;
  bytePerBloc*=2;
 }
 return 0;
}

/*double abs(double x)
{
 if (x>=0) return x;
 else return -x;
}*/

double max(double a, double b)
{
 if (a>b) return a;
 else return b;
}

void Audio::center()
{
 cout << "Removing DC offset...\n";
 for (unsigned int c=0; c<nbCanals; c++)
 {
  double delta=0;
  for (unsigned int i=0; i<samples[c].size(); i++)
  {
   showPercent((double(i/2+c*samples[c].size()))/(nbCanals*samples[0].size()-1));
   delta+=samples[c][i];
  }
  double mod=-delta/samples[c].size();
  for (unsigned int i=0; i<samples[c].size(); i++)
  {
   showPercent((double((i+samples[c].size())/2+c*samples[c].size()))/(nbCanals*samples[0].size()-1));
   samples[c][i]+=mod;
  }
 }
}

void Audio::normalize(double maxAmp)
{
 center();
 cout << "Normalizing...\n";

 double maxi=0;

 for (unsigned int c=0; c<nbCanals; c++)
 {
  for (unsigned int i=0; i<samples[c].size(); i++)
  {
   showPercent((double(i+c*samples[c].size())/2)/(nbCanals*samples[0].size()-1));
   maxi=max(maxi, abs(samples[c][i]));
  }
 }

 for (unsigned int c=0; c<nbCanals; c++)
 {
  for (unsigned int i=0; i<samples[c].size(); i++)
  {
   showPercent((double(i+c*samples[c].size())/2+nbCanals*samples[0].size()/2)/(nbCanals*samples[0].size()-1));
   samples[c][i]*=maxAmp/maxi;
  }
 }
}

void Audio::writeToFile(string filename)
{
 normalize(32767);
 cout << "Writing file \"" << filename << "\"...\n";
 vector<vector<int16_t> > samples16;

 for (unsigned int c=0; c<nbCanals; c++)
 {
  samples16.push_back(vector<int16_t>());
  for (unsigned int i=0; i<samples[c].size(); i++)
  {
   showPercent((double(i+c*samples[c].size()))/(nbCanals*samples[0].size()-1));
   vector<int16_t> buf;
   samples16[c].push_back(round(samples[c][i]));
  }
 }


 ofstream file(filename.c_str(), ios::out|ios::trunc|ios::binary);

 // writing haeder
 file.write(string("RIFF").c_str(), 4);
 size=dataSize+44;
 file.write(reinterpret_cast<char *>(&size), 4);
 file.write(string("WAVE").c_str(), 4);

 // writing fmt block
 file.write(string("fmt ").c_str(), 4);
 file.write(reinterpret_cast<char *>(&fmtSize), 4);
 file.write(reinterpret_cast<char *>(&audioFormat), 2);
 file.write(reinterpret_cast<char *>(&nbCanals), 2);
 file.write(reinterpret_cast<char *>(&freq), 4);
 file.write(reinterpret_cast<char *>(&bytePerSec), 4);
 file.write(reinterpret_cast<char *>(&bytePerBloc), 2);
 file.write(reinterpret_cast<char *>(&bitsPerSample), 2);

 // writing data block
 file.write(string("data").c_str(), 4);
 file.write(reinterpret_cast<char *>(&dataSize), 4);
 for (unsigned int i=0; i<samples16[0].size(); i++)
 {
  for (unsigned int c=0; c<nbCanals; c++)
  {
   file.write(reinterpret_cast<char *>(&samples16[c][i]), 2);
  }
 }
}

void Audio::showInfo()
{
 cout << "FileSize : " << size << endl;
 cout << "Fmt Size : " << fmtSize << endl;
 cout << "DataSize : " << dataSize << endl;
 cout << "Duration : " << dataSize/bytePerSec/60 << "'" << dataSize/bytePerSec%60 << "\"" << endl;

 cout << "Fmt block :\n";
 cout << "audioFmt : " << audioFormat << endl;
 cout << "nbCanals : " << nbCanals << endl;
 cout << "Freq     : " << freq << endl;
 cout << "bytesSec : " << bytePerSec << endl;
 cout << "bytesBlk : " << bytePerBloc << endl;
 cout << "bitsSmpl : " << bitsPerSample << endl;
}
