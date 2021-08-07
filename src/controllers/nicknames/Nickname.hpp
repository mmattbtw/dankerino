#pragma once

#include "controllers/accounts/AccountController.hpp"

#include "util/RapidJsonSerializeQString.hpp"
#include "util/RapidjsonHelpers.hpp"

#include <QString>
#include <pajlada/serialize.hpp>

#include <memory>

namespace chatterino {

class Nickname
{
public:
    Nickname(const QString &name, const bool regex, const QString &replace)
        : name_(name)
        , regex_(regex)
        , replace_(replace)
    {
    }

    const QString &name() const
    {
        return this->name_;
    }
    const bool &regex() const
    {
        return this->regex_;
    }
    const QString &replace() const
    {
        return this->replace_;
    }

private:
    QString name_;
    bool regex_;
    QString replace_;
};

}  // namespace chatterino

namespace pajlada {

template <>
struct Serialize<chatterino::Nickname> {
    static rapidjson::Value get(const chatterino::Nickname &value,
                                rapidjson::Document::AllocatorType &a)
    {
        rapidjson::Value ret(rapidjson::kObjectType);

        chatterino::rj::set(ret, "name", value.name(), a);
        chatterino::rj::set(ret, "regex", value.regex(), a);
        chatterino::rj::set(ret, "replace", value.replace(), a);

        return ret;
    }
};

template <>
struct Deserialize<chatterino::Nickname> {
    static chatterino::Nickname get(const rapidjson::Value &value,
                                    bool *error = nullptr)
    {
        if (!value.IsObject())
        {
            PAJLADA_REPORT_ERROR(error)
            return chatterino::Nickname(QString(), false, QString());
        }

        QString _name;
        bool _regex;
        QString _replace;

        chatterino::rj::getSafe(value, "name", _name);
        chatterino::rj::getSafe(value, "regex", _regex);
        chatterino::rj::getSafe(value, "replace", _replace);

        return chatterino::Nickname(_name, _regex, _replace);
    }
};

}  // namespace pajlada
