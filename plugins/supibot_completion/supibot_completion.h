#include <QLoggingCategory>
#include <QObject>
#include <QtNetwork>

#include <plugin_interfaces/Completer.hpp>
#include <plugin_interfaces/MessageCreator.hpp>

using namespace chatterino;

namespace supibot_completion {

Q_DECLARE_LOGGING_CATEGORY(supibotCompletionPlugin);
Q_LOGGING_CATEGORY(supibotCompletionPlugin, "plugin.supibotCompletion",
                   QtDebugMsg);

const QString completionsUnavailable =
    "Completions for Supibot commands are unavailable";

struct SupibotCommand {
    const int id;
    const QString name;
    const QJsonArray aliases;
    const QString description;
    const int cooldown;

    SupibotCommand(QJsonObject root)
        : id(root.value("ID").toInt())
        , name(root.value("name").toString())
        , aliases(root.value("aliases").toArray())
        , description(root.value("description").toString())
        , cooldown(root.value("cooldown").toInt())
    {
    }
};

struct SupibotCommandResponse {
    const QJsonArray data;

    SupibotCommandResponse(QJsonObject root)
        : data(root.value("data").toArray())
    {
    }
};

class SupibotCompletionPlugin : public QObject,
                                public plugin_interfaces::CompleterPlugin,
                                public plugin_interfaces::Plugin,
                                public plugin_interfaces::MessageCreatorPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "pl.kotmisia.dankerino.plugins.completer/1.0" FILE
                          "metadata.json")
    Q_INTERFACES(chatterino::plugin_interfaces::CompleterPlugin)
private:
    std::vector<QString> commands_;
    std::mutex downloading_mutex_;
    QNetworkAccessManager netmanager_;
    QDateTime last_shown_warning_;

    void fetch(QUrl url, std::function<void(QJsonObject)> onSuccess,
               std::function<void(QNetworkReply *)> onError)
    {
        this->downloading_mutex_.lock();
        QTimer *timer = new QTimer(this);
        timer->start(5000);
        timer->setSingleShot(true);

        QNetworkRequest req(url);
        QNetworkReply *reply = this->netmanager_.get(req);

        QObject::connect(timer, &QTimer::timeout, this, [&reply]() {
            reply->abort();
        });
        QObject::connect(&this->netmanager_, &QNetworkAccessManager::finished,
                         reply, &QNetworkReply::deleteLater);
        QObject::connect(
            &this->netmanager_, &QNetworkAccessManager::finished,
            [this, timer, onSuccess, onError](QNetworkReply *reply) {
                timer->deleteLater();
                if (reply->error())
                {
                    qCDebug(supibotCompletionPlugin)
                        << "error while fetching:" << reply->errorString();
                    onError(reply);
                }
                else
                {
                    qCDebug(supibotCompletionPlugin) << "downloaded?";
                    QJsonDocument jsonDoc(
                        QJsonDocument::fromJson(reply->readAll()));
                    onSuccess(jsonDoc.object());
                }
                this->downloading_mutex_.unlock();
            });
    }
    QString command_fetch_err_;

public:
    void initialize() override
    {
        qCDebug(supibotCompletionPlugin) << "fetching supibot commands...";
        this->fetch(
            QUrl("https://supinic.com/api/bot/command/list"),
            [this](QJsonObject root) {
                SupibotCommandResponse res = root;
                for (const auto obj : res.data)
                {
                    SupibotCommand cmd = obj.toObject();

                    this->commands_.push_back(cmd.name);
                    for (const QJsonValue alias : cmd.aliases)
                    {
                        this->commands_.push_back(alias.toString());
                    }
                }
            },
            [this](QNetworkReply *reply) {
                this->command_fetch_err_ = reply->errorString();
            });
    }

    bool refresh(std::function<void(const QString &str,
                                    CompletionModel::TaggedString::Type type)>
                     addString,
                 const QString &prefix, bool isFirstWord,
                 Channel &channel) override
    {
        qCDebug(supibotCompletionPlugin) << "Completions?";
        if (this->commands_.empty())
        {
            auto now = QDateTime::currentDateTime();
            if (this->last_shown_warning_.addSecs(30) < now)
            {
                if (this->command_fetch_err_.isEmpty())
                {
                    this->addMessageToChannel(
                        channel, this->makeSystemMessage(
                                     completionsUnavailable + " yet"));
                }
                else
                {
                    this->addMessageToChannel(
                        channel,
                        this->makeSystemMessage(completionsUnavailable + ": " +
                                                this->command_fetch_err_));
                }
                this->last_shown_warning_ = now;
            }
        }
        else
        {
            if (isFirstWord)
            {
                for (const QString completion : this->commands_)
                {
                    addString("$" + completion,
                              CompletionModel::TaggedString::Command);
                }
            }
            else
            {
                // pipe and shit
                for (const QString completion : this->commands_)
                {
                    addString(completion,
                              CompletionModel::TaggedString::Command);
                }
            }
        }
        return false;
    };

    // plugin
    QString name() const override
    {
        return "Supibot command completion";
    };
};

}  // namespace supibot_completion