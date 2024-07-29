#include <time.h>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <map>

#include "../SkipList.h"

#define MAX_NUM 1000000

template<typename Container>
void insert_element(Container& container, std::string output_file) {
  clock_t start = clock();
  std::ofstream out(output_file);
  for (int i = 1; i <= MAX_NUM; i++) {
    int key = rand();
    int value = rand();
    container[key] = value;
    if (i % 100 == 0) {
      clock_t now = clock();
      out << i << " " << (now - start) / (double)CLOCKS_PER_SEC << "\n";
    }
  }
  out.close();
  clock_t now = clock();
  std::cout << "Insertion time: " << (now - start) / (double)CLOCKS_PER_SEC << "\n";

}

void validate() {
  SkipList<int, int> skipList;
  std::map<int, int> mp;
  for (int i = 0; i < 1000; i++) {
    int key = rand() % 1000;
    int value = rand();
    skipList.insert(key, value);
    mp[key] = value;
  }
  for (int i = 0; i < 1000; i++) {
    int key = rand() % 1000;
    if (rand() & 1) {
      int value = rand();
      skipList.insert(key, value);
      mp[key] = value;
    } else {
      skipList.erase(key);
      mp.erase(key);
    }
  }
  for (auto &p : mp) {
    if (skipList.find(p.first) == skipList.end()) {
      std::cout << "Validation failed\n";
      return;
    }
  }
  std::cout << "Validation passed\n";
}

int main() {
  srand(time(nullptr));

  std::cout << "validate test ..." << std::endl;
  validate();
  SkipList<int, int> skipList;

  std::cout << "skiplist insert_element test ...\n" << std::endl;
  insert_element(skipList, "insert_element_skiplist.txt");

  std::map<int, int> mp;

  std::cout << "map insert_element test ...\n" << std::endl;
  insert_element(mp, "insert_element_map.txt");
  return 0;
}