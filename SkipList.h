#include <time.h>

#include <algorithm>
template <typename K, typename V, typename Comp = std::less<K>>
class SkipList {
 private:
  struct Node {
    int level;
    const K key;
    V value;
    Node **forward;
    Node() : level(0), key(K()), value(V()), forward(nullptr) {}
    Node(const K &k, const V &v, int l, Node *nxt = nullptr)
        : key(k), value(v), level(l) {
      forward = new Node *[level + 1];
      std::fill(forward, forward + level + 1, nxt);
    }
    ~Node() { delete[] forward; }
  };
  void init() {
    srand(time(nullptr));
    head = new Node();
    tail = new Node();
    head->forward = new Node *[MAX_LEVEL + 1];
    tail->forward = new Node *[MAX_LEVEL + 1];
    std::fill(head->forward, head->forward + MAX_LEVEL + 1, tail);
    std::fill(tail->forward, tail->forward + MAX_LEVEL + 1, nullptr);
    level = 0;
    length = 0;
  }

  int randomLevel() {
    int lvl = 1;
    while ((rand() & S) < PS) {
      lvl++;
    }
    return lvl < MAX_LEVEL ? lvl : MAX_LEVEL;
  }

  int level;
  int length;
  static const int MAX_LEVEL = 32;
  static const int P = 4;
  static const int S = 0xFFFF;
  static const int PS = S / P;
  Node *head;
  Node *tail;
  Comp comp;

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

  void insert(const K &key, const V &value) {
    Node *update[MAX_LEVEL + 1];
    Node *cur = find(key, update);
    if (cur != tail && cur->key == key) {
      cur->value = value;
      return;
    }
    int lvl = randomLevel();
    if (lvl > level) {
      lvl = ++level;
      update[lvl] = head;
    }
    Node *newNode = new Node(key, value, lvl);
    for (int i = lvl; i >= 0; i--) {
      newNode->forward[i] = update[i]->forward[i];
      update[i]->forward[i] = newNode;
    }
    length++;
  }

  bool erase(const K &key) {
    Node *update[MAX_LEVEL + 1];
    Node *cur = find(key, update);
    if (cur == tail || cur->key != key) {
      return false;
    }
    for (int i = 0; i <= cur->level; i++) {
      update[i]->forward[i] = cur->forward[i];
    }
    delete cur;
    while (level > 0 && head->forward[level] == tail) {
      level--;
    }
    length--;
    return true;
  }

  Iterator find(const K &key) {
    Node *cur = find(key, nullptr);
    if (cur == tail || cur->key != key) {
      return Iterator(tail);
    }
    return Iterator(cur);
  }

  bool count(const K &key) { return find(key) != tail; }

  V &operator[](const K &key) {
    Node *cur = find(key, nullptr);
    if (cur == tail || cur->key != key) {
      insert(key, V());
      cur = find(key, nullptr);
    }
    return cur->value;
  }

  Iterator begin() { return Iterator(head->forward[0]); }

  Iterator end() { return Iterator(tail); }
};