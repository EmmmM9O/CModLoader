#include "struct/Fi.hpp"
#include <iostream>
#include <string>
int main(int argc,char **args){
    if(argc<3){
        std::cout<<"No file";
        return 0;
    }
    Struct::ZipFi fi((std::string)args[1]);
    std::string o(args[2]);
    fi.unzip(o);
    return 0;
}
