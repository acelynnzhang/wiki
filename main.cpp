#include "parallel.h"
#include "start.h"

using namespace chrono;

int main() {
  vector<string> inputwords;
  // filter_out_noun("./wiki2-nouns", "./onlynoun");
  //  int count = 0;
  string path = "./onlynoun";
  int count = 200;
  for (const auto& entry : filesystem::directory_iterator(path)) {
    count--;
    inputwords.push_back(entry.path());
    // cout << entry.path() << endl;
    if (count == 0) {
      break;
    }
  }

  if (inputwords.size() == 0) {
    return 0;
  }



  auto start2 = high_resolution_clock::now();
  bfs mywiki = bfs(inputwords);
  auto stop2 = high_resolution_clock::now();
  auto duration2 = duration_cast<milliseconds>(stop2 - start2);
  cout << "build no threads " <<duration2.count() << " ms" << endl;


  auto start = high_resolution_clock::now();
  parallelbfs mywiki2 = parallelbfs(inputwords);
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(stop - start);
  cout << "build /w threads "  << duration.count() << " ms" << endl;
  mywiki2.parallelbfs::search();
  //   // vector<string> out = mywiki2.retruneight();
  // for (int i = 0; i < out.size(); i++) {
  //   cout << "@ i " << i << " = " << out[i] << endl;
  // }


  
  return 1;
}


