#pragma once

#include <string>

#include <QFileInfo>
#include <QString>
#include <chaiscript/chaiscript.hpp>
#include <pajlada/settings/setting.hpp>

#include "common/CompletionModel.hpp"
#include "common/QLogging.hpp"

namespace chatterino {

namespace plugin_interfaces {

    class Plugin;
    /**
     * @brief Represents a loaded Plugin to c++ code.
     */
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

        bool refreshCustomCompletions(
            std::function<void(const QString &str,
                               CompletionModel::TaggedString::Type type)>
                addString,
            QString prefix, QString message, bool isFirstWord,
            Channel &channel);

        void initialize(){};

    private:
        const QString path;
        chaiscript::ChaiScript vm;

        chaiscript::Boxed_Value getCallbackFunction(QString eventName);
    };

}  // namespace plugin_interfaces
}  // namespace chatterino
