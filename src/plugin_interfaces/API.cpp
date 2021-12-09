#include "API.hpp"

#include <pajlada/settings/setting.hpp>

#include "plugin_interfaces/Plugin.hpp"

namespace chatterino {
namespace plugin_interfaces {
    std::string API::getStringSetting(std::string name)
    {
        auto val = pajlada::Settings::Setting<std::string>::get(name);
        return val;
    }
    void API::setStringSetting(std::string name, std::string value)
    {
        pajlada::Settings::Setting<std::string>::set(name, value);
    }

    void API::log(std::string message)
    {
        qDebug() << owner->name() << QString::fromStdString(message);
    }
}  // namespace plugin_interfaces
}  // namespace chatterino
