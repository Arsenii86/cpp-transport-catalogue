#include "stat_reader.h"
#include <string_view>
#include <vector>
#include "transport_catalogue.h"
#include <iostream>
#include <iomanip>
namespace transport_directory{
    namespace stat_reader{
        void ParseAndPrintStat(const tr_dir::TransportCatalogue& tansport_catalogue, std::string_view request,std::ostream& output) {  

            auto space_pos = request.find(' ');
            auto not_space = request.find_first_not_of(' ', space_pos);
            std::string command=std::string(request.substr(0,space_pos));
            std::string name=std::string(request.substr(not_space));      
            auto catalogue_iter=const_cast<tr_dir::TransportCatalogue*>(&tansport_catalogue); 
            if (command=="Bus"){    
                auto root_iter=catalogue_iter->FindRout(name);
                if(root_iter!=nullptr){
                    auto [stop_number,unique_stop,lenght]=catalogue_iter->GetRoutInform(name);
                    output<<"Bus "<<name<<": "<<stop_number<<" stops on route, "<<unique_stop<<" unique stops, "<<std::setprecision(6)<<lenght<<" route length"<<std::endl;
                    }
                else{
                    output<<"Bus "<<name<<": not found"<<std::endl;
                }
            }
            if (command=="Stop"){ 
                auto root_iter=catalogue_iter->FindStop(name);
                    if(root_iter!=nullptr){
                        auto iter_buses=(catalogue_iter->GetBusThroughStop(name));
                        if (iter_buses==nullptr){
                            output<<"Stop "<<name<<": no buses"<<std::endl; 
                        }
                        else{
                            output<<"Stop "<<name<<": buses";
                            for(auto iter=begin(*iter_buses);iter!=end(*iter_buses);iter++){
                                output<<' '<<std::string(*iter);
                            }
                            output<<std::endl;
                        }
                    }
                    else{
                       output<<"Stop "<<name<<": not found"<<std::endl; 
                    }
            }
        }
    }
}