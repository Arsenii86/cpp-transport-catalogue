#include "transport_catalogue.h"
#include <stdexcept>

#include <iostream>
#include <string>
namespace transport_directory{
    namespace tr_cat{
        
        void TransportCatalogue::InsertStop(const std::string& stop_name, geo::Coordinates coord){    
            stops_.push_back(std::move(Stop {stop_name, coord}));
            stops_ptr[std::string_view(stops_.back().name)] = &stops_.back();   
        }

        void TransportCatalogue::InsertRout(const std::string& bus, std::vector<std::string_view> stops_name){
            Route rt;
            rt.name = bus;
            routes_.push_back(std::move(rt)); 
            for(const auto bus_stop:stops_name){         
                routes_.back().stops.push_back(stops_ptr[bus_stop]);
                buses_through_stop[std::string_view(stops_ptr.at(bus_stop) -> name)].push_back(std::string_view(routes_.back().name));
            }

            routes_ptr[std::string_view(routes_.back().name)] = &routes_.back();
        };

        const TransportCatalogue::Stop* TransportCatalogue::FindStop(std::string_view bus_stop){
            try {
                return stops_ptr.at(bus_stop);
            }
            catch ( std::out_of_range& ex){
                return nullptr;        
            }
        }

        const TransportCatalogue::Route* TransportCatalogue::FindRout(std::string_view bus){
                try {
                    return routes_ptr.at(bus);
                    }
                catch ( std::out_of_range& ex){
                    return nullptr;        
                }
         }



        RouteInf TransportCatalogue::GetRoutInform(std::string_view bus){
                auto route_ptr = TransportCatalogue::FindRout(bus);
                int stop_number = route_ptr -> stops.size();
                int unique_stop = 0; 
                double lenght = .0;
                //Создаем вектор, состоящий из названий остановок маршрута для последующей сортировки и нахождения уникальных остановок 
                std::vector <std::string_view> route_stop;
                //заполняем его названиями остановок и сразу вычислим длину маршрута
                for (auto iter = std::begin((*route_ptr).stops); iter != std::end((*route_ptr).stops); iter++){            
                    route_stop.push_back((**iter).name);
                    //вычислим длину маршрута
                    if (iter <= (route_ptr -> stops).end()-2){
                        lenght += ComputeDistance((*iter) -> position, (*(iter+1)) -> position);
                    }
                }
                //Сортируем вектор и удалением названия дублируемых остановок
                    sort_and_uniq(route_stop);
                    unique_stop = route_stop.size();
                return RouteInf{stop_number, unique_stop, lenght};
            }

        const std::deque<std::string_view>* TransportCatalogue::GetBusThroughStop(const std::string& bus_stop){  
                    try{
                        buses_through_stop.at(bus_stop);
                    }
                    catch ( std::out_of_range& ex){
                        return nullptr;        
                    }
                    auto &buses = buses_through_stop.at(bus_stop);
                    sort_and_uniq(buses);            
                    return &buses;
        }
    }
}



