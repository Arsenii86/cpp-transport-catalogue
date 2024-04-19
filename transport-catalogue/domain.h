#pragma once
#include "geo.h"
#include <string>
#include <vector>
namespace domain{
    struct Stop{
              std::string name;
              geo::Coordinates position;
            };
     struct Route{
              std::string name;  
              std::vector<Stop*> stops;  
            };

}