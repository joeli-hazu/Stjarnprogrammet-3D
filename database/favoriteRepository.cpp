#include "favoriteRepository.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>

bool FavoriteRepository::addFavorite(int userId, int starId) {
    QSqlQuery q;
    q.prepare("INSERT OR IGNORE INTO favorites (user_id, star_id) VALUES (:u, :s)");
    q.bindValue(":u", userId);
    q.bindValue(":s", starId);
    return q.exec();
}

bool FavoriteRepository::removeFavorite(int userId, int starId) {
    QSqlQuery q;
    q.prepare("DELETE FROM favorites WHERE user_id = :u AND star_id = :s");
    q.bindValue(":u", userId);
    q.bindValue(":s", starId);
    return q.exec();
}

QList<int> FavoriteRepository::getFavorites(int userId) {
    QList<int> list;
    QSqlQuery q;
    q.prepare("SELECT star_id FROM favorites WHERE user_id = :u");
    q.bindValue(":u", userId);
    q.exec();

    while (q.next())
        list.append(q.value(0).toInt());

    return list;
}

bool FavoriteRepository::isFavorite(int userId, int starId) {
    QSqlQuery q;
    q.prepare("SELECT 1 FROM favorites WHERE user_id = :u AND star_id = :s");
    q.bindValue(":u", userId);
    q.bindValue(":s", starId);
    q.exec();
    return q.next();
}
