#include "audio.hpp"
#include "diskinfo.hpp"

using namespace std;

#define disp(a) cout << #a << ":" << a << endl

int main()
{
 Audio myFile;
 cout << "file : " << flush;
 string filename;
 cin >> filename;
 if (myFile.readFromFile(filename)==0)
 {
  myFile.showInfo();
  cout << myFile.samples[0].size() << endl;
  myFile.applyEffects();
  string outfile=filename;
  outfile.pop_back();
  outfile.pop_back();
  outfile.pop_back();
  outfile.pop_back();
  myFile.writeToFile(outfile+"-vinyl.wav");

  /*Disk testDisk(DISK_33T);
  disp(testDisk.maxTime());
  disp(testDisk.getRad(0));
  disp(testDisk.getRad(20*60));
  disp(testDisk.getInterEchDist(15*CENTI, 44100));
  disp(testDisk.getInterEchDist(5*CENTI, 44100));
  disp(testDisk.getArmGrooveAngle(15*CENTI));
  disp(testDisk.getArmGrooveAngle(5*CENTI));*/
 }
}
