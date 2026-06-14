#ifndef USER_H
#define USER_H

#include <QString>

struct User {
    int id = 0;
    QString username;
    QString password_hash;
    QString role;
    QString created_at;
};


#endif // USER_H
