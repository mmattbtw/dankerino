#pragma once

#include <string>

namespace chatterino {
namespace plugin_interfaces {

    class Plugin;
    /**
     * @brief Represents Chatterino to the loaded Plugin.
     */
    class API
    {
    public:
        std::string getStringSetting(std::string name);
        void setStringSetting(std::string name, std::string value);
        void log(std::string message);

        API(Plugin *_owner)
            : owner(_owner){};

    private:
        Plugin *owner;
    };

}  // namespace plugin_interfaces
}  // namespace chatterino
