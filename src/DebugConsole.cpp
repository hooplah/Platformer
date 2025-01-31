#include "DebugConsole.h"

#include <iostream>

DebugConsole::DebugConsole(WorldRef& worldRef) :
    mWorldRef(worldRef)
{
    mCommands["test"] = [](std::vector<std::string>& param) -> void {std::cout << param[0] << std::endl;};
    mCommands["waypoint_edge"] = [this](std::vector<std::string>& param) -> void
    {
        int start = std::stof(param[0]);
        int end = std::stof(param[1]);
        int type = WayPointType::WALK;
        if (param[2] == "jump")
            type = WayPointType::JUMP;
        mWorldRef.getWayPointManager().addWayPointEdge(start, end, type);
    };
}

DebugConsole::~DebugConsole()
{
    //dtor
}
