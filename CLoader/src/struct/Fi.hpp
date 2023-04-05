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
#include <exception>
#include <string>
extern "C" {
#include <zip.h>
}
namespace Struct {

class Fi {
protected:
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
  bool is_file() {
    try {
      if (!exists())
        return false;
      return boost::filesystem::is_regular_file(status());
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
class DecompressionError : public std::exception {
private:
  std::string path, info;

public:
  DecompressionError(std::string p) : path(p) {
    info = "Failed to open zip archive: " + p;
  }
  const char *what() const throw() override final { return info.data(); }
};
class InvalidZip : public std::exception {
private:
  std::string path, info;

public:
  InvalidZip(std::string p) : path(p) {
    info = "Zip File is invalid Path:" + path;
  }
  const char *what() const throw() override final { return info.data(); }
};
class InvalidFile : public std::exception {
private:
  std::string path, info;

public:
  InvalidFile(std::string p) : path(p) { info = "File is Invalid path:" + p; }
  const char *what() const throw() override final { return info.data(); }
};
class ZipFi : public Fi {
public:
  ZipFi(std::string filePath) : Fi(filePath) {
    if (!is_file()) {
      throw InvalidZip(filePath);
    }
  }
  Fi unzip(std::string &out_file) {
    int error;
    zip *archive = zip_open(path.string().c_str(), 0, &error);
    if (archive == nullptr) {
      throw InvalidZip(path.string());
    }
    int num_entries = zip_get_num_entries(archive, 0);
    for (int i = 0; i < num_entries; i++) {
      const char *name = zip_get_name(archive, i, 0);
      if (name == nullptr) {
        throw DecompressionError(zip_strerror(archive));
      }
      std::string dir = out_file + "/" + std::string(name);
      size_t pos = dir.rfind("/");
      if (pos != std::string::npos) {
        dir = dir.substr(0, pos);
      }
      if (!boost::filesystem::create_directory(dir) &&
          !boost::filesystem::exists(dir)) {
        throw InvalidFile(strerror(errno));
      }
      zip_file *file = zip_fopen_index(archive, i, 0);
      if (file == nullptr) {
        throw InvalidFile(zip_strerror(archive));
      }
      std::string output_path = out_file + "/" + std::string(name);
      FILE *output_file = fopen(output_path.c_str(), "wb");
      if (output_file == nullptr) {
        zip_fclose(file);
        throw InvalidFile(strerror(errno));
      }
      char buffer[BUFSIZ];
      int read_bytes;
      while ((read_bytes = zip_fread(file, buffer, BUFSIZ)) > 0) {
        fwrite(buffer, sizeof(char), read_bytes, output_file);
      }
      fclose(output_file);
      zip_fclose(file);
    }
    zip_close(archive);
    return Fi(out_file);
  }
};
} // namespace Struct
