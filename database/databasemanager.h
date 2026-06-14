#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql/QSqlDatabase>
#include <QString>

class DatabaseManager {
public:
    // Singleton — hela programmet delar EN anslutning
    static DatabaseManager& instance();

    bool connect(const QString& dbPath);
    void disconnect();
    bool isConnected() const;

private:
    DatabaseManager() = default;
    DatabaseManager(const DatabaseManager&) = delete;
    void operator=(const DatabaseManager&)  = delete;

    bool createTables();

    QSqlDatabase m_db;
    bool         m_connected = false;
};

#endif // DATABASEMANAGER_H