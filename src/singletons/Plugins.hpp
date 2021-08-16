#pragma once

#include "common/Singleton.hpp"

#include "plugin_interfaces/Plugin.hpp"
#include "singletons/Settings.hpp"

#include <QtPlugin>

namespace chatterino {

class Plugins final : public Singleton
{
private:
    std::vector<plugin_interfaces::Plugin *> items_;

public:
    Plugins();
    void initialize(Settings &settings, Paths &paths) override;

    void forEachPlugin(
        std::function<void(plugin_interfaces::Plugin *plugin)> action);
};

}  // namespace chatterino
