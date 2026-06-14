#include "databasemanager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager inst;
    return inst;
}

bool DatabaseManager::connect(const QString& dbPath) {

    if (m_db.isOpen()) {
        m_db.close();
    }
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);

    if (!m_db.open()) {
        QSqlQuery q;
        q.exec("PRAGMA foreign_keys = ON;");
        qDebug() << "Fel: kunde inte öppna databasen:"
                 << m_db.lastError().text();
        return false;
    }


    QSqlQuery q;
    q.exec("PRAGMA foreign_keys = ON;");


    m_connected = true;
    qDebug() << "Databas öppnad:" << dbPath;

    return createTables();
}

void DatabaseManager::disconnect() {
    if (m_connected) {
        m_db.close();
        m_connected = false;
        qDebug() << "Databas stängd.";
    }
}

bool DatabaseManager::isConnected() const {
    return m_connected;
}

bool DatabaseManager::createTables() {
    QSqlQuery q;

    // STARS
    if (!q.exec(
            "CREATE TABLE IF NOT EXISTS stars ("
            "  id             INTEGER PRIMARY KEY AUTOINCREMENT,"
            "  main_id        TEXT    NOT NULL UNIQUE,"
            "  ra             REAL    DEFAULT 0,"
            "  dec            REAL    DEFAULT 0,"
            "  distance_ly    REAL    DEFAULT 0,"
            "  x              REAL    DEFAULT 0,"
            "  y              REAL    DEFAULT 0,"
            "  z              REAL    DEFAULT 0,"
            "  spectral_type  TEXT    DEFAULT '',"
            "  diameter_solar REAL    DEFAULT 0,"
            "  diameter_km    REAL    DEFAULT 0,"
            "  temperature    REAL    DEFAULT 0,"
            "  mass           REAL    DEFAULT 0"
            ")"
            )) {
        qDebug() << "stars-fel:" << q.lastError().text();
        return false;
    }


    q.exec("CREATE INDEX IF NOT EXISTS idx_stars_filter ON stars(distance_ly, temperature, mass, spectral_type);");
    // USERS
    if (!q.exec(
            "CREATE TABLE IF NOT EXISTS users ("
            "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "  username TEXT UNIQUE,"
            "  password_hash TEXT"
            ")"
            )) {
        qDebug() << "users-fel:" << q.lastError().text();
        return false;
    }


    // FAVORITES
    if (!q.exec(
            "CREATE TABLE IF NOT EXISTS favorites ("
            "  id       INTEGER PRIMARY KEY AUTOINCREMENT,"
            "  user_id  INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,"
            "  star_id  INTEGER NOT NULL REFERENCES stars(id) ON DELETE CASCADE,"
            "  added_at TEXT    DEFAULT (datetime('now')),"
            "  UNIQUE(user_id, star_id)"
            ")"
            )) {
        qDebug() << "favorites-fel:" << q.lastError().text();
        return false;
    }



    qDebug() << "Alla tabeller skapade OK";
    return true;
}
