#pragma once

#include "common/Singleton.hpp"

#include "singletons/Settings.hpp"

#include <QtPlugin>

namespace chatterino {

class Plugins final : public Singleton
{
private:
    std::vector<QObject *> items_;

public:
    Plugins();
    void initialize(Settings &settings, Paths &paths) override;

    // completers
    void forEachPlugin(std::function<void(QObject *plugin)> action);
};

}  // namespace chatterino
