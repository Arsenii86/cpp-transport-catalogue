#pragma once
#include "geo.h"
#include <deque>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <tuple>
namespace transport_directory{
    namespace tr_cat{
        
        struct RouteInf{
                int stop_number;
                int unique_stop_number;
                double lenght_route;
            };
        
        class TransportCatalogue {

            struct Stop{
              std::string name;
              geo::Coordinates position;
            };
            struct Route{
              std::string name;  
              std::vector<Stop*> stops;  
            };   
           
            public:
                void InsertStop(const std::string& stop_name, geo::Coordinates coord);
                void InsertRout(const std::string& bus, std::vector<std::string_view>stops_name);
                const Route* FindRout(std::string_view bus);
                const Stop* FindStop(std::string_view bus_stop);
                RouteInf GetRoutInform(std::string_view bus);
                const std::deque<std::string_view>* GetBusThroughStop(const std::string& bus_stop);


            private:
            template <typename T>
            void sort_and_uniq(T &name){
                    std::sort(name.begin(), name.end());
                    const auto ret = std::unique(name.begin(), name.end());
                    name.erase(ret, name.end());
            }

            std::deque<Stop> stops_;
            std::unordered_map <std::string_view,Stop*> stops_ptr;
            std::deque<Route> routes_;
            std::unordered_map <std::string_view,Route*> routes_ptr;
            std::unordered_map<std::string_view,std::deque<std::string_view>> buses_through_stop;
            };
    }
}
    
