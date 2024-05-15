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

    struct RouteTime{    
       std::string bus_or_wait_;
       double time_between_stop_=0;
       int span_count_=0;
       constexpr RouteTime()=default;
       RouteTime(std::string bus_or_wait,double time_between_stop,int span_count):bus_or_wait_(bus_or_wait),time_between_stop_(time_between_stop),span_count_(span_count){};

        bool operator<(const RouteTime& rdt ) const{
            return (rdt.time_between_stop_-time_between_stop_)>1e-3;
        }
        bool operator>(const RouteTime& rdt ) const{
            return (time_between_stop_-rdt.time_between_stop_)>1e-3;
        }
        RouteTime operator+(const RouteTime& rdt )const {
            return RouteTime(bus_or_wait_,time_between_stop_+rdt.time_between_stop_,span_count_);
        }
        ~RouteTime()=default;
    };
    
   
    struct RouteInfoTranslete{
        double time_=0.0;
        std::vector<const graph::Edge<transport_router::RouteTime>*> route_edges_inform_;
        
        RouteInfoTranslete()=default;
        RouteInfoTranslete(double time,const std::vector<const graph::Edge<transport_router::RouteTime>*>& route_edges_inform):time_(time),route_edges_inform_(std::move(route_edges_inform)){};
    };

    
    
    
    graph::DirectedWeightedGraph<RouteTime> GreateGrafForAllRoute(transport_directory::tr_cat::TransportCatalogue& catalogue,
                                                const std::vector<std::pair<std::string,bool>>& bus_all,
                                                const std::unordered_map<std::string,size_t>& Stop_VertexNum,
                                                double bus_velocity,
                                                int bus_wait_time);


    RouteInfoTranslete  FindOptimalRoute(const graph::Router<transport_router::RouteTime>& router,
                                        const graph::DirectedWeightedGraph<transport_router::RouteTime>& graf,
                                        size_t from,
                                        size_t to);
    
    void JsonBuildForRoute(json::Builder& answer,
                           const RouteInfoTranslete& optimal_route,
                           const std::unordered_map<size_t,std::string>& VertexNum_Stop,
                           int bus_wait_time,
                           int request_id);
    
  struct TransportRouter{
      const graph::DirectedWeightedGraph<RouteTime>& graf_;
      const graph::Router<RouteTime>& router_;
      int bus_wait_time_;      
      const std::unordered_map<size_t,std::string>& VertexNum_Stop_;
      const std::unordered_map<std::string,size_t>& Stop_VertexNum_;
      TransportRouter()=delete;
      TransportRouter(const graph::DirectedWeightedGraph<RouteTime>& graf,
                     const graph::Router<RouteTime>& router,
                      int bus_wait_time,
                     const std::unordered_map<size_t,std::string>& VertexNum_Stop,
                     const std::unordered_map<std::string,size_t>& Stop_VertexNum ):
                     graf_(graf), router_(router),
                     bus_wait_time_(bus_wait_time),
                     VertexNum_Stop_(VertexNum_Stop),
                     Stop_VertexNum_(Stop_VertexNum){};      
  };
    
    
}