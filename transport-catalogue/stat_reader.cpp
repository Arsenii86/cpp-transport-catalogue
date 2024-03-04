#include "stat_reader.h"
#include <string_view>
#include <vector>
#include "transport_catalogue.h"
#include <iostream>
#include <iomanip>


namespace transport_directory{
    namespace stat_reader{
	void PrintBusStat(const std::string& bus_name,const tr_cat::TransportCatalogue& catalogue, std::ostream& output){
		tr_cat::TransportCatalogue* catalogue_iter = const_cast<tr_cat::TransportCatalogue*>(&catalogue); 
		auto root_iter = catalogue_iter -> FindRout(bus_name);
                if(root_iter != nullptr){
                    tr_cat::RouteInf inform = catalogue_iter -> GetRoutInform(bus_name);
                    output << "Bus " << bus_name << ": " << inform.stop_number << " stops on route, " << inform.unique_stop_number << " unique stops, " << std::setprecision(6) << inform.lenght_route << " route length" << std::endl;
                    }
                else{
                    output << "Bus " << bus_name << ": not found" << std::endl;
                }
}

	void PrintStopStat(const std::string& stop_name, const tr_cat::TransportCatalogue& catalogue, std::ostream& output){
	tr_cat::TransportCatalogue* catalogue_iter = const_cast<tr_cat::TransportCatalogue*>(&catalogue);
	auto root_iter = catalogue_iter -> FindStop(stop_name);
                    if(root_iter != nullptr){
                        auto iter_buses = (catalogue_iter -> GetBusThroughStop(stop_name));
                        if (iter_buses == nullptr){
                            output << "Stop " << stop_name << ": no buses" << std::endl; 
                        }
                        else{
                            output << "Stop " << stop_name << ": buses";
                            for(auto iter = begin(*iter_buses); iter != end(*iter_buses); iter++){
                                output << ' ' << std::string(*iter);
                            }
                            output << std::endl;
                        }
                    }
                    else{
                       output << "Stop " << stop_name << ": not found" << std::endl; 
                    } 
}

	
        void ParseAndPrintStat(const tr_cat::TransportCatalogue& tansport_catalogue, std::string_view request, std::ostream& output) {  
            auto space_pos = request.find(' ');
            auto not_space = request.find_first_not_of(' ', space_pos);
            std::string command = std::string(request.substr(0, space_pos));
            std::string name = std::string(request.substr(not_space)); 
            if (command == "Bus"){    
                PrintBusStat(name, tansport_catalogue, output); 
            }
            if (command == "Stop"){ 
                PrintStopStat(name, tansport_catalogue, output); 
            }
        }
    }
}

    