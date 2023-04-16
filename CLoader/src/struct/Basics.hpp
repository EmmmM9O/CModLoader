#pragma once
#include <boost/function.hpp>
#include <boost/function/function_fwd.hpp>
#include <exception>
#include <map>
#include <queue>
#include <string>
namespace Struct {
class RunningTimeError : public std::exception {
public:
  std::string err;
  RunningTimeError(std::string e) { err = e; }
  const char *what() const throw() override { return err.data(); }
};
template <typename Key> class forwardKey {
public:
  Key key;
  bool must;
  forwardKey(Key k, bool m) {
    key = k;
    must = m;
  }
};
template <typename Key, typename Data> class forwardList {
public:
  std::map<Key, Data> run() {
    std::map<Key, Data> list;
    while (!Belt.empty()) {
      auto key = Belt.front();
      auto data = getData(key.key);
      if (checker(data) || !key.must) {
        list[key.key] = data;
      } else {
        Error(key.key);
      }
      Belt.pop();
    }
  }
  std::queue<forwardKey<Key>> Belt;
  boost::function<Data(Key)> getData;
  boost::function<void(Key)> Error;
  boost::function<bool(Data)> checker;
};
} // namespace Struct