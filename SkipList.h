#include <time.h>
#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>

// Class SkipLists
// The default comparator is std::less<K>
template <typename K, typename V, typename Comp = std::less<K>>
class SkipList {

 private:

  struct Node {

    int level; // level of this node
    const K key;
    V value;
    Node **forward; // next node at each level

    Node() : level(0), key(K()), value(V()), forward(nullptr) {}
    Node(const K &k, const V &v, int l, Node *nxt = nullptr)
        : key(k), value(v), level(l) {

      // allocate memory
      forward = new Node *[level + 1];

      // initialize
      std::fill(forward, forward + level + 1, nxt);
    }
    ~Node() { delete[] forward; }
  };
  void init() {

    // initialize
    srand(time(nullptr));

    // initialize head and tail
    head = new Node();
    tail = new Node();
    head->forward = new Node *[MAX_LEVEL + 1];
    tail->forward = new Node *[MAX_LEVEL + 1];

    // initialize head and tail
    std::fill(head->forward, head->forward + MAX_LEVEL + 1, tail);
    std::fill(tail->forward, tail->forward + MAX_LEVEL + 1, nullptr);

    // initialize level
    level = 0;
    length = 0;
  }

  // generate random level, 1 <= lvl <= MAX_LEVEL
  // 1/P chance to increase level
  int randomLevel() {
    int lvl = 1;
    while ((rand() & S) < PS) {
      lvl++;
    }
    return lvl < MAX_LEVEL ? lvl : MAX_LEVEL;
  }

  int level; // current level
  int length; // length of the list
  static const int MAX_LEVEL = 32; // max level
  static const int P = 4; // probability, 1/P chance to increase level
  static const int S = 0xFFFF; // max value of random number
  static const int PS = S / P; 


  Node *head; // head of the list
  Node *tail; // tail of the list
  Comp comp; // comparator

  Node *find(const K &key, Node **update) {
    Node *cur = head;
    for (int i = level; i >= 0; i--) {
      while (cur->forward[i] != tail && comp(cur->forward[i]->key, key)) {
        cur = cur->forward[i];
      }
      if (update != nullptr) {
        update[i] = cur;
      }
    }
    cur = cur->forward[0];
    return cur;
  }

 public:

  struct Iterator {
    Node *node;
    Iterator() : node(nullptr) {}
    Iterator(Node *n) : node(n) {}
    Iterator &operator++() {
      node = node->forward[0];
      return *this;
    }
    Iterator operator++(int) {
      Iterator tmp = *this;
      node = node->forward[0];
      return tmp;
    }
    bool operator==(const Iterator &it) const { return node == it.node; }
    bool operator!=(const Iterator &it) const { return node != it.node; }
    Node *operator->() const { return node; }
    Node &operator*() const { return *node; }
  };

  SkipList() : comp{Comp()} { init(); }

  SkipList(Comp c) : comp{c} { init(); }

  // insert element into skip list
  Node* insert(const K &key, const V &value) {

    /* 
                              +------------+
                              |  insert 50 |
                              +------------+
    level 4     +-->1+                                                      100
                    |
                    |                      insert +----+
    level 3         1+-------->10+---------------> | 50 |          70       100
                                                  |    |
                                                  |    |
    level 2         1          10         30       | 50 |          70       100
                                                  |    |
                                                  |    |
    level 1         1    4     10         30       | 50 |          70       100
                                                  |    |
                                                  |    |
    level 0         1    4   9 10         30   40  | 50 |  60      70       100
                                                  +----+

    */
    // update pointers to next nodes
    Node *update[MAX_LEVEL + 1];

    // find the position to insert
    Node *cur = find(key, update);

    // if key already exists, update value
    if (cur != tail && cur->key == key) {
      cur->value = value;
      return nullptr;
    }

    // generate random level
    int lvl = randomLevel();

    // if new level is higher than current level, update head
    if (lvl > level) {
      lvl = ++level;
      update[lvl] = head;
    }

    // insert new node
    Node *newNode = new Node(key, value, lvl);

    // link new node to next nodes
    for (int i = lvl; i >= 0; i--) {
      newNode->forward[i] = update[i]->forward[i];
      update[i]->forward[i] = newNode;
    }

    // increase length
    length++;

    return newNode;
  }

  // erase element from skip list
  bool erase(const K &key) {
    
    // find the position 
    Node *update[MAX_LEVEL + 1];
    Node *cur = find(key, update);

    // if key not found, return false
    if (cur == tail || cur->key != key) {
      return false;
    }

    // unlink node from next nodes
    for (int i = 0; i <= cur->level; i++) {
      update[i]->forward[i] = cur->forward[i];
    }

    // delete node
    delete cur;

    // decrease level if necessary
    while (level > 0 && head->forward[level] == tail) {
      level--;
    }

    // decrease length
    length--;
    return true;
  }

  // Search for element in skip list 
  Iterator find(const K &key) {
    /*
                              +------------+
                              |  select 60 |
                              +------------+
    level 4     +-->1+                                                      100
                    |
                    |
    level 3         1+-------->10+------------------>50+           70       100
                                                      |
                                                      |
    level 2         1          10         30         50|           70       100
                                                      |
                                                      |
    level 1         1    4     10         30         50|           70       100
                                                      |
                                                      |
    level 0         1    4   9 10         30   40    50+-->60      70       100
    */

    // find the position
    Node *cur = find(key, nullptr);

    // if key not found, return end()
    if (cur == tail || cur->key != key) {
      return Iterator(tail);
    }

    // return iterator
    return Iterator(cur);
  }

  // count element in skip list
  bool count(const K &key) { return find(key) != tail; }

  // get size of skip list
  int size() { return length; }

  // get value of element in skip list
  V &operator[](const K &key) {

    Node *cur = find(key, nullptr);

    // if key not found, insert it
    if (cur == tail || cur->key != key) {
      cur = insert(key, V());
    }
    return cur->value;
  }

  // iterator
  Iterator begin() { return Iterator(head->forward[0]); }

  Iterator end() { return Iterator(tail); }

  // dump skip list to file
  void dump(const std::string &filename) {

    std::ofstream out(filename);
    Node *cur = head->forward[0];
    while (cur != tail) {
      out << cur->key << ":" << cur->value << std::endl;
      cur = cur->forward[0];
    }
    out.close();
  }

  // load skip list from file
  void load(const std::string &filename) {
    std::ifstream in(filename);
    if (in) {
      std::string line;

      // line : key : value
      while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string key;
        std::string value;
        std::getline(ss, key, ':');
        std::getline(ss, value);

        // insert key and value, convert string to int
        insert(std::stoi(key), std::stoi(value));
      }
    }
  }
  // clear skip list
  void clear() {
    Node *cur = head->forward[0];
    while (cur != tail) {
      Node *next = cur->forward[0];
      delete cur;
      cur = next;
    }
    delete head;
    delete tail;
    head = nullptr;
    tail = nullptr;
    level = 0;
    length = 0;
  }
};