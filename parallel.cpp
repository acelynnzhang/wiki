// download list of nouns
// be able to pull up wiki page into readable form

#include "parallel.h"

using namespace std;



string remove_punct(const string &str) {
  string ret;
  remove_copy_if(str.begin(), str.end(), std::back_inserter(ret),
                 [](int c) { return std::ispunct(c); });
  return ret;
}

vector<string> wiki::getpage(const string &pagename) {
  ifstream words(pagename);
  vector<string> out;
  if (words.is_open()) {
    std::istream_iterator<string> word_iter(words);
    while (!words.eof()) {
      out.push_back(remove_punct(*word_iter));
      ++word_iter;
    }
  } else {
    cout << "file " << pagename << " failed to open" <<endl;
  }

  return out;
}

string wiki::extract_word(string prev_name) {
  string name;
  for (int m = 11; m < prev_name.size() - 4; m++) {
      name.push_back(tolower(prev_name.at(m)));
    }
  return name;
}


void *wiki::pg_init(arg_struct* args) {
  string page_name = args->pgname;
  int id = args->id;
  string noun = extract_word(page_name);
  vector<string> contents = getpage(page_name);
  page *curr = new page(noun, contents);
  curr->id = id;
  allpages[id] = curr; 
  return NULL;
}


wiki::wiki(const vector<string> &pagenames) {
  num_pgs = pagenames.size();
  grid.resize(num_pgs);
  allpages.resize(num_pgs);
  thread threads[num_pgs];
  for (size_t k = 0; k < num_pgs; k++) {
    //cout<< pagenames[k] << endl;
    struct arg_struct *args = new arg_struct(k,pagenames[k]);
    threads[k] = thread(&wiki::pg_init,this,args);
  }
  for (int i = 0; i < num_pgs; i++) {
    threads[i].join();
  }

  // for(int i = 0; i < num_pgs; i++){
  //   cout << allpages[i]->noun<<endl;
  // }
}

 void wiki::compare(vector<vector<int>> grid, page* curr) {
    for (auto i : allpages) {  // other pages
      int countcommonwords = 0;
      for (auto j : i->words) {  // interate through map of other page
        string currword = j.first;
        ////cout << currword << endl;
        if (currword == name) {
          countcommonwords = -1;
          break;
        }
        if (curr->words.find(currword) != curr->words.end()) {
          grid[curr->id][i->id] +=
              curr->words[currword] / static_cast<float>(curr->pglength) +
              j.second / static_cast<float>(i->pglength);
          grid[i->id][curr->id] +=
              curr->words[currword] / static_cast<float>(curr->pglength) +
              j.second / static_cast<float>(i->pglength);
          countcommonwords++;
        }
      }
      if (countcommonwords != -1) {  // if contains name
        grid[curr->id][i->id] *= countcommonwords;
        grid[i->id][curr->id] *= countcommonwords;
      } else {
        grid[curr->id][i->id] = 9999999;
        grid[i->id][curr->id] = 9999999;
      }
      grid[curr->id][curr->id] = 9999999;
      // //cout << " = " << grid[i->id * curr->id] << endl;
    }
 }



vector<float> addvecf(vector<float> fir, vector<float> sec) {
  vector<float> toreturn;
  int f = 0;
  int s = 0;
  for (int i = 0; i < max(fir.size(), sec.size()); i++) {
    if (i >= fir.size() || fir[i] == 9999999) {
      f = 0;
    } else {
      f = fir[i];
    }
    if (i >= sec.size() || sec[i] == 9999999) {
      s = 0;
    } else {
      s = sec[i];
    }
    toreturn.push_back(f + s);
  }
  return toreturn;
}

float sumvec(vector<float> v) {
  float toreturn;
  for (auto i : v) {
    if (i != 9999999) {
      toreturn += i;
    }
  }
  return toreturn;
}

vector<string> wiki::retruneight() {
  vector<string> toreturn;
  // vector<vector<pair<float, int>>> weightgrid;
  map<float, vector<int>> pathh;

  for (int i = 0; i < grid.size();
       i++) {  //+= weight of things added to see whats the nextt best
    
    vector<bool> seen(grid.size(), false);
    vector<int> temp;
    vector<float> currvec = grid[i];
    float sum = 0;
    temp.push_back(i);
    seen[i] = true;
    int indxnext = -1;
    while (temp.size() < mynum) {
      int currsmall = 99999;
      for (int j = 0; j < grid.size(); j++) {
        int save = sumvec(addvecf(currvec, grid[j]));
        if (!seen[j] && save < currsmall) {
          currsmall = save;
          indxnext = j;
        }
      }

      for (int k = 0; k < temp.size(); k++) {
        sum += grid[temp[k]][indxnext];
      }
      temp.push_back(indxnext);
      seen[indxnext] = true;
      currvec = addvecf(currvec, grid[indxnext]);
    }

    if (temp.size() == mynum) {
      pathh[sum] = temp;
    } else {
      // cout << "  hmmmm " << endl;
    }
  }

  cout << " finsih first run throgh" << endl;
  vector<vector<int>> setoftoreturn;

  int count = 0;
  vector<int> select;
  vector<float> tsums;
  for (auto i : pathh) {
    vector<int> temp;
    stringstream ss;
    // ss<<count << " : ";
    select.push_back(i.second[0]);
    for (auto j : i.second) {
      // ss<< allpages[j]->name << " -> ";
      temp.push_back(j);
    }
    // //cout<<ss.str()<<endl;
    tsums.push_back(i.first);
    setoftoreturn.push_back(temp);
    count++;
    if (count >= 20) {
      break;
    }
  }

  cout << "  b4 bfs" << endl;
  map<float, vector<int>> pathhnew;
  for (int i = 0; i < setoftoreturn.size(); i++) {
    pair<vector<int>, float> trythis =
        bfsnarrow(setoftoreturn[i], tsums[i], select);
    if (!trythis.first.empty()) {
      pathhnew[trythis.second] = trythis.first;
    }
  }

  cout << " sum " << pathh.begin()->first << endl;
  stringstream s;
  s << "chosen: ";
  for (auto i : pathh.begin()->second) {
    s << allpages[i]->noun << " -> ";
    toreturn.push_back(allpages[i]->noun);
  }

  cout << s.str() << endl;
  return toreturn;
}

