#include "transport_router.h"

namespace transport_router{
    
    void TransportRouter::GreateAdgeForRoute( const std::string& bus, bool is_round){
            const auto route = catalogue_.FindRoute(bus);//Нахожу указатель на маршрут

            size_t stop_count = route->stops.size();
            if (is_round){
                for (auto stop_iter_from= route->stops.begin(); stop_iter_from != (route->stops.end()) - 1; stop_iter_from += 1){
                    auto stop_iter_end = (route->stops.end());
                    GreateAddEdge(stop_iter_from, stop_iter_end, bus);
                }                
            }
            else{
                
                for (auto stop_iter_from = route->stops.begin(); stop_iter_from != route->stops.begin() + stop_count/2 + 1; stop_iter_from+=1){
                    auto stop_iter_end = route->stops.begin() + stop_count/2 + 1;
                    GreateAddEdge(stop_iter_from, stop_iter_end, bus);
                }

                for (auto stop_iter_from = route->stops.begin() + stop_count/2; stop_iter_from != route->stops.end(); stop_iter_from += 1){
                    auto stop_iter_end =  route->stops.end();
                    GreateAddEdge(stop_iter_from, stop_iter_end, bus);
                }
            } 
        } 
        
    void TransportRouter::GreateGraphAndRoute(const std::vector<std::pair<std::string,bool>>& bus_all){
        using namespace std::literals; 
        
        /*        
        Заполняю граф маршрутами.
        у меня есть вектор с номерами автобусов и определителем круговой ли это маршрут
        прохожусь по нему
        */
        
        for (const auto& [bus, is_round]:bus_all){
            GreateAdgeForRoute( bus,is_round);
        }
        
        //Создаю в динамической памяти объект клсса роутер
        router_= std::move(std::make_unique<graph::Router<RouteTime>>(*graph_));
    }
    
    
    TransportRouter::TransportRouter( transport_directory::tr_cat::TransportCatalogue& catalogue,
                                      const RouteSetings& route_settings,
                                      const std::vector<std::pair<std::string,bool>>& bus_all):
                      catalogue_(catalogue), bus_wait_time_(route_settings.bus_wait_time){                                  
            bus_velocity_ = route_settings.bus_velocity*1000/60;
            std::vector<std::string_view> stop_in_routes_only = std::move(catalogue_.GetStopInRoutesOnly());
            size_t vertex_count = stop_in_routes_only.size();
            graph_=std::move(std::make_unique<graph::DirectedWeightedGraph<RouteTime>>(vertex_count)); 
            vertex_num_stop_.reserve(vertex_count);          
            stop_vertex_num_.reserve(vertex_count);          
            size_t i = 0;
            for(const auto stop:stop_in_routes_only){
               vertex_num_stop_[i] = std::string(stop);                
               ++i;
            }
            for (const auto&[vertex,stop]:vertex_num_stop_){
                stop_vertex_num_[stop] = vertex;                 
            }  
            TransportRouter::GreateGraphAndRoute(bus_all);              
        }
    
    
    
    RouteInfoTranslete TransportRouter::FindOptimalRoute(const std::string_view stop_from,
                                         const std::string_view stop_to) const{  
        
        //Привожу название остановок в номерам вершин
         size_t from = stop_vertex_num_.at(std::string(stop_from));
         size_t to = stop_vertex_num_.at(std::string(stop_to));
        
         //Нахожу оптимальный маршрут 
        auto route_opt = router_->BuildRoute(from,to);
        
        //Проверяю, если маршрут не найден
        if (route_opt==std::nullopt){
           return RouteInfoTranslete(-1,-1,{},{});
        }
        
        //Если найден, то
        else{ 
            
            //Сохраняю ссылку на найднный маршрут внутри optional
            const graph::Router<transport_router::RouteTime>::RouteInfo& route = *route_opt;
            
            //Извлекаю суммарный вес маршрута 
            double time= route.weight.time_between_stop_;
            
            //Сохраняю ссылку на вектор с номерами граней внутри маршрута
            const auto& edges_id = route.edges;
            
            //Создаю вектор, куда в соответствии с номерами граней будут сохранены указатели на соотвествующие грани 
            std::vector< const graph::Edge<transport_router::RouteTime>*> route_edges_inform;
            
            //В цикле прохожусь по номерам граней и извлекаю соответствующю грань из графа
            for(const auto& edge_id:edges_id){
                const auto& edge = graph_->GetEdge(edge_id);  
                
                //Сохраняю указатель на грань в соответствующий вектор route_edges_inform;
                route_edges_inform.push_back(&edge); 
            }
            return RouteInfoTranslete (time, bus_wait_time_, route_edges_inform, vertex_num_stop_);
        }
    }
    
    
          
    
}