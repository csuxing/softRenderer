#include <unordered_map>
#include <assert.h>
#include <stdlib.h>
#include <direct.h>
#include <fstream>

#include "fileSystem.h"
namespace Jerry
{
    const std::unordered_map<Type, std::string> relative_paths = {
        {Type::Assets, "\\assets\\"},
        {Type::Shaders, "\\shaders\\spv\\"},
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

    std::string get_extension(const std::string& uri)
    {
        auto dot_pos = uri.find_last_of('.');
        if (dot_pos == std::string::npos)
        {
            throw std::runtime_error{ "Uri has no extension" };
        }

        return uri.substr(dot_pos + 1);
    }

    std::vector<uint8_t> read_binary_file(const std::string& filename, const uint32_t count)
    {
        std::vector<uint8_t> data;

        std::ifstream file;

        file.open(filename, std::ios::in | std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file: " + filename);
        }

        uint64_t read_count = count;
        if (count == 0)
        {
            file.seekg(0, std::ios::end);
            read_count = static_cast<uint64_t>(file.tellg());
            file.seekg(0, std::ios::beg);
        }

        data.resize(static_cast<size_t>(read_count));
        file.read(reinterpret_cast<char*>(data.data()), read_count);
        file.close();

        return data;
    }

}

