#include "Plugin.hpp"

#include <QMessageBox>

#include "Application.hpp"
#include "plugin_interfaces/API.hpp"
#include "singletons/WindowManager.hpp"
#include "widgets/Window.hpp"

namespace chatterino {

namespace plugin_interfaces {
    void Plugin::load()
    {
        this->vm.add_global(chaiscript::var<API *>(new API(this)), "api");
        this->vm.add(chaiscript::fun(&API::log), "log");
        this->vm.add(chaiscript::fun(&API::getStringSetting),
                     "getStringSetting");
        this->vm.add(chaiscript::fun(&API::setStringSetting),
                     "setStringSetting");
        this->vm.add(
            chaiscript::user_type<CompletionModel::TaggedString::Type>(),
            "TaggedStringType");

        auto m = chaiscript::ModulePtr(new chaiscript::Module());
        using TS = CompletionModel::TaggedString::Type;
        chaiscript::utility::add_class<TS>(
            *m, "TaggedStringType",
            {
                {TS::Username, "Username"},
                {TS::EmoteStart, "EmoteStart"},
                {TS::FFZGlobalEmote, "FFZGlobalEmote"},
                {TS::FFZChannelEmote, "FFZChannelEmote"},
                {TS::SEVENTVGlobalEmote, "SEVENTVGlobalEmote"},
                {TS::SEVENTVChannelEmote, "SEVENTVChannelEmote"},
                {TS::BTTVGlobalEmote, "BTTVGlobalEmote"},
                {TS::BTTVChannelEmote, "BTTVChannelEmote"},
                {TS::TwitchGlobalEmote, "TwitchGlobalEmote"},
                {TS::TwitchLocalEmote, "TwitchLocalEmote"},
                {TS::TwitchSubscriberEmote, "TwitchSubscriberEmote"},
                {TS::Emoji, "Emoji"},
                {TS::EmoteEnd, "EmoteEnd"},
                {TS::Command, "Command"},
            });
        this->vm.add(m);
        try
        {
            this->vm.eval_file(
                this->path.toStdString(),
                chaiscript::exception_specification<
                    const chaiscript::exception::eval_error &>());
        }
        catch (const chaiscript::exception::eval_error e)
        {
            qDebug() << "WutFace" << e.what();
            auto msgBox =
                new QMessageBox(QMessageBox::Icon::Warning, "Script error",
                                QString("There was an error while initializing "
                                        "your script named \"%1\": \n%2")
                                    .arg(this->name(), e.what()),
                                QMessageBox::Ok);
            msgBox->show();
        }
    }

    chaiscript::Boxed_Value Plugin::getCallbackFunction(QString eventName)
    {
        std::string funcName = QString("on_%1").arg(eventName).toStdString();
        try
        {
            return this->vm(funcName,
                            chaiscript::exception_specification<
                                const chaiscript::exception::eval_error &>());
        }
        catch (const chaiscript::exception::eval_error e)
        {
            if (e.reason == ("Can not find object: " + funcName))
            {
                qDebug() << "no function.........." << e.what();
                // plugin doesn't support this callback, ignore...
                return chaiscript::Boxed_Value();
            }
            qDebug() << "WutFace2" << e.what();
            auto msgBox = new QMessageBox(
                QMessageBox::Icon::Warning, "Script error",
                QString(
                    "There was an error while executing an event handler for "
                    "your script named \"%1\": \n%2")
                    .arg(this->name(), e.what()),
                QMessageBox::Ok);
            msgBox->show();
            return chaiscript::Boxed_Value();
        }
    }

    bool Plugin::refreshCustomCompletions(
        std::function<void(const QString &str,
                           CompletionModel::TaggedString::Type type)>
            addString,
        QString prefix, QString message, bool isFirstWord, Channel &channel)
    {
        auto func = this->getCallbackFunction("completions_refresh_requested");
        if (!func.is_null())
        {
            try
            {
                auto function = chaiscript::boxed_cast<std::function<bool(
                    chaiscript::Proxy_Function, std::string prefix,
                    std::string message, bool isFirstWord, Channel &channel)>>(
                    func);
                return function(
                    chaiscript::fun(
                        [addString](std::string str,
                                    CompletionModel::TaggedString::Type type) {
                            addString(QString::fromStdString(str), type);
                        }),
                    prefix.toStdString(), message.toStdString(), isFirstWord,
                    channel);
            }
            catch (chaiscript::exception::bad_boxed_cast e)
            {
                auto msgBox =
                    new QMessageBox(QMessageBox::Icon::Warning, "Script error",
                                    QString("There was an error while "
                                            "executing an event handler for "
                                            "your script named \"%1\": \n%2")
                                        .arg(this->name(), e.what()),
                                    QMessageBox::Ok);
                msgBox->exec();
            }
            catch (chaiscript::exception::eval_error e)
            {
                auto msgBox =
                    new QMessageBox(QMessageBox::Icon::Warning, "Script error",
                                    QString("There was an error while "
                                            "executing an event handler for "
                                            "your script named \"%1\": \n%2")
                                        .arg(this->name(), e.what()),
                                    QMessageBox::Ok);
                msgBox->exec();
            }
        }
        else
        {
            qDebug() << "no function...";
        }
        return true;
    }
}  // namespace plugin_interfaces
}  // namespace chatterino
