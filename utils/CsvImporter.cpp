#include "CsvImporter.h"
#include <QFile>
#include <QTextStream>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>

// Hjälpfunktion som tar bort citattecken runt värden
static QString stripQuotes(const QString& val) {
    QString result = val.trimmed();
    if (result.startsWith('"') && result.endsWith('"')) {
        result = result.mid(1, result.length() - 2);
    }
    return result.trimmed();
}

int CsvImporter::importStars(const QString& csvFilePath) {
    QFile file(csvFilePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Kunde inte öppna CSV:" << csvFilePath;
        return -1;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    // Hoppa over rubrikraden
    if (!in.atEnd()) in.readLine();

    QSqlDatabase::database().transaction();

    int count = 0;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList cols = line.split(",");
        if (cols.size() < 13) {
            cols = line.split(";");
        }


        if (cols.size() < 13) {
            qDebug() << "Felaktigt radformat (hoppar över):" << line;
            continue;
        }



        QString main_id        = stripQuotes(cols[1]);
        double  ra             = stripQuotes(cols[2]).toDouble();
        double  dec            = stripQuotes(cols[3]).toDouble();
        double  distance_ly    = stripQuotes(cols[4]).toDouble();
        double  x              = stripQuotes(cols[5]).toDouble();
        double  y              = stripQuotes(cols[6]).toDouble();
        double  z              = stripQuotes(cols[7]).toDouble();
        QString spectral_type  = stripQuotes(cols[8]);
        double  diameter_solar = stripQuotes(cols[9]).toDouble();
        double  diameter_km    = stripQuotes(cols[10]).toDouble();
        double  temperature    = stripQuotes(cols[11]).toDouble();
        double  mass           = stripQuotes(cols[12]).toDouble();

        // Hoppa over om stjarnan redan finns
        if (starExists(main_id)) continue;

        QSqlQuery q;
        q.prepare(
            "INSERT INTO stars "
            "(main_id, ra, dec, distance_ly, x, y, z, "
            " spectral_type, diameter_solar, diameter_km, "
            " temperature, mass) "
            "VALUES "
            "(:main_id, :ra, :dec, :distance_ly, :x, :y, :z, "
            " :spectral_type, :diameter_solar, :diameter_km, "
            " :temperature, :mass)"
            );

        q.bindValue(":main_id",        main_id);
        q.bindValue(":ra",             ra);
        q.bindValue(":dec",            dec);
        q.bindValue(":distance_ly",    distance_ly);
        q.bindValue(":x",              x);
        q.bindValue(":y",              y);
        q.bindValue(":z",              z);
        q.bindValue(":spectral_type",  spectral_type);
        q.bindValue(":diameter_solar", diameter_solar);
        q.bindValue(":diameter_km",    diameter_km);
        q.bindValue(":temperature",    temperature);
        q.bindValue(":mass",           mass);

        if (!q.exec()) {
            qDebug() << "Fel vid insert av"
                     << main_id << ":" << q.lastError().text();
            continue;
        }
        count++;
    }

    if (QSqlDatabase::database().commit()) {
        qDebug() << "Transaktion lyckades och sparades till databasen.";
    } else {
        qDebug() << "Transaktion misslyckades:" << QSqlDatabase::database().lastError().text();
    }

    file.close();
    qDebug() << "Importerade" << count << "stjärnor.";
    return count;
}

bool CsvImporter::starExists(const QString& mainId) {
    QSqlQuery q;
    q.prepare("SELECT id FROM stars WHERE main_id = :main_id LIMIT 1");
    q.bindValue(":main_id", mainId);
    q.exec();
    return q.next();
}
