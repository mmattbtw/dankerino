#pragma once

#include "plugin_interfaces/Plugin.hpp"

namespace chatterino {

namespace plugin_interfaces {

    class SettingsPlugin
    {
    private:
        std::function<QString(QString, QString)> realGetString_;
        std::function<void(QString, QString)> realSetString_;

    public:
        virtual ~SettingsPlugin(){};
        virtual void openSettings() = 0;

        /**
          * gets a setting's value by path
         */
        QString getString(QString name, QString defaultValue)
        {
            return this->realGetString_(name, defaultValue);
        };

        void setString(QString name, QString value)
        {
            return this->realSetString_(name, value);
        }

        void setSettingsFunctionPointers(decltype(realGetString_) realGetString,
                                         decltype(realSetString_) realSetString)
        {
            this->realGetString_ = realGetString;
            this->realSetString_ = realSetString;
        }
    };

}  // namespace plugin_interfaces
}  // namespace chatterino

Q_DECLARE_INTERFACE(chatterino::plugin_interfaces::SettingsPlugin,
                    "pl.kotmisia.dankerino.plugins.settings/1.0");
