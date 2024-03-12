#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include "geo.h"
#include "transport_catalogue.h"

namespace transport_directory{
    namespace input_reader{
        struct CommandDescription {
            // Определяет, задана ли команда (поле command непустое)
            explicit operator bool() const {
                return !command.empty();
            }

            bool operator!() const {
                return !operator bool();
            }

            std::string command;      // Название команды
            std::string id;           // id маршрута или остановки
            std::string description;  // Параметры команды
        };

        class InputReader {
        public:    
            void ParseLine(std::string_view line);
            void ApplyCommands(tr_cat::TransportCatalogue& catalogue) const;

        private:
            std::vector<CommandDescription> commands_;
        };
    }
}


