cmake_minimum_required(VERSION 3.1)
project(ThirdParty)
add_subdirectory(spdlog)
set_property(TARGET spdlog PROPERTY FOLDER "ThirdParty")