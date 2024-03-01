#pragma once
#include <string_view>
#include <functional>
#include <deque>
#include <vector>
#include <unordered_map>

#include <algorithm>
#include <string>
#include "geo.h"
namespace transport_directory{
    namespace tr_dir{
        class TransportCatalogue {   
            public:
                void InsertRout(std::string bus,std::vector<std::string_view> rout);
                void InsertStop(std::string bus_stop,geo::Coordinates lat_lng);
                const std::deque<std::string>* FindRout(std::string_view bus);            
                const geo::Coordinates* FindStop(std::string_view bus_stop);
                std::tuple<int,int,double> GetRoutInform(std::string_view bus);
                const std::deque<std::string_view>* GetBusThroughStop(std::string bus_stop);

            private:   
            std::unordered_map<std::string,geo::Coordinates> stop_base;
            std::unordered_map<std::string,std::deque<std::string>> route_base;
            std::unordered_map<std::string,std::deque<std::string_view>> buses_through_stop;
        };
    }
}