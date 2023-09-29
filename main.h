#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
struct page {
  string name;
  int givennum;
  map<string, int> words;
  int pglength;
  page(string name_, vector<string> intomap) : name(name_) {
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

class wiki {
 public:
  wiki(const vector<string>& pagenames);
    vector<vector<float>> getgrid() { return grid; }
    vector<string> retruneight();
    
 private:
 // void genmap();
  //void wiki::sortnoun(string path, string change);
  pair<vector<int>, float> bfs(vector<int> start, float tsum);
  pair<vector<int>, float> bfsnarrow(vector<int> start, float tsum, vector<int> select);
   // map<string, string> nounmap;
  //vector<string> getfilename(const vector<string>& pagenames);
  vector<float> gridd;
  vector<vector<float>> grid;  // 2d grid of intersections(same words) of pages
  vector<string> getpage(const string &pagename);
  int sizee;
  vector<page*> allpages;
  int mynum = 7;
  
};