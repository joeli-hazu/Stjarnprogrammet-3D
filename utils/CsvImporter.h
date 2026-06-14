#ifndef CSVIMPORTER_H
#define CSVIMPORTER_H

#include <QString>

class CsvImporter {
public:
    // Returnerar antal importerade stjärnor, -1 vid fel
    static int importStars(const QString& csvFilePath);

private:
    static bool starExists(const QString& mainId);
};

#endif // CSVIMPORTER_H
