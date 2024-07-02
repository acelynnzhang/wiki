// download list of nouns
// be able to pull up wiki page into readable form

#include "parallel.h"

using namespace std;

void wiki::getgrid() {
  stringstream s;
  s << '[';
  for (auto row : grid) {
    s << '[';
    for (auto entry : row) {
      s << entry << ",";
    }
    s << ']';
  }
  s << ']';
  cout << s.str() << endl;
}

string remove_punct(const string& str) {
  string ret;
  remove_copy_if(str.begin(), str.end(), std::back_inserter(ret),
                 [](int c) { return std::ispunct(c); });
  return ret;
}

vector<string> wiki::getpage(const string& pagename) {
  ifstream words(pagename);
  vector<string> out;
  if (words.is_open()) {
    std::istream_iterator<string> word_iter(words);
    while (!words.eof()) {
      out.push_back(remove_punct(*word_iter));
      ++word_iter;
    }
  } else {
    cout << "file " << pagename << " failed to open" << endl;
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

void* wiki::pg_init(arg_struct* args) {
  string page_name = args->pgname;
  int id = args->id;
  string noun = extract_word(page_name);
  vector<string> contents = getpage(page_name);
  page* curr = new page(noun, contents);
  curr->id = id;
  allpages[id] = curr;
  return NULL;
}

wiki::wiki(const vector<string>& pagenames) {
  num_pgs = pagenames.size();
  grid.resize(num_pgs);
  allpages.resize(num_pgs);
  thread threads[num_pgs];
  for (size_t k = 0; k < num_pgs; k++) {
    // cout<< pagenames[k] << endl;
    struct arg_struct* args = new arg_struct(k, pagenames[k]);
    threads[k] = thread(&wiki::pg_init, this, args);
  }
  for (int i = 0; i < num_pgs; i++) {
    threads[i].join();
  }

  for (int i = 0; i < num_pgs; i++) {
    cout << '[' << allpages[i]->noun << " : " << allpages[i]->id << ']' << endl;
  }

  // grid[0].resize(num_pgs);
  // compare(grid[0], allpages[1]);

  for (auto pg : allpages) {
    int i = pg->id;
    threads[i] = thread(&wiki::compare, this, pg);
  }

  for (int i = 0; i < num_pgs; i++) {
    threads[i].join();
  }
}
void wiki::compare(page* curr) {
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

vector<float> addvecf(vector<float> fir, vector<float> sec) {
  vector<float> toreturn;
  for (int i = 0; i < max(fir.size(), sec.size()); i++) {
    toreturn.push_back(fir[i] + sec[i]);
  }
  return toreturn;
}

float sumvec(vector<float> v) {
  float toreturn;
  for (auto i : v) {
    if (i != static_cast<float>(INT32_MAX)) {
      toreturn += i;
    }
  }
  return toreturn;
}

vector<string> wiki::retruneight() { atomic<bool> atmarr[num_pgs]; }

void wiki::bfs_on_lvl(vector<int> start, vector<bool>visited) {
  
  

 }

int main() {
  vector<string> inputwords;
  // filter_out_noun("./wiki-nouns", "./onlynoun");
  //  int count = 0;
  string path = "./onlynoun";
  int count = 10;
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

  wiki mywiki = wiki(inputwords);
  mywiki.getgrid();

  // vector<string> out = mywiki.retruneight();
  // for (int i = 0; i < out.size(); i++) {
  //   cout << "@ i " << i << " = " << out[i] << endl;
  // }
  return 1;
}
