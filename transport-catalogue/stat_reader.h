#pragma once

#include <iosfwd>
#include <string_view>
#include <string>
#include "transport_catalogue.h"

namespace transport_directory{
    namespace stat_reader{
    	void ParseAndPrintStat(const tr_cat::TransportCatalogue& tansport_catalogue, std::string_view request, std::ostream& output);
        void PrintBusStat(const std::string& bus_name, const tr_cat::TransportCatalogue& catalogue, std::ostream& output);
        void PrintStopStat(const std::string& stop_name, const tr_cat::TransportCatalogue& catalogue, std::ostream& output);

    }
}