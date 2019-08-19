#include <QApplication>
#include <boost/algorithm/string/replace.hpp>
#include "command_mega.hpp"
#include <string>
#include <iostream>

QString command_mega(QStringList words)
{
    std::string words1 = words.join(" ").toStdString();
//    boost::replace_all(words1, " ", "_");
    std::string words2 = "";
    const std::string space = " ";

    bool is_commented = false;
    auto _temp = [&](char const &c)
    {
        if(c == '!')
        {
            is_commented = true;
            return;
        }

        if(!is_commented)
        {
            if (c == 'o' || c == 'O')
            {
                words2 += " OMEGALUL ";
            }else if (c == ' ')
            {
                words2 += '_';

            }else{
                words2 += c;
                words2 += space;
            }
        }else{
            words2 += c;
        }
        std::cout << "c: " << c << "\nwords2: " << words2 << "\n";
    };
    std::for_each(words1.begin(), words1.end(), _temp);
    if (words2[words2.length()-1] == ' ')
    {
        words2.pop_back();  // remove the ending space
    }
//    boost::replace_all(words2, "O", " OMEGALUL ");
//    boost::replace_all(words2, "o", " OMEGALUL ");
//    std::replace(words2.begin(), words2.end(), "O", "OMEGALUL");
//    std::replace(words2.begin(), words2.end(), "o", "OMEGALUL");
//    QString result = words2;
    return QString::fromStdString(words2);
}
