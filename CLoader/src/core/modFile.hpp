#pragma once
#include "../struct/Basics.hpp"
#include "../struct/Fi.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <exception>
#include <string>
namespace Core {
class ErrorModFile : public std::exception {
public:
  std::string info;
  ErrorModFile(std::string p) { info = "mod " + p + " Is invalid"; }
  const char *what() const throw() override { return info.c_str(); }
};
class modFile {
public:
  const std::string standard = "cmod";
  std::string name, version, decs, author;
  Struct::ZipFi file;
  std::list<Struct::forwardKey<std::string>> Belt = {
      Struct::forwardKey<std::string>("mod.json", true)};
  void loadFile(Struct::Fi f) {
    try {
      file = Struct::ZipFi(f);
    } catch (Struct::InvalidZip &e) {
      throw e;
    }
    if (!f.extension().ends_with(standard)) {
      throw Struct::RunningTimeError("mod " + f.string() +
                                     " doesn't ends with " + standard +
                                     " may be isn't a mod");
    }
    auto tempFile = file.parent().child("tmp");
    if (!tempFile.exists())
      tempFile.mkdir();
    if (!file.copy_to(tempFile)) {
      throw Struct::RunningTimeError("Can't copy file " + file.string() +
                                     " to " + tempFile.string());
    }
    auto modF = tempFile.child(f.name());
    try {
      file.unzip(modF.string());
    } catch (...) {
      if (modF.exists())
        modF.remove();
      throw ErrorModFile(f.string());
    }
    Struct::forwardList<std::string, Struct::Fi> list;
    list.getData = [&modF](std::string key) -> Struct::Fi {
      return modF.child(key);
    };
    list.checker = [](Struct::Fi file) -> bool { return file.exists(); };
    list.Error = [&f, &modF](std::string file) -> void {
      modF.remove();
      throw Struct::RunningTimeError("cannot find " + file + " in " +
                                     f.string());
    };
    for (auto i : Belt) {
      list.Belt.push(i);
    }
    auto m = list.run();
    auto Target_file = m["mod.json"];
    // read mod json
    readInfo(Target_file.read_json());
  }
  void readInfo(boost::property_tree::ptree tree) {}
};
} // namespace Core