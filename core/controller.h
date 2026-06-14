#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QString>
#include <QSqlDatabase>



class Controller
{
public:
    Controller();
    bool registreraAnvandare(QString namn, QString losen);
    int loggaIn(QString namn, QString losen);
private:
    QSqlDatabase db;
};



#endif // CONTROLLER_H
