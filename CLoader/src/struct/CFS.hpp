#include <map>
#include <vector>
namespace Struct {
template <typename T> class CFSNode {
public:
  T data;
  int next, to;
};
template <typename T> class CFSTree {
public:
  std::vector<CFSNode<T>> lists;
  std::map<int, int> heads;
  void add(int from, int to, T data) {
    CFSNode<T> node;
    node.to = to;
    node.data = data;
    auto a = heads.find(from);
    if (a == heads.end()) {
      node.next = 1;
    } else {
      node.next = a->second;
    }
    lists.push_back(node);
    a->second = lists.size() - 1;
  }
};
} // namespace Struct