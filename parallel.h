#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <queue>
#include <thread>
#include <atomic>

using namespace std;
struct page {
  string noun;
  int id;
  map<string, int> words;
  int pglength;
  page(string noun_, vector<string> intomap) : noun(noun_) {
    pglength = intomap.size();
    for (auto i : intomap) {
      string temp;
      for (auto j: i) {
        if (int(j) >= 65 && int(j) <= 90) {
          temp.push_back(tolower(j));
        } else if (int(j) >= 97 && int(j) <= 122) {
          temp.push_back(j);
        }
      }
      if (temp.empty()) {
        //cout<< i <<endl;
      } else if (temp != "or" && temp!= "the" && temp!= "and" && temp!= "a" && temp!= "with" && temp!= "that" && temp!= "an" && temp!= " References" && temp!= "Category" &&  temp!= "or" &&  temp!= "we" && temp!= "is" && temp!= "can" &&temp!= "in" && temp!= "to") {
        words[temp]++;
        //cout<<i<<endl;
      }
      
      
    }
  }
};

struct arg_struct {
  int id;
  string pgname;
  arg_struct(int id_, string pgname_): id(id_), pgname(pgname_){};
};

class wiki {
 public:
  wiki(const vector<string>& pagenames);
    void getgrid();
    vector<string> retruneight();
    
 protected:
  void wiki::bfs_on_lvl(vector<int> start, vector<bool> visited);
  string extract_word(string name);
  vector<pair<vector<int>, float>> best_with_start;
  vector<vector<float>> grid;  // 2d grid of intersections(same words) of pages
  vector<string> getpage(const string &pagename);
  int num_pgs;
  vector<page*> allpages;
  int mynum = 7;
  void compare(page* page);
  void *pg_init(arg_struct* args);
};