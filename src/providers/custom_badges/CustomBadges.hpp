#pragma once

#include <boost/optional.hpp>
#include <common/Singleton.hpp>

#include "common/Aliases.hpp"

#include <map>
#include <vector>

namespace chatterino {

struct Emote;
using EmotePtr = std::shared_ptr<const Emote>;

class CustomBadges : public Singleton
{
public:
    virtual void initialize(Settings &settings, Paths &paths) override;
    CustomBadges();

    boost::optional<EmotePtr> getBadge(const UserName &username);

private:
    void loadCustomBadges();
    std::map<QString, int> badgeMap;
    std::vector<EmotePtr> emotes;
};

}  // namespace chatterino
