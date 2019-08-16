#include "CustomBadges.hpp"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QThread>
#include "common/NetworkRequest.hpp"
#include "common/Outcome.hpp"
#include "messages/Emote.hpp"
#include "singletons/Settings.hpp"

#include <QUrl>

#include <map>

namespace chatterino {
void CustomBadges::initialize(Settings &settings, Paths &paths)
{
    this->loadCustomBadges();
}

CustomBadges::CustomBadges()
{
}

boost::optional<EmotePtr> CustomBadges::getBadge(const UserName &username)
{
    auto it = badgeMap.find(username.string);
    if (it != badgeMap.end())
    {
        return emotes[it->second];
    }
    return boost::none;
}

void CustomBadges::loadCustomBadges()
{
    //default is "http://www.kotmisia.pl/chatterino/badges.json"
    static QUrl url(getSettings()->customBadgesUrl);
    NetworkRequest req(url);
    req.setCaller(QThread::currentThread());

    req.onSuccess([this](auto result) -> Outcome {
        auto jsonRoot = result.parseJson();
        int index = 0;
        for (const auto &jsonBadge_ : jsonRoot.value("badges").toArray())
        {
            auto jsonBadge = jsonBadge_.toObject();
            auto emote = Emote{
                EmoteName{}, ImageSet{Url{jsonBadge.value("image").toString()}},
                Tooltip{jsonBadge.value("tooltip").toString()}, Url{}};

            emotes.push_back(std::make_shared<const Emote>(std::move(emote)));

            for (const auto &user : jsonBadge.value("users").toArray())
            {
                badgeMap[user.toString()] = index;
            }
            ++index;
        }

        return Success;
    });

    req.execute();
}

}  // namespace chatterino
