TEMPLATE      = lib
CONFIG       += plugin
QT           += widgets network concurrent
INCLUDEPATH  += ../../src
HEADERS       = supibot_completion.h
SOURCES       =
TARGET        = $$qtLibraryTarget(supibot_completion)
DESTDIR       = ../../built

git_commit=$$(GIT_COMMIT)
git_release=$$(GIT_RELEASE)

isEmpty(git_commit) {
git_commit=$$system(git rev-parse HEAD)
}
isEmpty(git_release) {
git_release=$$system(git describe)
}
git_hash = $$str_member($$git_commit, 0, 8)

DEFINES += CHATTERINO_GIT_HASH=\\\"$$git_hash\\\"
CONFIG += c++17

target.path = ./
INSTALLS += target

CONFIG += install_ok  # Do not cargo-cult this!
uikit: CONFIG += debug_and_release

include(lib/rapidjson.pri)
