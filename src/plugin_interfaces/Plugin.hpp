#pragma once

#include <QString>

#include "plugin_interfaces/Completer.hpp"
#include "plugin_interfaces/Plugin.hpp"

namespace chatterino {

namespace plugin_interfaces {

    class Plugin
    {
    public:
        virtual ~Plugin(){};
        virtual QString name() const = 0;
    };

}  // namespace plugin_interfaces
}  // namespace chatterino

Q_DECLARE_INTERFACE(chatterino::plugin_interfaces::Plugin,
                    "pl.kotmisia.dankerino.plugins.base/1.0");
