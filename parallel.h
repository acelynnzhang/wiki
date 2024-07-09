#ifndef PARALLEL_H
#define PARALLEL_H

#include "wiki.h"

using namespace std;

class parallelbfs : public wiki {
 public:
  parallelbfs(const vector<string>& pagenames);
  void search();
  
 protected:
  
  void bfs(vector<int> start, vector<float> weights, float* lowestsum,vector<int>*  path);
  void bfs_on_lvl(vector<int> start, vector<float> weights, atomic<vector<int> *> &a_path, atomic<float *> & a_best);
  void compare(page* page);
  void* pg_init(arg_struct* args);
};

#endif