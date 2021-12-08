#include "DankerinoPage.hpp"

#include <QFontDialog>
#include <QLabel>
#include <QScrollArea>

#include "Application.hpp"
#include "common/Version.hpp"
#include "plugin_interfaces/SettingsPlugin.hpp"
#include "singletons/Plugins.hpp"
#include "widgets/BaseWindow.hpp"
#include "widgets/helper/Line.hpp"
#include "widgets/settingspages/GeneralPageView.hpp"

#include <QDesktopServices>
#include <QFileDialog>
#include <QGroupBox>

namespace chatterino {

DankerinoPage::DankerinoPage()
{
    auto y = new QVBoxLayout;
    auto x = new QHBoxLayout;
    auto view = new GeneralPageView;
    this->view_ = view;
    x->addWidget(view);
    auto z = new QFrame;
    z->setLayout(x);
    y->addWidget(z);
    this->setLayout(y);

    this->initLayout(*view);
}

bool DankerinoPage::filterElements(const QString &query)
{
    if (this->view_)
        return this->view_->filterElements(query) || query.isEmpty();
    else
        return false;
}

void DankerinoPage::initLayout(GeneralPageView &layout)
{
    auto &s = *getSettings();

    layout.addTitle("Apperance");
    layout.addCheckbox("Show placeholder in text input box (requires restart)",
                       s.showTextInputPlaceholder);
    layout.addCheckbox("Colorize usernames on IRC", s.colorizeNicknamesOnIrc);
    layout.addCheckbox("Gray-out recent messages", s.grayOutRecents);
    layout.addTitle("Behavior");
    layout.addCheckbox("Lowercase tab-completed usernames",
                       s.lowercaseUsernames);
    layout.addTitle("Emotes");
    layout.addCheckbox("Enable loading 7TV emotes", s.enableLoadingSevenTV);
    layout.addTitle("Miscellaneous");
    layout.addIntInput("High rate limit spam delay in milliseconds (mod/vip)",
                       s.twitchHighRateLimitDelay, 100, 2000, 100);
    layout.addIntInput(
        "Low rate limit spam delay in milliseconds (non mod/vip)",
        s.twitchLowRateLimitDelay, 500, 3000, 1100);
    layout.addCheckbox(
        "Enable EXPERIMENTAL plugin support (requires restart).\nYour "
        "chatterino might crash unexpectedly if you have plugins!\nPlugins "
        "can load arbitrary code into Chatterino,\nbe sure you trust "
        "their authors!",
        s.enablePlugins);
    if (s.enablePlugins)
    {
        auto pluginsDisplay = new QGroupBox("Plugins loaded:");
        layout.addWidget(pluginsDisplay);
        {
            auto list = new QVBoxLayout(pluginsDisplay);

            getApp()->plugins->forEachPlugin(
                [&list](plugin_interfaces::Plugin *plugin) {
                    auto settings =
                        dynamic_cast<plugin_interfaces::SettingsPlugin *>(
                            plugin);
                    if (settings)
                    {
                        auto label = new QLabel(plugin->name() +
                                                " <a href=.>Open settings</a>");
                        list->addWidget(label);
                        QObject::connect(label, &QLabel::linkActivated,
                                         [pl = settings]() {
                                             pl->openSettings();
                                         });
                    }
                    else
                    {
                        auto label = new QLabel(plugin->name());
                        list->addWidget(label);
                    }
                });
        }
    }

    if (s.dankerinoThreeLetterApiEasterEgg)
    {
        layout.addTitle("Advanced");
        layout.addCheckbox("Click to disable GraphQL easter egg and "
                           "advanced settings "
                           "(requires restart)",
                           s.dankerinoThreeLetterApiEasterEgg);
        layout.addTitle("Random 'hacks'");
        layout.addCheckbox("Enable. Required for settings below to work!",
                           s.nonceFuckeryEnabled);
        layout.addCheckbox("Abnormal nonce detection",
                           s.abnormalNonceDetection);

        layout.addCheckbox(
            "Webchat detection highlights. Highlights messages sent from "
            "webchat in orange or the specified color below.",
            s.normalNonceDetection);

        layout.addColorButton("Webchat detected color",
                              QColor(getSettings()->webchatColor.getValue()),
                              getSettings()->webchatColor);
    }
    layout.addStretch();
    // invisible element for width
    auto inv = new BaseWidget(this);
    //    inv->setScaleIndependantWidth(600);
    layout.addWidget(inv);
}

}  // namespace chatterino
