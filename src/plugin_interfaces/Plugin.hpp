#pragma once

#include <string>

#include <QFileInfo>
#include <QString>
#include <chaiscript/chaiscript.hpp>
#include <pajlada/settings/setting.hpp>

#include "common/QLogging.hpp"

namespace chatterino {

namespace plugin_interfaces {

    class Plugin;
    class API
    {
    public:
        std::string getStringSetting(std::string name)
        {
            auto val = pajlada::Settings::Setting<std::string>::get(name);
            return val;
        }
        void setStringSetting(std::string name, std::string value)
        {
            pajlada::Settings::Setting<std::string>::set(name, value);
        }
        void log(std::string message);

        API(Plugin *_owner)
            : owner(_owner){};

    private:
        Plugin *owner;
    };
    class Plugin
    {
    public:
        Plugin(QString filepath)
            : path(filepath)
        {
            this->load();
        }

        void load();

        ~Plugin(){};

        QString name()
        {
            return QFileInfo(this->path).baseName();
        };

        void initialize(){};

    private:
        const QString path;
        chaiscript::ChaiScript vm;
    };

}  // namespace plugin_interfaces
}  // namespace chatterino
