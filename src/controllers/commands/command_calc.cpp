#include "command_calc.hpp"
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <QApplication>
#include <QFile>
#include <QRegularExpression>
#include <stdlib.h>    // malloc
char* escapeshellarg(const char* str) {
    char *escStr;
    int i,
        count = strlen(str),
            ptr_size = count+3;

    escStr = (char *) calloc(ptr_size, sizeof(char));
    if (escStr == NULL) {
        return NULL;
    }
    sprintf(escStr, "'");

    for(i=0; i<count; i++) {
        if (str[i] == '\'') {
                    ptr_size += 3;
            escStr = (char *) realloc(escStr,ptr_size * sizeof(char));
            if (escStr == NULL) {
                return NULL;
            }
            sprintf(escStr, "%s'\\''", escStr);
        } else {
            sprintf(escStr, "%s%c", escStr, str[i]);
        }
    }

    sprintf(escStr, "%s%c", escStr, '\'');
    return escStr;
}

std::string exec(const char *cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

QString run_command_calc(QStringList words)
{
    QString cmd = QString("/bin/sh -c \"echo ")
            + QString(escapeshellarg(words.join(" ").toStdString().c_str()))
            + QString(" | bc\"");
    QString result = exec(cmd.toStdString().c_str()).c_str();
    return result;
}
