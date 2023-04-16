#pragma once
#include "../struct/Fi.hpp"
#include <boost/next_prior.hpp>
#include <map>
#include <string>
namespace Core {
class script {
public:
  std::string name;
  virtual void load(Struct::Fi file, boost::property_tree::ptree tree) = 0;
};
} // namespace Core
namespace __Scripts__ {
class CXX : public Core::script {
public:
  CXX() { name = "c++"; }
};
} // namespace __Scripts__

extern std::map<std::string, Core::script *> Scripts;