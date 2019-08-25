#include <QApplication>

#ifdef Q_OS_LINUX
QString run_command_calc(QStringList words);
std::string exec(const char* cmd);
char* escapeshellarg(const char *str);
#endif
