#pragma once
#include "geo.h"
#include <deque>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <utility>
namespace transport_directory{
    namespace tr_cat{ 
        
        struct RouteInf{
                int stop_number;
                int unique_stop_number;
                double route_geo_lenght;
                double route_road_lenght;
                double curvature;
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
            
            class MapHasher {
                    public:
                        size_t operator()(const std::pair<TransportCatalogue::Stop*,TransportCatalogue::Stop*> stop_pair) const {
                            size_t h1 = std::hash<const void*>{}(stop_pair.first);
                            size_t h2 = std::hash<const void*>{}(stop_pair.second);
                            return (h1*1e3 + h2*1e1+1);
                        }
                };
           
            public:
               
                void InsertStop(const std::string& stop_name, geo::Coordinates& coord);
                void InsertStopDist(const std::string& stop_name, std::unordered_map <std::string_view, std::string_view>& stop_dist);
                void InsertRout(const std::string& bus, std::vector<std::string_view> stops_name);
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
            std::unordered_map <std::string_view,std::deque<std::string_view>> buses_through_stop;
            std::unordered_map <std::pair<Stop*,Stop*>, int, MapHasher> distance_between_stop; 
            };
        
        
    }
}
    
