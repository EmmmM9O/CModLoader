#pragma once
#include <boost/function.hpp>
#include <boost/function/function_fwd.hpp>
#include <exception>
#include <map>
#include <queue>
#include <string>
#include <typeinfo>
namespace Struct {
class RunningTimeError : public std::exception {
public:
  std::string err;
  RunningTimeError(std::string e) { err = e; }
  const char *what() const throw() override { return err.data(); }
};
class BasicsType {
public:
  virtual const std::type_info &type() {
    const std::type_info &type = typeid(BasicsType);
    return type;
  }
  virtual std::string toString() { return "[BasicsType][BasicsType]"; }
};
} // namespace Struct