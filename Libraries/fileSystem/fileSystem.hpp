//
//  fileSystem.hpp
//  Terralistic
//
//  Created by Jakob Zorz on 05/12/2020.
//

#ifndef fileSystem_hpp
#define fileSystem_hpp

#include <string>

namespace fileSystem {

bool dirExists(const std::string& path);
bool fileExists(const std::string& path);
void createDirIfNotExists(const std::string& path);
void removeDir(const std::string& path);
void removeFile(const std::string& path);

}

#endif /* fileSystem_hpp */
