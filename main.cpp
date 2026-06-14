#include <QApplication>
#include <QDebug>
#include <qdir.h>
#include <QFileInfo>
#include <QStandardPaths>
#include "mainwindow.h"
#include "database/databasemanager.h"
#include "utils/CsvImporter.h"

QString findCsvPath()
{
    QDir dir(QCoreApplication::applicationDirPath());

    for (int i = 0; i < 6; ++i) {
        QString candidate = dir.filePath("data/stars.csv");
        if (QFileInfo::exists(candidate)) {
            return candidate;
        }
        dir.cdUp();
    }

    return "";
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    app.setQuitOnLastWindowClosed(false);

    // Steg 1 — öppna databasen, skapar stars.db + tabeller automatiskt
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);

    QString dbPath = dataDir + "/stars.db";
    qDebug() << "Databasen sparas här:" << dbPath;




    if (!DatabaseManager::instance().connect(dbPath)) {
        qDebug() << "KRITISKT FEL: Kunde inte starta databasen!";
        return 1;
    }

    // Steg 2 — importera CSV till stars-tabellen
    QString csvPath = findCsvPath();
    qDebug() << "CSV path:" << csvPath;

    if (csvPath.isEmpty()) {
        qDebug() << "VARNING: Kunde inte hitta data/StarSimbad.csv";
    } else {
        int imported = CsvImporter::importStars(csvPath);
        qDebug() << "Importerade" << imported << "nya stjärnor.";
    }

    // Steg 3 — starta huvudfonster
    MainWindow w;
    w.show();

    int result = app.exec();

    // Steg 4 — stäng databasen vid avslut
    DatabaseManager::instance().disconnect();
    return result;
}