pair<vector<int>, float> wiki::bfsnarrow(vector<int> start, float tsum,
                                         vector<int> select) {
  vector<int> toreturn;
  map<float, bool> seensums;
  queue<vector<int>> tocheck;
  queue<vector<bool>> seen;
  queue<float> sums;
  vector<int> temp;
  temp.push_back(start[0]);
  tocheck.push(temp);
  vector<bool> tempo(grid.size(), false);
  tempo[start[0]] = true;
  seen.push(tempo);
  sums.push(0);

  while (!tocheck.empty()) {
    // printveci(tocheck.front());

    if (tocheck.front().size() < mynum && sums.front() < tsum) {
      vector<int> curr = tocheck.front();
      float currsum = sums.front();
      vector<bool> currseen = seen.front();
      for (int i = 0; i < select.size(); i++) {
        if (currseen[select[i]] == false) {
          float tempsum = currsum;
          for (int k = 0; k < curr.size(); k++) {
            tempsum += grid[curr[k]][select[i]];
          }

          if (tempsum < tsum && seensums.find(tempsum) == seensums.end()) {
            seensums[tempsum] = true;
            vector<int> newpath = curr;
            newpath.push_back(select[i]);
            vector<bool> newseen = currseen;
            newseen[select[i]] = true;
            if (newpath.size() == mynum) {
              // printveci(newpath);
              tsum = tempsum;
              toreturn = newpath;
            } else {
              seen.push(newseen);
              sums.push(tempsum);
              tocheck.push(newpath);
            }
          }
        }
      }
    }
    tocheck.pop();
    sums.pop();
    seen.pop();
  }

  // cout<< " tsum " << tsum<<endl;
  return pair<vector<int>, float>(toreturn, tsum);
}

pair<vector<int>, float> wiki::bfs(vector<int> start, float tsum) {
  vector<int> toreturn;
  map<float, bool> seensums;
  queue<vector<int>> tocheck;
  queue<vector<bool>> seen;
  queue<float> sums;
  vector<int> temp;
  temp.push_back(start[0]);
  tocheck.push(temp);
  vector<bool> tempo(grid.size(), false);
  tempo[start[0]] = true;
  seen.push(tempo);
  sums.push(0);

  while (!tocheck.empty()) {
    // printveci(tocheck.front());

    if (tocheck.front().size() < mynum && sums.front() < tsum) {
      vector<int> curr = tocheck.front();
      float currsum = sums.front();
      vector<bool> currseen = seen.front();
      for (int i = 0; i < grid.size(); i++) {
        if (currseen[i] == false) {
          float tempsum = currsum;
          for (int k = 0; k < curr.size(); k++) {
            tempsum += grid[curr[k]][i];
          }

          if (tempsum < tsum && seensums.find(tempsum) == seensums.end()) {
            seensums[tempsum] = true;
            vector<int> newpath = curr;
            newpath.push_back(i);
            vector<bool> newseen = currseen;
            newseen[i] = true;
            if (newpath.size() == mynum) {
              // printveci(newpath);
              tsum = tempsum;
              toreturn = newpath;
            } else {
              seen.push(newseen);
              sums.push(tempsum);
              tocheck.push(newpath);
            }
          }
        }
      }
    }
    tocheck.pop();
    sums.pop();
    seen.pop();
  }

  // cout<< " tsum " << tsum<<endl;
  return pair<vector<int>, float>(toreturn, tsum);
}

int main() {
  vector<string> inputwords;
  //filter_out_noun("./wiki-nouns", "./onlynoun");
  // int count = 0;
  string path = "./onlynoun";
  int count = 200; 
  for (const auto &entry : filesystem::directory_iterator(path)) {
    count --;
    inputwords.push_back(entry.path());
    // cout << entry.path() << endl;
      // if (count == 0) {
      //   break;
      // }
  }

  if (inputwords.size() ==0) {

    return 0;  }
  wiki mywiki = wiki(inputwords);
  // vector<string> out = mywiki.retruneight();
  // for (int i = 0; i < out.size(); i++) {
  //   cout << "@ i " << i << " = " << out[i] << endl;
  // }
  return 1;
}
