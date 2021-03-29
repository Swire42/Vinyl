#pragma once
#include <iostream>
#include <vector>

using namespace std;

#define FILTER_NO_OUTPUT 0
#define FILTER_LINEAR 1
#define FILTER_RIAA_LECT 2
#define FILTER_RIAA_ENRG 3

struct Filter
{
 vector<double> a, b;
 int type;

 Filter();
 Filter(vector<double> _a, vector<double> _b);
 Filter(int filter_type, double freqEch=44100);

 vector<double> applyOn(vector<double> input);
};
