#ifndef WIKI_H
#define WIKI_H

#pragma once

#include <algorithm>
#include <atomic>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <deque>

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
      for (auto j : i) {
        if (int(j) >= 65 && int(j) <= 90) {
          temp.push_back(tolower(j));
        } else if (int(j) >= 97 && int(j) <= 122) {
          temp.push_back(j);
        }
      }
      if (temp.empty()) {
        // cout<< i <<endl;
      } else if (temp != "or" && temp != "the" && temp != "and" &&
                 temp != "a" && temp != "with" && temp != "that" &&
                 temp != "an" && temp != " References" && temp != "Category" &&
                 temp != "or" && temp != "we" && temp != "is" &&
                 temp != "can" && temp != "in" && temp != "to") {
        words[temp]++;
        // cout<<i<<endl;
      }
    }
  }
};

struct arg_struct {
  int id;
  string pgname;
  arg_struct(int id_, string pgname_) : id(id_), pgname(pgname_){};
};

class wiki {
 public:
  wiki(){};
  void getgrid();
  vector<string> retruneight();
  ~wiki();

 protected:

  float sumvec(vector<float> v);
  vector<float> addvecf(vector<float> fir, vector<float> sec, vector<int> path);
  void sortnoun(string pathh, string changeto);
  map<string, string> genmap(string path);
  string remove_punct(const string& str);
  string extract_word(string name);
  vector<string> getpage(const string& pagename);
  
  vector<vector<float>> grid;  // 2d grid of intersections(same words) of pages
  int num_pgs;
  vector<page*> allpages;
  int mynum = 7;
};

#endif