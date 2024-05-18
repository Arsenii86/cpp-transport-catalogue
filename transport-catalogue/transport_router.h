#pragma once
#include "transport_catalogue.h"
#include "graph.h"
#include "router.h"
#include <unordered_map>
#include <map>
#include <memory>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <iterator>
#include "json_builder.h"
#include <iostream>


namespace transport_router{
using namespace std::literals;
    struct RouteSetings{
        double bus_velocity=0;
        int bus_wait_time =0;
    };

    struct RouteTime{    
       std::string bus_or_wait_;
       double time_between_stop_ = 0;
       int span_count_ = 0;
        
       constexpr RouteTime() = default;
       RouteTime(std::string bus_or_wait,double time_between_stop,int span_count): bus_or_wait_(bus_or_wait),
                                                                       time_between_stop_(time_between_stop),
                                                                       span_count_(span_count){};

        bool operator<(const RouteTime& rdt ) const{
            return (rdt.time_between_stop_ - time_between_stop_) > 1e-3;
        }
        bool operator>(const RouteTime& rdt ) const{
            return (time_between_stop_ - rdt.time_between_stop_) > 1e-3;
        }
        RouteTime operator+(const RouteTime& rdt )const {
            return RouteTime(bus_or_wait_, time_between_stop_ + rdt.time_between_stop_, span_count_);
        }
        ~RouteTime() = default;
    };
    
   
    struct RouteInfoTranslete{
        double time = 0.0;
        int wait_time =0;
        std::vector<const graph::Edge<transport_router::RouteTime>*> route_edges_inform;
        const std::unordered_map<size_t,std::string>& vertex_num_stop; 
            
        RouteInfoTranslete() = delete;
        
        RouteInfoTranslete(double all_route_time,
                           int stop_time, 
                           const std::vector<const graph::Edge<transport_router::RouteTime>*>& route_edges,
                           const std::unordered_map<size_t,std::string>& vertex_stop):time(all_route_time),
                                                                                    wait_time(stop_time), 
                                                                                    route_edges_inform(std::move(route_edges)),
                                                                                    vertex_num_stop(vertex_stop){};
    };
    
    void JsonBuildForRoute(json::Builder& answer,
                           const RouteInfoTranslete& optimal_route,
                           int request_id);
    
    class TransportRouter{
      
    //Итерпритатор вершин,представляющий из себя словарь с парой "номер	вершины - название остановки"
        std::unordered_map<size_t,std::string> vertex_num_stop_;
      
    //Итерпритатор вершин,представляющий из себя словарь с парой "название остановки - номер	вершины"
        std::unordered_map<std::string,size_t> stop_vertex_num_; 
      
      //Ссылка на транспортный каталог
        transport_directory::tr_cat::TransportCatalogue& catalogue_;
      
      //время ожидания на остновке
        int bus_wait_time_;
      
      //Скорость передвижени автобуса
        double bus_velocity_; 
      
      //Создал объект графа
        std::unique_ptr<graph::DirectedWeightedGraph<RouteTime>> graph_;
      
      //Создал указатель на объект router
        std::unique_ptr<graph::Router<RouteTime>> router_;
      
        void GreateGraphAndRoute(const std::vector<std::pair<std::string,bool>>& bus_all);
      
    public: 
        TransportRouter()=delete;
      
        TransportRouter( transport_directory::tr_cat::TransportCatalogue& catalogue, 
                    const RouteSetings& route_settings,
                    const std::vector<std::pair<std::string,bool>>& bus_all
                   );
      
        
      
        RouteInfoTranslete FindOptimalRoute(const std::string_view stop_from,const std::string_view stop_to) const;      
        
    };  
      
    
    
}