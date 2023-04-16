#include "core/modFile.hpp"
#include <iostream>
#include <string>
int main(int argc, char **args) {
  if (argc < 2) {
    std::cout << "No file";
    return 0;
  }
  std::cout<<"\nTest!";
  Core::modFile mod((std::string(args[1])));
  std::cout << mod.toString();
  return 0;
}
