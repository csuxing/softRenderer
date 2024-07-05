#pragma once
#include <string>
namespace Jerry
{
    enum Type : uint8_t
    {
        //Relative paths
        Assets,
        Shaders,
        Storage,
        Screenshots,
        Logs,
        Graphs,
        /* NewFolder */
        TotalRelativePathTypes,

        //Special paths
        ExternalStorage,
        WorkingDir = ExternalStorage,
        Temp
    };
    const std::string get(const Type type, const std::string& file = "");

    std::vector<uint8_t> read_binary_file(const std::string& filename, const uint32_t count);

    std::string get_extension(const std::string& uri);
}