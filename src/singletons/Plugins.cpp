#include "singletons/Plugins.hpp"

#include "common/QLogging.hpp"
#include "singletons/Paths.hpp"

#include "plugin_interfaces/Completer.hpp"

#include <QDir>

namespace chatterino {

void Plugins::initialize(Settings &settings, Paths &paths)
{
    if (!settings.enablePlugins)
    {
        return;
    }
    QDir pluginsDir = paths.pluginsDirectory;

    const auto entryList = pluginsDir.entryList(QDir::Files);
    for (const QString &fileName : entryList)
    {
        if (fileName.endsWith(".chai"))
        {
            this->items_.push_back(std::make_shared<plugin_interfaces::Plugin>(
                pluginsDir.absoluteFilePath(fileName)));
        }
    }
}

void Plugins::forEachPlugin(
    std::function<void(std::shared_ptr<plugin_interfaces::Plugin>)> action)
{
    for (auto plugin : this->items_)
    {
        action(plugin);
    }
}

}  // namespace chatterino
