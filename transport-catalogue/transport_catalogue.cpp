#include "transport_catalogue.h"
namespace transport_directory{
    namespace tr_dir{
        void TransportCatalogue::InsertStop(std::string bus_stop,geo::Coordinates lat_lng){
            stop_base[bus_stop]=lat_lng;
        }

        void TransportCatalogue::InsertRout(std::string bus,std::vector<std::string_view> rout){
            std::deque <std::string> stop_list;
            for (const auto bus_stop:rout){
                stop_list.push_back(std::string(bus_stop));
            }
            route_base[bus]=std::move(stop_list);
        }

        const std::deque<std::string>* TransportCatalogue::FindRout(std::string_view bus){
                    if (route_base.find(std::string(bus))==route_base.end()){
                        return nullptr;
                        }
                    else{
                        return &route_base.at(std::string(bus));
                        }
                    }

        const geo::Coordinates* TransportCatalogue::FindStop(std::string_view bus_stop){
            if (stop_base.find(std::string(bus_stop))==stop_base.end()){
                        return nullptr;
                        }
                    else{
                        return &stop_base.at(std::string(bus_stop));
                        }
                    }

        std::tuple<int,int,double> TransportCatalogue::GetRoutInform(std::string_view bus){

            int stop_number=TransportCatalogue::FindRout(bus)->size();
            int unique_stop=0;    
            {
                std::vector<std::string_view> route(route_base.at(std::string(bus)).begin(),route_base.at(std::string(bus)).end());        
                std::sort(route.begin(),route.end());
                const auto ret =std::unique(route.begin(),route.end());
                route.erase(ret, route.end());
                unique_stop=route.size();        

            }    
            double lenght=.0;
                for (auto iter=(route_base.at(std::string(bus))).begin();iter<(route_base.at(std::string(bus))).end()-1;iter++){        
                    lenght+=ComputeDistance(*TransportCatalogue::FindStop(*iter),*TransportCatalogue::FindStop(*(iter+1)));
                }    
            return {stop_number,unique_stop,lenght};
        }


        const std::deque<std::string_view>* TransportCatalogue::GetBusThroughStop(std::string bus_stop){   
            //auto iter=find(stop_base.begin(),stop_base.end(),std::string(bus_stop));
            //auto iter_stop=&(iter->first);//string*
            std::deque<std::string_view>  buses;
            for(const auto &[bus,deque_stop]:route_base){
                if(find(deque_stop.begin(),deque_stop.end(),std::string(bus_stop))!=deque_stop.end()){
                     buses.push_back(bus);
                }    
            }   
            if (!buses.empty()) sort(buses.begin(),buses.end());
            buses_through_stop[bus_stop]=std::move(buses);
            if (buses_through_stop.at(bus_stop).empty()) return nullptr;
            else return &(buses_through_stop.at(bus_stop));

        };
    }
}