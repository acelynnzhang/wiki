#ifndef PARALLEL_H
#define PARALLEL_H

#include "wiki.h"

using namespace std;

class parallelbfs : public wiki {
 public:
  parallelbfs(const vector<string>& pagenames);

 protected:
  void bfs_on_lvl(vector<int> start, vector<bool> visited);
  vector<pair<vector<int>, float>> best_with_start;
  void compare(page* page);
  void* pg_init(arg_struct* args);
};

#endif