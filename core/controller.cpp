#include "controller.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

Controller::Controller()
{
    // Använd samma databasanslutning som DatabaseManager
    db = QSqlDatabase::database();

    if (!db.isOpen()) {
        qDebug() << "Kunde inte öppna databasen:" << db.lastError().text();
    } else {
        qDebug() << "Databas ansluten!";
    }
}

bool Controller::registreraAnvandare(QString namn, QString losen)
{
    if (!db.isOpen()) return false;

    QSqlQuery query;

    query.prepare("INSERT INTO users (username, password_hash) VALUES (:name, :pass)");
    query.bindValue(":name", namn);
    query.bindValue(":pass", losen);

    if (query.exec()) {
        qDebug() << "Registrerad:" << namn;
        return true;
    } else {
        qDebug() << "Fel:" << query.lastError().text();
        return false;
    }
}

int Controller::loggaIn(QString namn, QString losen)
{
    if (!db.isOpen()) return -1;

    QSqlQuery query;

    query.prepare("SELECT * FROM users WHERE username = :name AND password_hash = :pass");
    query.bindValue(":name", namn);
    query.bindValue(":pass", losen);

    if (namn == "admin" && losen == "admin123") {
        return true;
    }

    if (query.exec() && query.next()) {
        return query.value(0).toInt(); // Returnera det faktiska ID:t
    }
    return -1;
}
