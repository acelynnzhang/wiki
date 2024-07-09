#include "wiki.h"

using namespace std;

void wiki::getgrid() {
  stringstream s;
  s << '[';
  for (auto row : grid) {
    s << '[';
    for (auto entry : row) {
      s << entry << ",";
    }
    s << "]\n";
  }
  s << ']';
  cout << s.str() << endl;
}

map<string, string> wiki::genmap(string path) {
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

void wiki::sortnoun(string pathh, string changeto) {
  string path = pathh;
  map<string, string> nounmap = genmap("nounlist.txt");

  std::filesystem::create_directories("./onlynoun");

  for (auto& entry : filesystem::directory_iterator(path)) {
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

      } catch (std::filesystem::filesystem_error& e) {
        std::cout << e.what() << '\n';
      }
    }
  }
}

string wiki::remove_punct(const string& str) {
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

string wiki::extract_word(string page_name) {
  string name;
  for (int m = 11; m < page_name.size() - 4; m++) {
    name.push_back(tolower(page_name.at(m)));
  }
  return name;
}

float wiki::sumvec(vector<float> v) {
  float toreturn;
  for (auto i : v) {
    if (i != static_cast<float>(INT32_MAX)) {
      toreturn += i;
    }
  }
  return toreturn;
}

vector<float> wiki::addvecf(vector<float> fir, vector<float> sec, vector<int> added) {
  vector<float> toreturn;
  int f = 0;
  int s = 0;
  for (int i = 0; i < max(fir.size(), sec.size()); i++) {
    if (i >= fir.size() || fir[i] == static_cast<float>(INT32_MAX)) {
      f = 0;
    } else {
      f = fir[i];
    }
    if (i >= sec.size() || sec[i] == static_cast<float>(INT32_MAX)) {
      s = 0;
    } else {
      s = sec[i];
    }
    toreturn.push_back(f + s);
  }
  return toreturn;
}

wiki::~wiki() {
  for(auto *pg : allpages) {
    delete pg;
  }
}