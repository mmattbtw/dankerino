#pragma once
#include <QString>

namespace chatterino {

class Channel;
struct Message;

namespace plugin_interfaces {
    class MessageCreatorPlugin
    {
    public:
        virtual ~MessageCreatorPlugin(){};
        void setMessageCreationFunctionPointers(
            const std::function<MessagePtr(const QString &)> _makeSystemMessage,
            const std::function<void(Channel &chan,
                                     std::shared_ptr<const Message> message)>
                _addMessageToChannel)
        {
            this->makeSystemMessage = _makeSystemMessage;
            this->addMessageToChannel = _addMessageToChannel;
        }

    protected:
        std::function<MessagePtr(QString)> makeSystemMessage;
        std::function<void(Channel &chan,
                           std::shared_ptr<const Message> message)>
            addMessageToChannel;
    };
}  // namespace plugin_interfaces
}  // namespace chatterino

Q_DECLARE_INTERFACE(chatterino::plugin_interfaces::MessageCreatorPlugin,
                    "pl.kotmisia.dankerino.plugins.messagecreator/1.0");
