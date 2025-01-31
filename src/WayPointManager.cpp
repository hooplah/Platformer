#include "WayPointManager.h"

#include "Line.h"

#include <iostream>

WayPointManager::WayPointManager()
{
    mPather = new micropather::MicroPather(this);
}

WayPointManager::~WayPointManager()
{
    delete mPather;
}

void WayPointManager::draw(sf::RenderTarget& target)
{
}

void WayPointManager::addWayPointEdge(int startIndex, int endIndex, int type)
{
    auto a = WayPointEdge(endIndex, type);
    auto b = WayPointEdge(startIndex, type);
    mWayPoints[startIndex].mEdges.push_back(a);
    mWayPoints[endIndex].mEdges.push_back(b);
    mWayPointEdges.push_back(std::make_tuple(a, b));
}

float WayPointManager::LeastCostEstimate(void* stateStart, void* stateEnd)
{
    WayPoint* start = (WayPoint*)stateStart;
    WayPoint* end = (WayPoint*)stateEnd;
    float dist = 0.f;
    if (start && end)
        dist = sqrt(pow(end->mPosition.x - start->mPosition.x, 2) + pow(end->mPosition.y - start->mPosition.y, 2));
    return dist;
}

void WayPointManager::AdjacentCost(void* state, std::vector<micropather::StateCost> *adjacent)
{
    WayPoint* point = (WayPoint*)state;

    for (auto& edge : point->mEdges)
    {
        WayPoint* target = &mWayPoints[edge.mTargetIndex];

        micropather::StateCost nodeCost;
        nodeCost.state = (void*)target;
        nodeCost.cost = sqrt(pow(target->mPosition.x - point->mPosition.x, 2) + pow(target->mPosition.y - point->mPosition.y, 2));
        adjacent->push_back(nodeCost);
    }
}

void WayPointManager::getPath(std::vector<WayPoint*>& path, int index, int targetIndex)
{
    path.clear();
    std::vector<void*>* path_witchery = (std::vector<void*>*)(&path);
    float totalCost = 0;
    /*int result =*/ mPather->Solve((void*)&mWayPoints[index], (void*)&mWayPoints[targetIndex], path_witchery, &totalCost);
}
