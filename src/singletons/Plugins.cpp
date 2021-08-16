#include "singletons/Plugins.hpp"

#include "singletons/Paths.hpp"

// for side-effects
#include "plugin_interfaces/Completer.hpp"

#include <QPluginLoader>

namespace chatterino {

void Plugins::initialize(Settings &settings, Paths &paths)
{
    QDir pluginsDir = paths.miscDirectory;
    pluginsDir.cd("plugins");

    const auto entryList = pluginsDir.entryList(QDir::Files);
    for (const QString &fileName : entryList)
    {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin)
        {
            this->items_.push_back(plugin);
        }
        else
        {
            qDebug() << "unable to load" << fileName;
        }
    }
}

Plugins::Plugins()
{
}

void Plugins::forEachPlugin(std::function<void(QObject *)> action)
{
    for (QObject *plugin : QPluginLoader::staticInstances())
    {
        action(plugin);
    }
    for (QObject *plugin : this->items_)
    {
        action(plugin);
    }
}

}  // namespace chatterino
