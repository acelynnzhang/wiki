// download list of nouns
// be able to pull up wiki page into readable form
#define PY_SSIZE_T_CLEAN
// #include <Python.h>

#include "main.h"

#include <stdlib.h>

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>

using namespace std;

// https://en.wikipedia.org/wiki/Zhejiangopterus ,
// https://en.wikipedia.org/wiki/Arambourgiania -> for close but obscure

map<string, string> genmap(string path) {
  map<string, string> nounmap;
  ifstream words(path);
  string curr;
  // vector<string> out;
  if (words.is_open()) {
    while (getline(words, curr, '\n')) {
      nounmap[curr] = "";
    }
  }
  cout << "finish nounmap" << endl;
  return nounmap;
}

void sortnoun(string pathh, string changeto) {
  string path = pathh;
  map<string, string> nounmap = genmap("nounlist.txt");

  std::filesystem::create_directories("./onlynoun");

  for (auto &entry : filesystem::directory_iterator(path)) {
    string temp = static_cast<string>(entry.path());
    string name;
    for (int k = pathh.size() + 1; k < temp.size() - 4; k++) {
      name.push_back(tolower(temp.at(k)));
    }
    // cout << name << endl;
    if (nounmap.find(name) != nounmap.end()) {
      cout << entry.file_size() << endl;
      // cout << entry.path() << endl;
      try {
        std::ifstream src(entry.path(), std::ios::binary);
        std::ofstream dst("./onlynoun/" + name + ".txt", std::ios::binary);
        // std::cout<< " moved "<< name<<endl;
        dst << src.rdbuf();
        // src.close();
        // dst.close();

      } catch (std::filesystem::filesystem_error &e) {
        std::cout << e.what() << '\n';
      }
    }
  }
}

void clean() {
  int count = 0;
  for (const auto &entry : filesystem::directory_iterator("./onlynoun")) {
    
    if (entry.file_size() < 1000) {
      remove(entry.path());
      count++;
    }
    // FILE *fp = fopen((string)entry.path());
    // fseek(fp, 0, SEEK_END);  // seek to end
    // if (ftell(fp) < 100) {
    //   remove((string)entry.path());
    //   count++;
    // } else {
    //   fclose(fp);
    // }
  }
  cout<< "killed " << count<< " files"<<endl;
}

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
      // //cout << (*word_iter) << endl;
      ++word_iter;
    }
  } else {
    // cout << "uh oh" << endl;
  }

  return out;
}

wiki::wiki(const vector<string> &pagenamen, int num) {
  mynum = num;
  sizee = pagenamen.size();
  cout << sizee << endl;
  grid.resize(sizee);
  for (size_t k = 0; k < sizee; k++) {
    string name;
    for (int m = 11; m < pagenamen[k].size() - 4; m++) {
      name.push_back(tolower(pagenamen[k].at(m)));
    }

    grid[k].resize(sizee);
    vector<string> temp = getpage(pagenamen[k]);
    // if (temp.size() )
    page *curr = new page(name, temp);
    curr->givennum = k;  // hjere

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
          grid[curr->givennum][i->givennum] +=
              curr->words[currword] / static_cast<float>(curr->pglength) +
              j.second / static_cast<float>(i->pglength);
          grid[i->givennum][curr->givennum] +=
              curr->words[currword] / static_cast<float>(curr->pglength) +
              j.second / static_cast<float>(i->pglength);
          countcommonwords++;
        }
      }
      if (countcommonwords != -1) {  // if contains name
        grid[curr->givennum][i->givennum] *= countcommonwords;
        grid[i->givennum][curr->givennum] *= countcommonwords;
      } else {
        grid[curr->givennum][i->givennum] = 9999999;
        grid[i->givennum][curr->givennum] = 9999999;
      }
      grid[curr->givennum][curr->givennum] = 9999999;
      // //cout << " = " << grid[i->givennum * curr->givennum] << endl;
    }
    allpages.push_back(curr);
  }
  cout << "finish build grid" << endl;
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

vector<string> wiki::retruneight(float &sum) {
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
  sum = pathh.begin()->first;
  for (auto i : pathh.begin()->second) {
    s << allpages[i]->name << " -> ";
    toreturn.push_back(allpages[i]->name);
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

int main() {
  vector<string> inputwords;
  clean();
  // sortnoun("./wiki-nouns", "./onlynoun");
  //  int count = 0;

  string path = "./onlynoun";
  int count = 50;
  float ceiling = 999999;
  float temp;
  vector<string> curr;
  for (const auto &entry : filesystem::directory_iterator(path)) {
    count--;
    inputwords.push_back(entry.path());
    // cout << entry.path() << endl;
    if (count == 0) {
      wiki mywiki = wiki(inputwords, 7);
      vector<string> out = mywiki.retruneight(temp);
      if (temp < ceiling) {
        ceiling = temp;
        cout << "floor ->" << ceiling << endl;
        for (int i = 0; i < inputwords.size(); i++) {
          curr.push_back(inputwords[i]);
        }
      }
      count = 50;
      inputwords.clear();
    }
  }

  wiki mywiki = wiki(curr, 7);
  vector<string> out = mywiki.retruneight(temp);
  for (int i = 0; i < out.size(); i++) {
    cout << "@ i " << i << " = " << out[i] << endl;
  }
}
