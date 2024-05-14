#include <unordered_map>
#include <assert.h>
#include "fileSystem.h"
#include <stdlib.h>
#include <direct.h>
namespace Jerry
{
    const std::unordered_map<Type, std::string> relative_paths = {
        {Type::Assets, "\\assets\\"},
        {Type::Shaders, "\\shaders\\"},
        {Type::Storage, "\\output\\"},
        {Type::Screenshots, "\\output\\images\\"},
        {Type::Logs, "\\output\\logs\\"},
        {Type::Graphs, "\\output\\graphs\\"}
    };
    const std::string get(const Type type, const std::string& file)
    {
        assert(relative_paths.size() == Type::TotalRelativePathTypes 
            && "Not all paths are defined in filesystem, please check that each enum is specified");

        auto it = relative_paths.find(type);

        if (type == Type::Assets || type == Type::Shaders)
        {
            std::string currentFilePath = __FILE__;
            auto pos0 = currentFilePath.rfind("\\");
            auto substr0 = currentFilePath.substr(0, pos0);
            auto pos1 = substr0.rfind("\\");
            auto substr1 = substr0.substr(0, pos1);
            return substr1 + it->second + file;
        }
        else
        {
            char currentPath[1024] = {};
            char dir[1024] = {};
            std::string str = _fullpath(currentPath, dir, 1024);
            return str + it->second + file;
        }
    }
}

