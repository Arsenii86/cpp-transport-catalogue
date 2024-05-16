#include "transport_router.h"

namespace transport_router{
        
    void TransportRouter::GreateGrafAndRoute(const std::vector<std::pair<std::string,bool>>& bus_all){
        using namespace std::literals; 
        /*        
        заполняю граф маршрутами.
        у меня есть вектор с номерами автобусов и определителем круговой ли это маршрут
        прохожусь по нему
        */
        for (const auto& [bus, is_round]:bus_all){
            const auto route = catalogue_.FindRoute(bus);//нахожу указатель на маршрут

            size_t stop_count = route->stops.size();
            if (is_round){
                for (auto stop_iter_from= route->stops.begin(); stop_iter_from != (route->stops.end()) - 1; stop_iter_from += 1){
                    double road_dist_summ = 0;
                    int span_count = 0;
                    auto stop_iter = stop_iter_from;
                    for (auto stop_iter_to = stop_iter_from + 1; stop_iter_to != (route->stops.end()); stop_iter_to += 1){
                        ++span_count;
                        //нахожу расстояние между соседними остановками
                        //расстояние в метрах, а скорость в км/час, а время суммарное должно быть в минутах 
                        
                        road_dist_summ += catalogue_.GetRoadDist((**stop_iter).name,(**stop_iter_to).name);
                        
                        //нахождение времени на преодаление расстояния между остановками
                        double time_between_stop = road_dist_summ/bus_velocity_ + bus_wait_time_;
                        size_t from = stop_vertex_num_.at((**stop_iter_from).name);
                        size_t to = stop_vertex_num_.at((**stop_iter_to).name);
                        graph::Edge edge(from, to , RouteTime{bus ,time_between_stop, span_count});   
                        graph_->AddEdge(edge);                    
                        stop_iter = stop_iter_to;
                    }
                }
                
            }
            else{
                
                for (auto stop_iter_from = route->stops.begin(); stop_iter_from != route->stops.begin() + stop_count/2 + 1; stop_iter_from+=1){
                    double road_dist_summ=0;
                    int span_count = 0;
                    auto stop_iter = stop_iter_from;
                    for (auto stop_iter_to = stop_iter_from+1; stop_iter_to != route->stops.begin() + stop_count/2 + 1; stop_iter_to += 1){
                        ++span_count;
                        //нахожу расстояние между соседними остановками
                        //расстояние в метрах, а скорость в км/час, а время суммарное должно быть в минутах 
                        road_dist_summ += catalogue_.GetRoadDist((**stop_iter).name,(**stop_iter_to).name);
                        //нахождение времени на преодаление расстояния между остановками
                        double time_between_stop = road_dist_summ/bus_velocity_ + bus_wait_time_;
                        size_t from = stop_vertex_num_.at((**stop_iter_from).name);
                        size_t to = stop_vertex_num_.at((**stop_iter_to).name);
                        graph::Edge edge(from, to, RouteTime{bus, time_between_stop, span_count});   
                        graph_->AddEdge(edge);
                        stop_iter = stop_iter_to;
                    }
                }

                for (auto stop_iter_from = route->stops.begin() + stop_count/2; stop_iter_from != route->stops.end(); stop_iter_from += 1){
                    double road_dist_summ=0;
                    int span_count = 0;
                    auto stop_iter=stop_iter_from;
                    for (auto stop_iter_to = stop_iter_from+1; stop_iter_to != route->stops.end(); stop_iter_to += 1){
                        ++span_count;
                        //нахожу расстояние между соседними остановками
                        //расстояние в метрах, а скорость в км/час, а время суммарное должно быть в минутах 
                        road_dist_summ += catalogue_.GetRoadDist((**stop_iter).name,(**stop_iter_to).name);
                        //нахождение времени на преодаление расстояния между остановками
                        double time_between_stop = road_dist_summ/bus_velocity_ + bus_wait_time_;
                        size_t from = stop_vertex_num_.at((**stop_iter_from).name);
                        size_t to = stop_vertex_num_.at((**stop_iter_to).name);
                        graph::Edge edge(from, to, RouteTime{bus ,time_between_stop, span_count});   
                        graph_->AddEdge(edge);
                        stop_iter = stop_iter_to;
                    }
                }
            } 
        }
        //Создаю в динамической памяти объект клсса роутер
        
       router_ = new graph::Router<RouteTime>(*graph_);
    }
    
    
    TransportRouter::TransportRouter( transport_directory::tr_cat::TransportCatalogue& catalogue,
                                      const RouteSetings& route_settings,
                                      const std::vector<std::pair<std::string,bool>>& bus_all):
                      catalogue_(catalogue), bus_wait_time_(route_settings.bus_wait_time){                                  
            bus_velocity_ = route_settings.bus_velocity*1000/60;
            std::vector<std::string_view> stop_in_routes_only = std::move(catalogue_.GetStopInRoutesOnly());
            size_t vertex_count = stop_in_routes_only.size();  
            graph_= new graph::DirectedWeightedGraph<RouteTime> (vertex_count);    
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
            TransportRouter::GreateGrafAndRoute(bus_all);              
        }
    
    
    
