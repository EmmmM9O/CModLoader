#pragma once
#include <utility>
extern "C" {
#include <dlfcn.h>
}
#include "mod.hpp"
#include <exception>
#include <string>
namespace Core {
class ErrorLib : public std::exception {
public:
  std::string info;
  ErrorLib(std::string p) { info = "Error Library " + p; }
  const char *what() const throw() override { return info.data(); }
};

class ErrorType : public std::exception {
public:
  std::string info;
  ErrorType(std::string i) { info = "Error loading symbol:" + i; }
  const char *what() const throw() override { return info.data(); }
};
class CLoader {
public:
  template <typename T>
  static std::pair<T *, void *> LoadAsClass(std::string path) {
    void *dl_handle = dlopen(path.c_str(), RTLD_NOW);
    if (dl_handle == nullptr) {
      throw ErrorLib(path);
    }
    using CtorType = T *(*)();
    CtorType ctor = (CtorType)dlsym(dl_handle, "create");
    if (ctor == nullptr) {
      throw ErrorType(dlerror());
    }
    T *res = ctor();
    return std::make_pair(res, dl_handle);
  }
  static void Load(std::string path) {
    auto p = LoadAsClass<Mod>(path);
    p.first->init();
    delete p.first;
    dlclose(p.second);
  }
};
} // namespace Core