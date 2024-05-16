#pragma once
#include "transport_catalogue.h"
#include "graph.h"
#include "router.h"
#include <unordered_map>
#include <map>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <iterator>
////////
#include "json_builder.h"
#include <iostream>
////////

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
        double time_ = 0.0;
        std::vector<const graph::Edge<transport_router::RouteTime>*> route_edges_inform_;
        
        RouteInfoTranslete() = default;
        RouteInfoTranslete(double time,const std::vector<const graph::Edge<transport_router::RouteTime>*>& route_edges_inform):time_(time),route_edges_inform_(std::move(route_edges_inform)){};
    };

    
    void JsonBuildForRoute(json::Builder& answer,
                           const RouteInfoTranslete& optimal_route,
                           const std::unordered_map<size_t,std::string>& VertexNum_Stop,
                           int bus_wait_time,
                           int request_id);
    
    
    
    
  class TransportRouter{
      //итерпритатор вершин,представляющий из себя словарь с парой "номер	вершины - название остановки"
      std::unordered_map<size_t,std::string> vertex_num_stop_;
      //итерпритатор вершин,представляющий из себя словарь с парой "название остановки - номер	вершины"
      std::unordered_map<std::string,size_t> stop_vertex_num_; 
      //ссылка на транспортный каталог
      transport_directory::tr_cat::TransportCatalogue& catalogue_;
      //время ожидания на остновке
      int bus_wait_time_;
      //скорость передвижени автобуса
      double bus_velocity_;    
      //Создал объект графа
      graph::DirectedWeightedGraph<RouteTime>* graph_;
      //Создал указатель на объект router
      graph::Router<RouteTime>* router_;
      
      void GreateGrafAndRoute(const std::vector<std::pair<std::string,bool>>& bus_all);
      
      public: 
    TransportRouter()=delete;
      
    TransportRouter( transport_directory::tr_cat::TransportCatalogue& catalogue, 
                    const RouteSetings& route_settings,
                    const std::vector<std::pair<std::string,bool>>& bus_all
                   );
      
        
      
    RouteInfoTranslete FindOptimalRoute(const std::string& stop_from,const std::string& stop_to) const;
      
    int GetWaitTime() const {
        return bus_wait_time_;
    }
     
    const std::unordered_map<size_t,std::string>& GetVertexStop() const{
        return vertex_num_stop_;
    }
     
      
    ~  TransportRouter(){
        delete router_; 
        delete graph_;
        router_ = nullptr;
        graph_ = nullptr;
    }
      
  };  
      
    
    
}