#include "parallel.h"

using namespace std;

void* parallelbfs::pg_init(arg_struct* args) {
  string page_name = args->pgname;
  int id = args->id;
  string noun = extract_word(page_name);
  vector<string> contents = getpage(page_name);
  page* curr = new page(noun, contents);
  curr->id = id;
  allpages[id] = curr;
  return NULL;
}

parallelbfs::parallelbfs(const vector<string>& pagenames) {
  num_pgs = pagenames.size();
  grid.resize(num_pgs);
  allpages.resize(num_pgs);
  thread threads[num_pgs];
  for (size_t k = 0; k < num_pgs; k++) {
    // cout<< pagenames[k] << endl;
    struct arg_struct* args = new arg_struct(k, pagenames[k]);
    threads[k] = thread(&parallelbfs::pg_init, this, args);
  }
  for (int i = 0; i < num_pgs; i++) {
    threads[i].join();
  }

  // for (int i = 0; i < num_pgs; i++) {
  //   cout << '[' << allpages[i]->noun << " : " << allpages[i]->id << ']' <<
  //   endl;
  // }
  for (auto pg : allpages) {
    int i = pg->id;
    threads[i] = thread(&parallelbfs::compare, this, pg);
  }

  for (int i = 0; i < num_pgs; i++) {
    threads[i].join();
  }
}

void parallelbfs::compare(page* curr) {
  grid[curr->id].resize(num_pgs);
  for (auto i : allpages) {  // other pages
    if (i->noun != curr->noun) {
      int countcommonwords = 0;
      for (auto j : i->words) {  // interate through map of other page
        string currword = j.first;
        if (currword == curr->noun) {
          countcommonwords = -1;
          break;
        }
        if (curr->words.find(currword) != curr->words.end()) {
          grid[curr->id][i->id] +=
              curr->words[currword] / static_cast<float>(curr->pglength) +
              j.second / static_cast<float>(i->pglength);
          countcommonwords++;
        }
      }
      if (countcommonwords != -1) {  // if contains name
        grid[curr->id][i->id] *= countcommonwords;
      } else {
        grid[curr->id][i->id] = static_cast<float>(INT32_MAX);
      }
      grid[curr->id][curr->id] = static_cast<float>(INT32_MAX);
    }
  }
}

// vector<float> addvecf(vector<float> fir, vector<float> sec) {
//   vector<float> toreturn;
//   for (int i = 0; i < max(fir.size(), sec.size()); i++) {
//     toreturn.push_back(fir[i] + sec[i]);
//   }
//   return toreturn;
// }

// float sumvec(vector<float> v) {
//   float toreturn;
//   for (auto i : v) {
//     if (i != static_cast<float>(INT32_MAX)) {
//       toreturn += i;
//     }
//   }
//   return toreturn;
// }

// vector<string> wiki2::retruneight() { atomic<bool> atmarr[num_pgs]; }

void parallelbfs::bfs_on_lvl(vector<int> start, vector<bool> visited) {}
