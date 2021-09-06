#include <QLoggingCategory>
#include <QObject>
#include <QtNetwork>

#include <plugin_interfaces/Completer.hpp>
#include <plugin_interfaces/MessageCreator.hpp>

using namespace chatterino;

namespace supibot_completion {

const QString VERSION = "1.0";

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

enum CompletionAdditionOption {
    NO = 0,
    YES_INCLUDE_OTHERS,
    YES_EXCLUDE_OTHERS
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
        timer->setSingleShot(true);
        timer->start(0);

        QNetworkRequest req(url);
        QNetworkReply *reply = this->netmanager_.get(req);

        QObject::connect(timer, &QTimer::timeout, this, [&reply]() {
            qCDebug(supibotCompletionPlugin) << "I HAVE NO SOURCES 1";
            reply->abort();
        });
        //QObject::connect(&this->netmanager_, &QNetworkAccessManager::finished,
        //                 reply, &QNetworkReply::deleteLater);
        QObject::connect(&this->netmanager_, &QNetworkAccessManager::finished, this, [reply]() {
            qCDebug(supibotCompletionPlugin) << "I HAVE NO SOURCES 2";
            reply->deleteLater();
        });
        QObject::connect(
            &this->netmanager_, &QNetworkAccessManager::finished,
            [this, timer, onSuccess, onError](QNetworkReply *reply) {
            qCDebug(supibotCompletionPlugin) << "I HAVE NO SOURCES 3";
                timer->deleteLater();
                if (reply->error())
                {
                    qCDebug(supibotCompletionPlugin)
                        << "error while fetching:" << reply->errorString();
                    onError(reply);
                }
                else
                {
                    QJsonDocument jsonDoc(
                        QJsonDocument::fromJson(reply->readAll()));
                    onSuccess(jsonDoc.object());
                }
                this->downloading_mutex_.unlock();
            });
    }
    QString command_fetch_err_;

    CompletionAdditionOption shouldAddNoPrefixCompletions(QString message)
    {
        if (!message.startsWith("$"))
        {
            return CompletionAdditionOption::NO;
        }
        if (message.startsWith("$ "))
        {
            if (message.count(' ') == 1)
            {
                return CompletionAdditionOption::
                    YES_EXCLUDE_OTHERS;  // handle completions just after a prefix with a space
            }
            message.replace("$ ", "$");  // account for prefix then space
        }

        if (message.startsWith("$pipe"))
        {
            return CompletionAdditionOption::YES_INCLUDE_OTHERS;
        }

        if (message.startsWith("$alias "))
        {
            // sub command hell
            message.replace("$alias ", "");
            if (message.startsWith("add ") || message.startsWith("addedit") ||
                message.startsWith("upsert ") || message.startsWith("edit "))
            {
                if (message.count(' ') == 2)
                {   // add xd COMMAND
                    // handle completions just after a prefix with a space
                    return CompletionAdditionOption::YES_EXCLUDE_OTHERS;
                }
                else
                {
                    return CompletionAdditionOption::YES_INCLUDE_OTHERS;
                }
            }
        }
        return CompletionAdditionOption::NO;
    }

public:
    void initialize() override
    {
        this->fetch(
            QUrl("https://supinic.com/api/bot/command/list"),
            [this](QJsonObject result) {
                SupibotCommandResponse res = result;
                qCDebug(supibotCompletionPlugin)
                    << "Fetched" << res.data.size() << "Supibot commands";
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
            [this](QNetworkReply *res) {
                this->command_fetch_err_ = res->errorString();
            });
    }

    bool refresh(std::function<void(const QString &str,
                                    CompletionModel::TaggedString::Type type)>
                     addString,
                 const QString &prefix, const QString &message,
                 bool isFirstWord, Channel &channel) override
    {
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
            CompletionAdditionOption opt;
            if (isFirstWord && message.startsWith("$"))
            {
                for (const QString completion : this->commands_)
                {
                    addString("$" + completion,
                              CompletionModel::TaggedString::Command);
                }
                return false;
            }
            else if ((opt = this->shouldAddNoPrefixCompletions(message)) !=
                     CompletionAdditionOption::NO)
            {
                // pipe and shit
                for (const QString completion : this->commands_)
                {
                    addString(completion,
                              CompletionModel::TaggedString::Command);
                }
                if (opt == CompletionAdditionOption::YES_EXCLUDE_OTHERS)
                {
                    return false;
                }
            }
        }
        return true;
    };

    // plugin
    QString name() const override
    {
        return "Supibot command completion v" + VERSION;
    };
};

}  // namespace supibot_completion
