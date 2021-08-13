#pragma once

#include "common/NetworkRequest.hpp"
#include "messages/Link.hpp"
#include "providers/twitch/TwitchEmotes.hpp"

#include <boost/noncopyable.hpp>

#include <functional>

namespace chatterino {

using IvrFailureCallback = std::function<void()>;
template <typename... T>
using ResultCallback = std::function<void(T...)>;

struct IvrSubage {
    const bool isSubHidden;
    const bool isSubbed;
    const QString subTier;
    const int totalSubMonths;
    const QString followingSince;

    IvrSubage(QJsonObject root)
        : isSubHidden(root.value("hidden").toBool())
        , isSubbed(root.value("subscribed").toBool())
        , subTier(root.value("meta").toObject().value("tier").toString())
        , totalSubMonths(
              root.value("cumulative").toObject().value("months").toInt())
        , followingSince(root.value("followedAt").toString())
    {
    }
};

struct IvrEmoteSet {
    const QString setId;
    const QString displayName;
    const QString login;
    const QString channelId;
    const QString tier;
    const QJsonArray emotes;

    IvrEmoteSet(QJsonObject root)
        : setId(root.value("setID").toString())
        , displayName(root.value("channelName").toString())
        , login(root.value("channelLogin").toString())
        , channelId(root.value("channelID").toString())
        , tier(root.value("tier").toString())
        , emotes(root.value("emoteList").toArray())

    {
    }
};

struct IvrEmote {
    const QString code;
    const QString id;
    const QString setId;
    const QString url;
    const QString emoteType;
    const QString imageType;

    explicit IvrEmote(QJsonObject root)
        : code(root.value("code").toString())
        , id(root.value("id").toString())
        , setId(root.value("setID").toString())
        , url(QString(TWITCH_EMOTE_TEMPLATE)
                  .replace("{id}", this->id)
                  .replace("{scale}", "3.0"))
        , emoteType(root.value("type").toString())
        , imageType(root.value("assetType").toString())
    {
    }
};

struct IvrV2Emote {
    const QString channelName;
    const QString channelDisplayName;
    const QString channelId;
    const QString id;
    const QString code;
    const QString setId;
    const QString assetType;
    const QString type;
    const QString tier;

    IvrV2Emote(QJsonObject root)
        : channelName(root.value("channelLogin").toString())
        , channelDisplayName(root.value("channelName").toString())
        , channelId(root.value("channelID").toString())
        , id(root.value("emoteID").toString())
        , code(root.value("emoteCode").toString())
        , setId(root.value("emoteSetID").toString())
        , assetType(root.value("emoteAssetType").toString())
        , type(root.value("emoteType").toString())
        , tier(root.value("emoteTier").toString())
    {
    }
};

class IvrApi final : boost::noncopyable
{
public:
    // https://api.ivr.fi/docs#tag/Twitch/paths/~1twitch~1subage~1{username}~1{channel}/get
    void getSubage(QString userName, QString channelName,
                   ResultCallback<IvrSubage> resultCallback,
                   IvrFailureCallback failureCallback);

    // https://api.ivr.fi/v2/docs/static/index.html#/Twitch/get_twitch_emotes_sets
    void getBulkEmoteSets(QString emoteSetList,
                          ResultCallback<QJsonArray> successCallback,
                          IvrFailureCallback failureCallback,
                          std::function<void()> finallyCallback);

    // https://api.ivr.fi/v2/docs/static/index.html#/Twitch/get_twitch_emotes__emote_
    void getEmote(QString identifier, bool isId,
                  ResultCallback<IvrV2Emote> successCallback,
                  IvrFailureCallback failureCallback,
                  std::function<void()> finallyCallback);

    static void initialize();

private:
    NetworkRequest makeRequest(QString url, QUrlQuery urlQuery);
};

IvrApi *getIvr();

}  // namespace chatterino
