#pragma once
#include "geo.h"
#include <deque>
#include <vector>
#include <string>
#include <unordered_map>
#include <set>
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
                void InsertStopDist(const std::string& stop_from,const std::string_view stop_to,const std::string_view stop_dist);
                double GetRoadDist(const std::string_view stop_from, const std::string_view stop_to);
                void InsertRoute(const std::string& bus,const std::vector<std::string_view>& stops_name);
                const Route* FindRoute(const std::string_view bus);
                const Stop* FindStop(const std::string_view bus_stop);
                RouteInf GetRoutInform(const std::string_view bus);
                const std::set<std::string_view>* GetBusThroughStop(const std::string& bus_stop); 

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
            std::unordered_map <std::string_view,std::set<std::string_view>> buses_through_stop;
            std::unordered_map <std::pair<Stop*,Stop*>, int, MapHasher> distance_between_stop; 
            };
        
        
    }
}
    
