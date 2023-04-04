#pragma once
#include <boost/filesystem.hpp>
#include <boost/filesystem/directory.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/function.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <string>
namespace Struct {

class Fi {
private:
  boost::filesystem::path path;

public:
  Fi(boost::filesystem::path file) : path(file) {}
  Fi(std::string filePath) : path(filePath) {}
  boost::filesystem::file_status status() {
    try {
      return boost::filesystem::status(path);
    } catch (boost::filesystem::filesystem_error &e) {
      throw e;
    }
  }
  bool exists() {
    try {
      return boost::filesystem::exists(status());
    } catch (boost::filesystem::filesystem_error &e) {
      return false;
    }
  }
  bool mkdir() {
    try {
      return boost::filesystem::create_directory(path);
    } catch (boost::filesystem::filesystem_error &e) {
      return false;
    }
  }
  bool rename(std::string name) {
    try {
      if (!exists())
        return false;
      boost::filesystem::rename(path, name);
      return true;
    } catch (boost::filesystem::filesystem_error &e) {
      return false;
    }
  }
  bool remove() {
    try {
      if (!exists())
        return false;
      return boost::filesystem::remove(path);
    } catch (boost::filesystem::filesystem_error &e) {
      return false;
    }
  }
  boost::filesystem::ofstream writer() {
    return boost::filesystem::ofstream(path);
  }
  boost::filesystem::ifstream reader() {
    return boost::filesystem::ifstream(path);
  }
  auto size() { return boost::filesystem::file_size(path); }
  std::string readALL() {
    auto file_size = size();
    std::string file_content;
    file_content.resize(file_size);
    boost::filesystem::ifstream(path, std::ios_base::in | std::ios_base::binary)
        .read(&file_content[0], file_size);
    return file_content;
  }
  bool is_directory() {
    try {
      if (!exists())
        return false;
      return boost::filesystem::is_directory(status());
    } catch (boost::filesystem::filesystem_error &e) {
      return false;
    }
  }
  void
  forEach(boost::function<void(boost::filesystem::directory_entry &)> func) {
    for (auto &entry : boost::filesystem::directory_iterator(path)) {
      func(entry);
    }
  }
  void
  forEachDeep(boost::function<void(boost::filesystem::directory_entry &)> func,
              boost::filesystem::path path) {
    for (auto &entry : boost::filesystem::recursive_directory_iterator(path)) {
      if (boost::filesystem::is_regular_file(entry.path())) {
        func(entry);
      }
      if (boost::filesystem::is_directory(entry.path())) {
        forEachDeep(func, entry.path());
      }
    }
  }
};
} // namespace Struct
