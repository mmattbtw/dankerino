#pragma once

#include "common/CompletionModel.hpp"

namespace chatterino {

namespace plugin_interfaces {

    class CompleterPlugin
    {
    public:
        virtual ~CompleterPlugin(){};

        /**
         * @brief refresh Called when the user presses Tab for the first time
         * @param addString a function which adds items into the items of the CompletionModel, you do not need to care about Settings::prefixOnlyEmoteCompletion
         * @param prefix The partial word the user typed out
         * @param isFirstWord Is this the first word in the input box?
         * @param channel Current channel
         * @return Should the completion model add its completions
         */
        virtual bool refresh(
            std::function<void(const QString &str,
                               CompletionModel::TaggedString::Type type)>
                addString,
            const QString &prefix, bool isFirstWord,
            const Channel &channel) = 0;
    };

}  // namespace plugin_interfaces
}  // namespace chatterino

Q_DECLARE_INTERFACE(chatterino::plugin_interfaces::CompleterPlugin,
                    "pl.kotmisia.dankerino.plugins.completer/1.0");
