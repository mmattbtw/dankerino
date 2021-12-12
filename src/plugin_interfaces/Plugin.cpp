#include "Plugin.hpp"

#include <QMessageBox>

#include "Application.hpp"
#include "common/NetworkRequest.hpp"
#include "common/Outcome.hpp"
#include "plugin_interfaces/API.hpp"
#include "singletons/WindowManager.hpp"
#include "widgets/Window.hpp"

namespace {

void addNetworkRequest(chaiscript::ChaiScript &chai)
{
    using namespace chatterino;

    chai.add(chaiscript::user_type<NetworkRequest>(), "NetworkRequest");
    chai.add(chaiscript::constructor<NetworkRequest(const std::string,
                                                    NetworkRequestType)>(),
             "NetworkRequest");
    chai.add(chaiscript::fun([](NetworkRequest *that, NetworkErrorCallback cb) {
                 return std::move(*that).onError(cb);
             }),
             "onError");
    chai.add(
        chaiscript::fun([](NetworkRequest *that, NetworkFinallyCallback cb) {
            return std::move(*that).finally(cb);
        }),
        "finally");
    chai.add(chaiscript::fun([](NetworkRequest *that, int ms) {
                 return std::move(*that).timeout(ms);
             }),
             "timeout");
    chai.add(chaiscript::fun([](NetworkRequest *that) {
                 return std::move(*that).cache();
             }),
             "cache");
    chai.add(chaiscript::fun(&NetworkRequest::execute), "execute");
    chai.add(
        chaiscript::fun([](NetworkRequest *that, const std::string headerName,
                           const std::string value) {
            return std::move(*that).header(headerName.c_str(), value.c_str());
        }),
        "header");
    chai.add(
        chaiscript::fun([](NetworkRequest *that, NetworkSuccessCallback cb) {
            return std::move(*that).onSuccess(cb);
        }),
        "onSuccess");
    {
        auto m = chaiscript::ModulePtr(new chaiscript::Module());
        using ReqTyp = NetworkRequestType;
        chaiscript::utility::add_class<ReqTyp>(*m, "NetworkRequestType",
                                               {
                                                   {ReqTyp::Get, "GET"},
                                                   {ReqTyp::Post, "POST"},
                                                   {ReqTyp::Put, "PUT"},
                                                   {ReqTyp::Delete, "DELETE"},
                                                   {ReqTyp::Patch, "PATCH"},
                                               });
        chai.add(m);
    }

    chai.add(chaiscript::user_type<NetworkResult>(), "NetworkResult");
    chai.add(chaiscript::fun(&NetworkResult::parseJson), "parseJson");
    chai.add(chaiscript::fun(&NetworkResult::parseJsonArray), "parseJsonArray");
    chai.add(chaiscript::fun(&NetworkResult::getData), "getData");
    chai.add(chaiscript::fun(&NetworkResult::status), "status");
}
void addJSON(chaiscript::ChaiScript &chai)
{
    chai.add(chaiscript::fun([](QJsonObject *that, const std::string &key) {
                 return that->value(QString::fromStdString(key));
             }),
             "value");

    chai.add(chaiscript::fun([](QJsonObject *that) {
                 return QString("QJsonObject(%1 keys)").arg(that->size());
             }),
             "to_string");
    chai.add(chaiscript::fun([](QJsonArray *that) {
                 return QString("QJsonArray(%1 elements)").arg(that->size());
             }),
             "to_string");
    chai.add(chaiscript::fun([](QJsonValue *) {
                 return QString("QJsonValue()");
             }),
             "to_string");
    chai.add(chaiscript::fun([](QJsonValueRef *) {
                 return QString("QJsonValueRef()");
             }),
             "to_string");

    chai.add(chaiscript::fun(&QJsonValue::isNull), "isNull");
    chai.add(chaiscript::fun(&QJsonValue::isBool), "isBool");
    chai.add(chaiscript::fun(&QJsonValue::isDouble), "isDouble");
    chai.add(chaiscript::fun(&QJsonValue::isString), "isString");
    chai.add(chaiscript::fun(&QJsonValue::isArray), "isArray");
    chai.add(chaiscript::fun(&QJsonValue::isObject), "isObject");
    chai.add(chaiscript::fun(&QJsonValue::isUndefined), "isUndefined");

    //chai.add(
    //    chaiscript::fun<bool (QJsonValue::*)(bool) const>(&QJsonValue::toBool),
    //    "toBool");
    chai.add(chaiscript::fun(&QJsonValue::toBool), "toBool");
    //chai.add(
    //    chaiscript::fun<int (QJsonValue::*)(int) const>(&QJsonValue::toInt),
    //    "toInt");
    chai.add(chaiscript::fun(&QJsonValue::toInt), "toInt");

    chai.add(chaiscript::fun([](QJsonValue *that) {
                 return that->toString().toStdString();
             }),
             "to_string");
    //chai.add(
    //    chaiscript::fun<QString (QJsonValue::*)() const>(&QJsonValue::toString),
    //    "toString");
    chai.add(chaiscript::fun([](QJsonValue *that) {
                 return that->toString();
             }),
             "toString");
    chai.add(chaiscript::fun([](QJsonValue *that) {
                 return that->toArray();
             }),
             "toArray");
    chai.add(chaiscript::fun([](QJsonValue *that) {
                 return that->toObject();
             }),
             "toObject");
    chai.add(chaiscript::fun([](QJsonArray *that, int i) {
                 return that->at(i);
             }),
             "at");
    chai.add(chaiscript::fun([](QJsonArray *that) {
                 std::vector<QJsonValue> out;
                 for (const auto val : *that)
                 {
                     out.push_back(val);
                 }
                 return out;
             }),
             "to_vector");
    chai.add(chaiscript::fun([](QJsonArray *that) {
                 return that->size();
             }),
             "size");

    chai.add(chaiscript::fun([](QJsonValueRef *that) {
                 return QJsonValue(*that);
             }),
             "toValue");
}

}  // namespace
namespace chatterino {

namespace plugin_interfaces {
    void Plugin::load()
    {
        this->vm.add_global(chaiscript::var<API *>(new API(this)), "api");
        this->vm.add(chaiscript::fun(&API::log), "log");
        this->vm.add(chaiscript::fun([](API *that, QString data) {
                         that->log(data.toStdString());
                     }),
                     "log");
        this->vm.add(chaiscript::fun(&API::getStringSetting),
                     "getStringSetting");
        this->vm.add(chaiscript::fun(&API::setStringSetting),
                     "setStringSetting");
        this->vm.add_global_const(chaiscript::const_var(Outcome(Failure)),
                                  "Failure");
        this->vm.add_global_const(chaiscript::const_var(Outcome(Success)),
                                  "Success");
        this->vm.add(chaiscript::fun([](QString *that) {
                         return that->toStdString();
                     }),
                     "to_string");

        addNetworkRequest(this->vm);
        addJSON(this->vm);
        this->vm.add(
            chaiscript::user_type<CompletionModel::TaggedString::Type>(),
            "TaggedStringType");
        {
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
        }

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
