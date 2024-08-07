#include "parallel.h"
using namespace chrono;
using namespace std;

void *parallelbfs::pg_init(arg_struct *args) {
  string page_name = args->pgname;
  int id = args->id;
  string noun = extract_word(page_name);
  vector<string> contents = getpage(page_name);
  page *curr = new page(noun, contents);
  curr->id = id;
  allpages[id] = curr;
  return NULL;
}

parallelbfs::parallelbfs(const vector<string> &pagenames) {
  num_pgs = pagenames.size();
  grid.resize(num_pgs);
  allpages.resize(num_pgs);
  thread threads[num_pgs];
  vector<arg_struct *> argall(num_pgs);
  for (size_t k = 0; k < num_pgs; k++) {
    // cout<< pagenames[k] << endl;
    struct arg_struct *args = new arg_struct(k, pagenames[k]);
    argall[k] = args;
    threads[k] = thread(&parallelbfs::pg_init, this, args);
  }
  for (int i = 0; i < num_pgs; i++) {
    threads[i].join();
    delete argall[i];
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

void parallelbfs::compare(page *curr) {
  grid[curr->id].resize(num_pgs);
  for (auto i : allpages) { // other pages
    if (i->noun != curr->noun) {
      int countcommonwords = 0;
      for (auto j : i->words) { // interate through map of other page
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
      if (countcommonwords != -1) { // if contains name
        grid[curr->id][i->id] *= countcommonwords;
      } else {
        grid[curr->id][i->id] = static_cast<float>(INT32_MAX);
      }
      grid[curr->id][curr->id] = 0;
    }
  }
}

// •each operation is executed in entirety or not executed at all.(ATOMIC)

void parallelbfs::search() {
  vector<vector<int> *> paths(num_pgs);
  vector<float *> sums(num_pgs);
  thread threads[num_pgs];

  for (int i = 0; i < num_pgs; i++) {
    sums[i] = new float(static_cast<float>(INT32_MAX));
    paths[i] = new vector<int>(num_pgs);
    vector<int> curr{i};
    vector<float> weight = grid[i];
    threads[i] = thread(&parallelbfs::bfs, this, curr, weight, ref(sums[i]),
                        ref(paths[i]));
  }

  for (int i = 0; i < num_pgs; i++) {
    threads[i].join();
  }

  float currsmall = maxfloat;
  vector<int> currpath;
  for (int i = 0; i < num_pgs; i++) {
    //cout<< " sum "<< *sums[i] << ' '<<endl;
    printpath(*paths[i]);
    if (*sums[i] < currsmall) {
      currpath = *paths[i];
      currsmall = *sums[i];
    }
    // delete sums[i];
    // delete paths[i];
  }

  cout << "final= " << currsmall << endl;
  printpath(currpath);
}

void parallelbfs::bfs(vector<int> start, vector<float> weights,
                      float *lowestsum, vector<int> *path) {
  if (start[0] == 1) {
  }
  if (start.size() == 3) {
    float tot = sumvec(weights, start);
    if ( tot< *lowestsum) {
      printpath(start);
      *lowestsum = tot;
      *path = start;

      cout << "\n change" << "loewsetsum "<<*lowestsum <<"\n" << endl;
      printpath(*path);
    }
    return;
  } else {
    int currloc = 0;
    for (int i = 0; i < num_pgs; i++) {
      // if (start[0] == 1) {
      //   cout << "i = " << i << "vs currloc = " << start[currloc] << endl;
      //   // cout<< "\n -------------------- \n" <<endl;
      // }
      if (i == start[currloc]) { // if already in path
        // if (start[0] == 1) {
        //   printpath(start);
        //   cout << "currloc now " << start[currloc] << endl;
        // }
        if (currloc < start.size() - 1) {
          currloc++;
        }

      } else {
        vector<int> start2 = start;
        vector<int>::iterator iter = start2.begin();
        // if (start[0] == 1) {
        //   cout << "adding " << i << endl;
        // }

        for (int k = 0; k < start2.size(); k++) { // insert in order
          if (i < start2[k]) {
            start2.insert(iter, i);
            break;
          }
          iter++;
        }
        if (start2.size() == start.size()) {
          start2.push_back(i);
        }
        // if (start[0] == 1) {
        //   cout<<"path after" <<endl;
        //   printpath(start2);
        // }

        vector<float> weights2 = addvecf(weights, grid[i]);
        bfs(start2, weights2, lowestsum, path);
      }
    }
  }
  return;
}

int main() {
  vector<string> inputwords;
  // filter_out_noun("./wiki2-nouns", "./onlynoun");
  //  int count = 0;
  string path = "./onlynoun";
  int count = 10;
  for (const auto &entry : filesystem::directory_iterator(path)) {
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
  auto start = high_resolution_clock::now();
  parallelbfs mywiki2 = parallelbfs(inputwords);
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(stop - start);
  cout << "build /w threads " << duration.count() << " ms" << endl;

  mywiki2.getgrid();
  mywiki2.search();
  return 1;
}

// void parallelbfs::search() {
//   vector<vector<int> *> paths(num_pgs);
//   vector<float *> sums(num_pgs);
//   thread threads[num_pgs];

//   for (int i = 0; i < num_pgs; i++) {
//     sums[i] = new float(static_cast<float>(INT32_MAX));
//     paths[i] = new vector<int>(num_pgs);
//     atomic<vector<int> *> a_path(paths[i]);
//     atomic<float *> a_best(sums[i]);
//     vector<int> curr{i};
//     vector<float> weight = grid[i];
//     threads[i] = thread(&parallelbfs::bfs_on_lvl, this, curr, weight,
//     ref(a_path), ref(a_best));
//   }

//   for (int i = 0; i < num_pgs; i++) {
//     threads[i].join();
//   }

//   float currmax = 0;
//   vector<int> currpath;
//   for (int i = 0; i < num_pgs; i++) {
//     if (*sums[i] > currmax) {
//       currpath = *paths[i];
//     }
//   }

//   cout << "sum=" << currmax << endl;
//   stringstream s;
//   s << "chosen: ";
//   for (auto i : currpath) {
//     s << allpages[i]->noun << " -> ";
//   }
//   cout << s.str() << endl;
// }

// void parallelbfs::bfs_on_lvl(vector<int> start, vector<float> weights,
//                              atomic<vector<int> *> &a_path,
//                              atomic<float *> &a_best) {
//   if (start.size() == mynum) {
//     float *localbest = new float(sumvec(weights));
//     vector<int> *localpath = new vector(start);
//     float *currval = a_best.load();

//     while (*currval < *localbest) {
//       if (a_best.compare_exchange_strong(currval, localbest)) {
//         delete currval;
//         vector<int> *prevpath = a_path.load();
//         while (a_best.load() == localbest) {
//           a_path.compare_exchange_strong(prevpath, localpath);
//           delete prevpath;
//           break;
//         }
//         break;
//       }
//       float *currval =
//           a_best.load();  // check if value has updated and compare to that
//     }

//   } else {
//     thread threads[num_pgs];
//     int currloc = 0;
//     for (int i = 0; i < num_pgs; i++) {
//       if (i == start[currloc]) {  // if already in path
//         currloc++;
//       } else {
// vector<int> start2 = start;
// vector<int>::iterator iter = start2.begin();
// for (iter; iter < start2.end(); iter++) { //insert in order
//   if(i < *iter) {
//     start2.insert(iter, i);
//   }
// }
// start2.push_back(i);
// vector<float> weights2 = addvecf(weights, grid[i], start2);
//         threads[i] = thread(&parallelbfs::bfs_on_lvl, this, start2, weights2,
//                             ref(a_path), ref(a_best));
//       }
//     }
//   }
// }