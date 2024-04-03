#include "transport_catalogue.h"
#include <stdexcept>

#include <iostream>
#include <string>
namespace transport_directory{
    namespace tr_cat{       
        
        void TransportCatalogue::InsertStop(const std::string& stop_name, geo::Coordinates& coord){    
            stops_.push_back(std::move(domain::Stop {stop_name, coord}));
            stops_ptr[std::string_view(stops_.back().name)] = &stops_.back(); 
            
        }
        
       
        void TransportCatalogue::InsertStopDist(const std::string& stop_from,const std::string_view stop_to,const std::string_view stop_dist){
            auto stop_first_ptr = stops_ptr.at(stop_from);
            auto stop_second_ptr = stops_ptr.at(std::string(stop_to));
            int dist=std::stoi(std::string(stop_dist)); 
            distance_between_stop[{stop_first_ptr,stop_second_ptr}] = dist;            
        };
        
        
        void TransportCatalogue::InsertRoute(const std::string& bus, const std::vector<std::string_view>& stops_name){
            domain::Route rt;
            rt.name = bus;
            routes_.push_back(std::move(rt)); 
            for(const auto bus_stop:stops_name){         
                routes_.back().stops.push_back(stops_ptr[bus_stop]);
                buses_through_stop[std::string_view(stops_ptr.at(bus_stop) -> name)].insert(routes_.back().name);
            }

            routes_ptr[std::string_view(routes_.back().name)] = &routes_.back();
        };

        const domain::Stop* TransportCatalogue::FindStop(const std::string_view bus_stop){
            try {
                return stops_ptr.at(bus_stop);
            }
            catch ( std::out_of_range& ex){
                return nullptr;        
            }
        };

        const domain::Route* TransportCatalogue::FindRoute(const std::string_view bus){
                try {
                    return routes_ptr.at(bus);
                    }
                catch ( std::out_of_range& ex){
                    return nullptr;        
                }
         };
        double TransportCatalogue::GetRoadDist(const std::string_view stop_from, const std::string_view stop_to){
            double road_dist=.0;
            try{                        
                road_dist= distance_between_stop.at({stops_ptr.at(stop_from), stops_ptr.at(stop_to)});
                    }
            catch ( std::out_of_range& ex){                        
                road_dist= distance_between_stop.at({stops_ptr.at(stop_to), stops_ptr.at(stop_from)});
                    }
            return road_dist;
};


        RouteInf TransportCatalogue::GetRoutInform(const std::string_view bus){
                auto route_ptr = TransportCatalogue::FindRoute(bus);
                int stop_number = route_ptr -> stops.size();
                int unique_stop = 0; 
                double geo_lenght = .0;
                double road_lenght = .0;
                double curvature =.0;
                //Создаем вектор, состоящий из названий остановок маршрута для последующей сортировки и нахождения уникальных остановок 
                std::vector <std::string_view> route_stop;
                //заполняем его названиями остановок и сразу вычислим длину маршрута
                for (auto iter = std::begin((*route_ptr).stops); iter != std::end((*route_ptr).stops); iter++){            
                    route_stop.push_back((**iter).name);
                    //вычислим географич длину маршрута
                    if (iter <= (route_ptr -> stops).end()-2){
                        geo_lenght += ComputeDistance((*iter) -> position, (*(iter+1)) -> position);
                    }
                }
                //поиск действительной длины маршрута 
                for(int i = 0, j = 1; j < static_cast<int>(route_stop.size()); i++, j++){
                    road_lenght +=GetRoadDist(route_stop[i],route_stop[j]);
                }
                   curvature=road_lenght/geo_lenght ;
            
                //Сортируем вектор и удалением названия дублируемых остановок
                    sort_and_uniq(route_stop);
                    unique_stop = route_stop.size();
                return RouteInf{stop_number, unique_stop, geo_lenght, road_lenght, curvature};
            };

        const std::set<std::string_view> * TransportCatalogue::GetBusThroughStop(const std::string& bus_stop){  
                    try{
                        buses_through_stop.at(bus_stop);
                    }
                    catch ( std::out_of_range& ex){
                        return nullptr;        
                    }
                    return &(buses_through_stop.at(bus_stop));                    
        };
    }
}



