#pragma once
#include "../struct/Basics.hpp"
#include "../struct/Fi.hpp"
#include "./script.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
namespace Core {
class ErrorModFile : public std::exception {
public:
  std::string info;
  ErrorModFile(std::string p) { info = "mod " + p + " Is invalid"; }
  const char *what() const throw() override { return info.c_str(); }
};
class modFile : public Struct::BasicsType {
public:
  const std::string standard = "cmod";
  std::string name, version, decs, author;
  std::vector<std::string> scripts;
  Struct::ZipFi file;
  std::list<std::string> list = {"mod.json", "build"};
  // std::list<std::string> list;
  modFile(std::string p) : file(p) { loadFile(Struct::Fi(p)); }
  void loadFile(Struct::Fi f) {
    // std::cout << "Test1" << std::endl;
    try {
      file = Struct::ZipFi(f);
    } catch (Struct::InvalidZip &e) {
      throw e;
    }
    // std::cout << "Test1" << std::endl;
    if (!f.extension().ends_with(standard)) {
      throw Struct::RunningTimeError("mod " + f.string() +
                                     " doesn't ends with " + standard +
                                     " may be isn't a mod");
    }
    auto tempFile = file.parent().child("tmp");
    if (!tempFile.exists())
      tempFile.mkdir();
    // std::cout << "Test2" << std::endl;
    if (!file.copy_to(tempFile)) {
      throw Struct::RunningTimeError("Can't copy file " + file.string() +
                                     " to " + tempFile.string());
    }
    auto tempModFile = Struct::ZipFi(tempFile.child(file.name()));
    tempModFile.change_extension(".zip");
    // std::cout << "Test4" << std::endl;
    auto modF = tempFile.child(f.path.stem().string());
    try {
      // std::cout << "path:" + tempModFile.string() << " to " << modF.string()
      ///          << std::endl;
      // exit(0);
      tempModFile.unzip(modF.string());
    } catch (...) {
      if (modF.exists())
        modF.remove();
      tempModFile.remove();
      throw ErrorModFile(f.string());
    }
    for (auto i : list) {
      auto t = modF.child(i);
      if (!t.exists()) {
        modF.remove();
        tempModFile.remove();
        throw Struct::RunningTimeError("mod without " + i);
      }
    }
    auto Target_file = modF.child("mod.json");
    // read mod json
    readInfo(Target_file.read_json(), modF);
    modF.remove();
    tempModFile.remove();
  }
  void readInfo(boost::property_tree::ptree tree, Struct::Fi file) {
    name = tree.get<std::string>("name");
    author = tree.get<std::string>("author");
    version = tree.get<std::string>("version");
    decs = tree.get<std::string>("decs");
    auto s_tree = tree.get_child("scripts");
    for (auto &script : s_tree) {
      auto str = script.second.data();
      if (Scripts.count(str) > 0) {
        scripts.push_back(str);
        Scripts[str]->load(file.child("build"), tree);
      } else {
        throw Struct::RunningTimeError("No such script " + str);
      }
    }
  }

  std::string toString() override final {
    std::string str = (std::string) "-----mod-----\n" + "name:" + name +
                      "  author:" + author + "\nversion:" + version +
                      "\ndecs:" + decs + "\nScripts:";
    for (auto s : scripts) {
      str += s + "\n";
    }
    return str;
  }
};
} // namespace Core