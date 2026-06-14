#ifndef STARREPOSITORY_H
#define STARREPOSITORY_H

#include <QList>
#include <QtSql/QSqlQuery>
#include "../models/Star.h"
#include "../models/StarFilter.h"

class StarRepository {
public:
    StarRepository() = default;

    QList<Star> getAll();
    QList<Star> searchByName(const QString& name);
    QList<Star> getFiltered(const StarFilter &filter) const;


private:

    Star mapQueryToStar(const QSqlQuery& q) const;

};

#endif // STARREPOSITORY_H
