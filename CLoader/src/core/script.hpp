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
  void load(Struct::Fi file, boost::property_tree::ptree tree) override {}
};
} // namespace __Scripts__

const std::map<std::string, Core::script *> Scripts = {
    {"c++", new (__Scripts__::CXX)}};