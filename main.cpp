#include <iostream>
#include "framework/jerry_engine.h"

int main()
{
    std::shared_ptr<Jerry::JerryEngine> engine = std::make_shared<Jerry::JerryEngine>();
    engine->startEngine("");
    engine->run();
    engine->shutdownEngine();
    return 0;
}