    RouteInfoTranslete TransportRouter::FindOptimalRoute(const std::string& stop_from,
                                         const std::string& stop_to) const{  
        //Привожу название остановок в номерам вершин
         size_t from = stop_vertex_num_.at(stop_from);
         size_t to = stop_vertex_num_.at(stop_to);
         //Нахожу оптимальный маршрут 
       auto route_opt = router_->BuildRoute(from,to);
        //Проверяю, если маршрут не найден
       if (route_opt==std::nullopt){
           return RouteInfoTranslete(-1,{});
       }
        //Если найден, то
        else{ 
            //сохраняю ссылку на найднный маршрут внутри optional
            const graph::Router<transport_router::RouteTime>::RouteInfo& route = *route_opt;
            //извлекаю суммарный вес маршрута 
            double time= route.weight.time_between_stop_;
            //сохраняю ссылку на вектор с номерами граней внутри маршрута
            const auto& edges_id = route.edges;
            //создаю вектор, куда в соответствии с номерами граней будут сохранены указатели на соотвествующие грани 
            std::vector< const graph::Edge<transport_router::RouteTime>*> route_edges_inform;
            //std::vector<const graph::Edge<transport_router::RouteTime>*> route_edges_inform;
            //в цикле прохожусь по номерам граней и извлекаю соответствующю грань из графа
            for(const auto& edge_id:edges_id){
                const auto& edge = graph_->GetEdge(edge_id);                             
                //Сохраняю указатель на грань в соответствующий вектор route_edges_inform;
                route_edges_inform.push_back(&edge); 
            }
            return RouteInfoTranslete (time,route_edges_inform);
        }
    }
    
    
          
    void JsonBuildForRoute(json::Builder& answer,
                           const RouteInfoTranslete& optimal_route,
                           const std::unordered_map<size_t,std::string>& VertexNum_Stop,
                           int bus_wait_time,
                           int request_id){
        
       const auto& route_edges_inform = optimal_route.route_edges_inform_;
        //Если время в оптимальном маршруте отриц, значит маршрут пустой
        if(optimal_route.time_==0){           
            answer.StartDict().
            Key("request_id").Value(request_id).
            Key("total_time").Value(optimal_route.time_).
            Key("items").StartArray().
            EndArray().    
            EndDict();            
        }  
        else if(optimal_route.time_==-1){
            answer.StartDict().
                            Key("request_id").Value(request_id).
                            Key("error_message").Value("not found").
                            EndDict();
        }
        else{ 
            answer.StartDict();
            answer.Key("request_id").Value(request_id);                
            answer.Key("total_time").Value(optimal_route.time_);
            answer.Key("items").StartArray(); 
            for(auto iter=route_edges_inform.begin();iter!=route_edges_inform.end();++iter){
                double time_summ_bus_stop = (**iter).weight.time_between_stop_; 
                double time_on_bus = time_summ_bus_stop-bus_wait_time;
                int edge_id = (**iter).from;
                int span_count =(**iter).weight.span_count_; 
                std::string stop_bus =  VertexNum_Stop.at(edge_id);  
                std::string bus = (**iter).weight.bus_or_wait_;
                answer.StartDict().
                    Key("type"s).Value("Wait").
                    Key("stop_name").Value(stop_bus).
                    Key("time").Value(bus_wait_time).
                    EndDict();
                answer.StartDict().
                    Key("bus").Value(bus).
                    Key("span_count").Value(span_count).
                    Key("time").Value(time_on_bus).
                    Key("type").Value("Bus").   
                    EndDict(); 
            }             
            answer.EndArray();
            answer.EndDict();
            
            }
        }
 
    
    
    
}