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
}