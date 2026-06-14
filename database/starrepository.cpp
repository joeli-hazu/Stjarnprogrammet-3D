#include "starrepository.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include "filterengine.h"


Star StarRepository::mapQueryToStar(const QSqlQuery& q) const {
    Star s;
    s.id             = q.value("id").toInt();
    s.main_id        = q.value("main_id").toString();
    s.x              = q.value("x").toDouble();
    s.y              = q.value("y").toDouble();
    s.z              = q.value("z").toDouble();
    s.spectral_type  = q.value("spectral_type").toString();
    s.diameter_solar = q.value("diameter_solar").toDouble();
    s.distance_ly    = q.value("distance_ly").toDouble();
    s.temperature    = q.value("temperature").toDouble();
    s.mass           = q.value("mass").toDouble();
    return s;

}

QList<Star> StarRepository::getAll() {
    QList<Star> stars;
    QSqlQuery q;

    if (!q.exec("SELECT id, main_id, x, y, z, "
                "spectral_type, diameter_solar, "
                "distance_ly, temperature, mass "
                "FROM stars")) {
        qDebug() << "getAll fel:" << q.lastError().text();
        return stars;
    }

    while (q.next()) {
        Star s;
        s.id             = q.value("id").toInt();
        s.main_id        = q.value("main_id").toString();
        s.x              = q.value("x").toDouble();
        s.y              = q.value("y").toDouble();
        s.z              = q.value("z").toDouble();
        s.spectral_type  = q.value("spectral_type").toString();
        s.diameter_solar = q.value("diameter_solar").toDouble();
        s.distance_ly    = q.value("distance_ly").toDouble();
        s.temperature    = q.value("temperature").toDouble();
        s.mass           = q.value("mass").toDouble();
        stars.append(s);
    }

    qDebug() << "Hamtade" << stars.size() << "stjarnor fran databasen.";
    return stars;
}

QList<Star> StarRepository::searchByName(const QString& name) {
    QList<Star> stars;
    QSqlQuery q;

    q.prepare("SELECT id, main_id, x, y, z, "
              "spectral_type, diameter_solar, "
              "distance_ly, temperature, mass "
              "FROM stars WHERE main_id LIKE :name");
    q.bindValue(":name", "%" + name + "%");

    if (!q.exec()) {
        qDebug() << "searchByName fel:" << q.lastError().text();
        return stars;
    }

    while (q.next()) {
        Star s;
        s.id             = q.value("id").toInt();
        s.main_id        = q.value("main_id").toString();
        s.x              = q.value("x").toDouble();
        s.y              = q.value("y").toDouble();
        s.z              = q.value("z").toDouble();
        s.spectral_type  = q.value("spectral_type").toString();
        s.diameter_solar = q.value("diameter_solar").toDouble();
        s.distance_ly    = q.value("distance_ly").toDouble();
        s.temperature    = q.value("temperature").toDouble();
        s.mass           = q.value("mass").toDouble();
        stars.append(s);
    }

    return stars;
}




QList<Star> StarRepository::getFiltered(const StarFilter &filter) const
{
    QList<Star> results;

    QString queryString = FilterEngine::buildQuery(filter);

    QSqlQuery query;

    if (query.exec(queryString)) {
        while (query.next()) {
            results.append(mapQueryToStar(query));
        }
    } else {
        qDebug() << "SQL Fel:" << query.lastError().text();
        qDebug() << "Query:" << queryString;
    }

    return results;
}
