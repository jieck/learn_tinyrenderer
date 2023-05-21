#include "model.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "geometry.h"
#include <stdio.h>

Model::Model(const std::string &path) {
    std::ifstream ifs;
    ifs.open(path, std::ifstream::in);
    if (ifs.fail()) {
        std::cout<< "open file fail"<< std::endl;
        return;
    }
    std::string line;
    char trash;
    while(!ifs.eof()) {
        std::getline(ifs, line);
//        std::cout<< line << std::endl;
        std::istringstream iss(line);
        if (line.substr(0, 2) == "v ") {
            iss >> trash;
            float x ;
            float y ;
            float z ;
            iss >> x;
            iss >> y;
            iss >> z;
            this->verts_.push_back(*new Vec3f(x, y, z));
        }else if (line.substr(0, 2) == "f ") {
            std::vector<int> veci;
            int itrash, idx;
            iss >> trash;
            while(iss >> idx >> trash >> itrash >> trash >> itrash) {
                veci.push_back(--idx);
//                std::cout<< idx<< "id  " << trash << "pp" << itrash << "ll" << line<< std::endl;
            }
//            std::cout<< "---\n"<< std::endl;
            this->faces_.push_back(veci);
        }
    }
}

int Model::nverts() {
    return this->verts_.size();
}

int Model::nfaces() {
    return this->faces_.size();
}

std::vector<int> Model::face(int idx) {
    return this->faces_[idx];
}

Vec3f Model::vert(int idx) {
    return this->verts_[idx];
}
