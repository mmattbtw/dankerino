#include "LogsPopup.hpp"

#include "IrcMessage"
#include "common/Channel.hpp"
#include "common/NetworkRequest.hpp"
#include "messages/Message.hpp"
#include "providers/twitch/TwitchChannel.hpp"
#include "providers/twitch/TwitchIrcServer.hpp"
#include "providers/twitch/TwitchMessageBuilder.hpp"
#include "util/PostToThread.hpp"
#include "widgets/helper/ChannelView.hpp"

#include <QDateTime>
#include <QJsonArray>
#include <QMessageBox>
#include <QVBoxLayout>

namespace chatterino {

LogsPopup::LogsPopup(QWidget *parent)
    : SearchPopup(parent)
    , channel_(Channel::getEmpty())
{
    this->resize(400, 600);
}

void LogsPopup::setChannel(const ChannelPtr &channel)
{
    this->channel_ = channel;
    this->updateWindowTitle();
}

void LogsPopup::setChannelName(const QString &channelName)
{
    this->channelName_ = channelName;
    this->updateWindowTitle();
}

void LogsPopup::setTargetUserName(const QString &userName)
{
    this->userName_ = userName;
    this->updateWindowTitle();
}

void LogsPopup::updateWindowTitle()
{
    this->setWindowTitle(this->userName_ + "'s logs in #" + this->channelName_);
}

void LogsPopup::getLogs()
{
    if (this->channel_ && !this->channel_->isEmpty())
    {
        if (auto twitchChannel =
                dynamic_cast<TwitchChannel *>(this->channel_.get()))
        {
            this->channelName_ = twitchChannel->getName();
            this->getJustlogLogs();

            return;
        }
    }

    if (!this->channelName_.isEmpty())
    {
        this->getJustlogLogs();
        return;
    }

    qDebug() << "Unable to get logs, no channel name or something specified";
}

void LogsPopup::setMessages(std::vector<MessagePtr> &messages)
{
    ChannelPtr logsChannel(new Channel("logs", Channel::Type::Misc));

    logsChannel->addMessagesAtStart(messages);
    SearchPopup::setChannel(logsChannel);
}

/*
void LogsPopup::getOverrustleLogs()
{
    auto url =
        QString("https://overrustlelogs.net/api/v1/stalk/%1/%2.json?limit=500")
            .arg(this->channelName_, this->userName_);

    NetworkRequest(url)
        .caller(this)
        .onError([this](NetworkResult) {
            auto box = new QMessageBox(
                QMessageBox::Information, "Error getting logs",
                "No logs could be found for channel " + this->channelName_);
            box->setWindowFlag(Qt::WindowStaysOnTopHint);
            box->setAttribute(Qt::WA_DeleteOnClose);
            box->show();
            box->raise();
            this->close();
            box->exec();
        })
        .onSuccess([this](auto result) -> Outcome {
            auto data = result.parseJson();
            std::vector<MessagePtr> messages;
            if (data.contains("lines"))
            {
                QJsonArray dataMessages = data.value("lines").toArray();
                for (auto i : dataMessages)
                {
                    QJsonObject singleMessage = i.toObject();
                    auto text = singleMessage.value("text").toString();
                    QTime timeStamp =
                        QDateTime::fromSecsSinceEpoch(
                            singleMessage.value("timestamp").toInt())
                            .time();

                    MessageBuilder builder;
                    builder.emplace<TimestampElement>(timeStamp);
                    builder.emplace<TextElement>(this->userName_,
                                                 MessageElementFlag::Username,
                                                 MessageColor::System);
                    builder.emplace<TextElement>(text, MessageElementFlag::Text,
                                                 MessageColor::Text);
                    builder.message().messageText = text;
                    builder.message().displayName = this->userName_;
                    messages.push_back(builder.release());
                }
            }
            messages.push_back(
                MessageBuilder(systemMessage,
                               "Logs provided by https://overrustlelogs.net")
                    .release());
            this->setMessages(messages);

            return Success;
        })
        .execute();
}
*/
void LogsPopup::getJustlogLogs()
{
    QDate now = QDateTime::currentDateTime().date();
    auto url = QString("https://logs.ivr.fi/channel/%1/user/%2/%3/%4?json")
                   .arg(this->channelName_, this->userName_.toLower())
                   .arg(now.year())
                   .arg(now.month());
    qDebug() << url;

    NetworkRequest(url)
        .caller(this)
        .onError([this](NetworkResult) {
            auto box = new QMessageBox(
                QMessageBox::Information, "Error getting logs",
                "No logs could be found for channel " + this->channelName_);
            box->setWindowFlag(Qt::WindowStaysOnTopHint);
            box->setAttribute(Qt::WA_DeleteOnClose);
            box->show();
            box->raise();
            this->close();
            box->exec();
        })
        .onSuccess([this](auto result) -> Outcome {
            qDebug() << result.status();
            auto data = result.parseJson();
            auto hack = new IrcConnection;
            std::vector<MessagePtr> messages;
            MessageParseArgs args;
            args.disablePingSounds = true;

            if (data.contains("messages"))
            {
                QJsonArray dataMessages = data.value("messages").toArray();
                qDebug() << dataMessages.count();
                for (auto i : dataMessages)
                {
                    QJsonObject singleMessage = i.toObject();
                    auto text = singleMessage.value("text").toString();
                    QTime timeStamp =
                        QDateTime::fromString(
                            singleMessage.value("timestamp").toString(),
                            Qt::ISODate)
                            .time();

                    QColor usernameColor = MessageColor::System;
                    if (auto tags = singleMessage.value("tags").toObject();
                        !tags.isEmpty())
                    {
                        if (auto color = tags.value("color").toString();
                            !color.isNull() && !color.isEmpty())
                        {
                            usernameColor = QColor(color);
                        }
                    }
                    MessageBuilder builder;
                    builder.emplace<TimestampElement>(timeStamp);
                    builder.emplace<TextElement>(
                        singleMessage.value("username").toString(),
                        MessageElementFlag::Username, usernameColor);
                    builder.emplace<TextElement>(text, MessageElementFlag::Text,
                                                 MessageColor::Text);
                    builder.message().messageText = text;
                    builder.message().displayName = this->userName_;
                    messages.push_back(builder.release());
                }
            }
            else
            {
                messages.push_back(makeSystemMessage("No logs found"));
            }

            messages.push_back(
                makeSystemMessage("Logs sourced from logs.ivr.fi."));
            this->setMessages(messages);
            delete hack;

            return Success;
        })
        .execute();
}

}  // namespace chatterino
