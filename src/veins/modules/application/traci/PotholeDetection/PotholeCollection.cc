
#include "veins/modules/application/traci/PotholeDetection/Pothole.h"
#include <veins/modules/application/traci/PotholeDetection/PotholeCollection.h>
#include <map>
#include <vector>

namespace veins {
    std::map<std::string,std::vector<Pothole>> PotholeCollection::getPotholesRoadMap()
    {
        std::map<std::string,std::vector<Pothole>> returnMap;

        for(Pothole p : potholes)
        {
            if(returnMap.count(p.roadId)>0)
            {
                returnMap[p.roadId].push_back(p);
            }
            else
            {
                std::vector<Pothole> v {p};
                returnMap[p.roadId] = v;
            }
        }

        return returnMap;
    }

};
