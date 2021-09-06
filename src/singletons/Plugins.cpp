#include "singletons/Plugins.hpp"

#include "common/QLogging.hpp"
#include "messages/MessageBuilder.hpp"
#include "singletons/Paths.hpp"

#include "plugin_interfaces/Completer.hpp"
#include "plugin_interfaces/MessageCreator.hpp"
#include "plugin_interfaces/SettingsPlugin.hpp"

#include <QDir>
#include <QPluginLoader>

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
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin)
        {
            qCDebug(chatterinoPlugins) << "Loaded" << fileName;
            auto pl = dynamic_cast<plugin_interfaces::Plugin *>(plugin);
            if (pl)
            {
                this->items_.push_back(pl);
            }
        }
        else
        {
            qCWarning(chatterinoPlugins)
                << "unable to load" << fileName << loader.errorString();
        }
    }
    // initialize plugins!
    this->forEachPlugin([](plugin_interfaces::Plugin *plugin) {
        {
            auto pl =
                dynamic_cast<plugin_interfaces::MessageCreatorPlugin *>(plugin);
            if (pl)
            {
                pl->setMessageCreationFunctionPointers(
                    [](const QString &text) {
                        return makeSystemMessage(text);
                    },
                    [](Channel &chan,
                       std::shared_ptr<const Message> message) -> void {
                        chan.addMessage(message);
                    },
                    [](Channel &chan) {
                        return int(chan.getType());
                    },
                    [](Channel &chan) {
                        return chan.getName();
                    });
            }
        }
        {
            auto pl = dynamic_cast<plugin_interfaces::SettingsPlugin *>(plugin);
            if (pl)
            {
                pl->setSettingsFunctionPointers(
                    [](QString name, QString defaultValue) -> QString {
                        auto val = pajlada::Settings::Setting<QString>::get(
                            name.toStdString());
                        return val.isNull() ? defaultValue : val;
                    },
                    [](QString name, QString value) -> void {
                        pajlada::Settings::Setting<QString>::set(
                            name.toStdString(), value);
                    });
            }
        }
        plugin->initialize();
    });
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
