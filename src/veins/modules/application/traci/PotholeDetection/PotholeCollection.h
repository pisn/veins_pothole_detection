#pragma once

#include "veins/modules/application/traci/PotholeDetection/Pothole.h"
#include <map>
#include <vector>

namespace veins {

    class PotholeCollection{
    public:
        std::vector<Pothole> potholes;

        std::map<std::string,std::vector<Pothole>> getPotholesRoadMap();
    };

}
