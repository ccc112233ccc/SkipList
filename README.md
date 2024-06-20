# SkipList


## description

一个跳表的实现，使用C++泛型编程实现，支持Key和Value的类型自定义，支持自定义比较函数。
支持的操作有：
- 插入
- 删除
- 查找
- 遍历

## usage

```cpp
#include "SkipList.h"

int main() {
    SkipList<int, int> skipList;
    skipList.insert(1, 1);
    skipList.insert(2, 2);
    skipList.insert(3, 3);
    skipList.insert(4, 4);

    skipList.erase(2);

    auto it = skipList.find(3);

    cout << it->key << " " << it->value << endl;

    for (auto it = skipList.begin(); it != skipList.end(); ++it) {
        std::cout << it->key << " " << it->value << std::endl;
    }

    return 0;
}    

