#pragma once
#include <QString>

namespace chatterino {

class Channel;
struct Message;

namespace plugin_interfaces {
    class MessageCreatorPlugin
    {
    protected:
        std::function<MessagePtr(QString)> makeSystemMessage;
        std::function<void(Channel &chan,
                           std::shared_ptr<const Message> message)>
            addMessageToChannel;
        std::function<int(Channel &chan)> getChannelType;
        std::function<QString(Channel &chan)> getChannelName;

    public:
        virtual ~MessageCreatorPlugin(){};
        void setMessageCreationFunctionPointers(
            decltype(makeSystemMessage) _makeSystemMessage,
            decltype(addMessageToChannel) _addMessageToChannel,
            decltype(getChannelType) _getChannelType,
            decltype(getChannelName) _getChannelName)
        {
            this->makeSystemMessage = _makeSystemMessage;
            this->addMessageToChannel = _addMessageToChannel;
            this->getChannelType = _getChannelType;
            this->getChannelName = _getChannelName;
        }
    };
}  // namespace plugin_interfaces
}  // namespace chatterino

Q_DECLARE_INTERFACE(chatterino::plugin_interfaces::MessageCreatorPlugin,
                    "pl.kotmisia.dankerino.plugins.messagecreator/1.0");
