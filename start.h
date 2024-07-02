#ifndef START_H
#define START_H

#include "wiki.h"

using namespace std;

class bfs : public wiki {
 public:
  bfs(const vector<string>& pagenamen);
  pair<vector<int>, float> bfsnarrow(vector<int> start, float tsum,
                                     vector<int> select);
  vector<string> retruneight();
};

#endif