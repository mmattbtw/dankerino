#include "singletons/Plugins.hpp"

#include "singletons/Paths.hpp"

// for side-effects
#include "plugin_interfaces/Completer.hpp"

#include <QPluginLoader>

namespace chatterino {

void Plugins::initialize(Settings &settings, Paths &paths)
{
    if (!settings.enablePlugins)
    {
        return;
    }
    QDir pluginsDir = paths.miscDirectory;
    pluginsDir.cd("plugins");

    const auto entryList = pluginsDir.entryList(QDir::Files);
    for (const QString &fileName : entryList)
    {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin)
        {
            qDebug() << "Loaded" << fileName;
            auto pl = dynamic_cast<plugin_interfaces::Plugin *>(plugin);
            if (pl)
            {
                qDebug() << "plugin!";
                this->items_.push_back(pl);
            }
            else
            {
                qDebug() << "no plugin!";
            }
        }
        else
        {
            qDebug() << "unable to load" << fileName << loader.errorString();
        }
    }
}

Plugins::Plugins()
{
}

void Plugins::forEachPlugin(
    std::function<void(plugin_interfaces::Plugin *)> action)
{
    for (QObject *plugin : QPluginLoader::staticInstances())
    {
        auto pl = qobject_cast<plugin_interfaces::Plugin *>(plugin);
        if (pl)
        {
            action(pl);
        }
    }
    for (plugin_interfaces::Plugin *plugin : this->items_)
    {
        action(plugin);
    }
}

}  // namespace chatterino
