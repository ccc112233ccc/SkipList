#include <time.h>

#include <algorithm>
#include <iostream>
#include <map>

#include "SkipList.h"

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

void benchmark() {
  int n = 1e6;
  SkipList<int, int> skipList;
  clock_t start = clock();
  for (int i = 0; i < n; i++) {
    int key = rand();
    int value = rand();
    skipList.insert(key, value);
  }
  for (int i = 0; i < n; i++) {
    int key = rand();
    if (i & 1) {
      skipList.erase(key);
    } else {
      skipList.find(key);
    }
  }
  std::cout << "SkipList: " << (double)(clock() - start) / CLOCKS_PER_SEC
            << "s\n";

  std::map<int, int> mp;
  start = clock();
  for (int i = 0; i < n; i++) {
    int key = rand();
    int value = rand();
    mp[key] = value;
  }
  for (int i = 0; i < n; i++) {
    int key = rand();
    if (i & 1) {
      mp.erase(key);
    } else {
      mp.find(key);
    }
  }
  std::cout << "std::map: " << (double)(clock() - start) / CLOCKS_PER_SEC
            << "s\n";
}

int main() {
  srand(time(nullptr));
  validate();
  benchmark();
  return 0;
}