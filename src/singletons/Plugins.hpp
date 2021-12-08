#pragma once

#include "common/Singleton.hpp"

#include "plugin_interfaces/Plugin.hpp"
#include "singletons/Settings.hpp"

namespace chatterino {

class Plugins final : public Singleton
{
private:
    std::vector<std::shared_ptr<plugin_interfaces::Plugin>> items_;

public:
    void initialize(Settings &settings, Paths &paths) override;

    void forEachPlugin(
        std::function<void(std::shared_ptr<plugin_interfaces::Plugin> plugin)>
            action);
};

}  // namespace chatterino
