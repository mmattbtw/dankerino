#pragma once

#include "widgets/helper/SearchPopup.hpp"

namespace chatterino {

class LogsPopup : public SearchPopup
{
public:
    LogsPopup(QWidget *parent);

    void setChannel(const ChannelPtr &channel) override;
    void setChannelName(const QString &channelName);
    void setTargetUserName(const QString &userName);

    void getLogs();

protected:
    void updateWindowTitle() override;

private:
    ChannelPtr channel_;

    QString userName_;
    QString channelName_;

    void setMessages(std::vector<MessagePtr> &messages);
    void getJustlogLogs();
    //void getOverrustleLogs();
    friend class AbstractIrcServer;
};

}  // namespace chatterino
