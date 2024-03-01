#pragma once

#include <iosfwd>
#include <string_view>

#include "transport_catalogue.h"
namespace transport_directory{
    namespace stat_reader{
    void ParseAndPrintStat(const tr_dir::TransportCatalogue& tansport_catalogue, std::string_view request,
                           std::ostream& output);
    }
}