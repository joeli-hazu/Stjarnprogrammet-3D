#include "filterengine.h"
#include "models/StarFilter.h"
#include <QLocale>

QString FilterEngine::buildQuery(const StarFilter &filter)
{
    // Vi listar kolumnerna explicit för att säkerställa att mapQueryToStar får all data den behöver
    QString query = "SELECT id, main_id, x, y, z, spectral_type, "
                    "diameter_solar, distance_ly, temperature, mass "
                    "FROM stars WHERE 1=1";

    QLocale c(QLocale::C);

    // ---------------- MASS ----------------
    if (filter.mass.active) {
        // Genom att skicka båda värdena i samma .arg() slipper vi varningen
        query += QString(" AND mass BETWEEN %1 AND %2")
                     .arg(c.toString(filter.mass.min, 'f', 4),
                          c.toString(filter.mass.max, 'f', 4));
    }

    // ---------------- TEMPERATURE ----------------
    if (filter.temperature.active) {
        query += QString(" AND temperature BETWEEN %1 AND %2")
        .arg(c.toString(filter.temperature.min, 'f', 2),
             c.toString(filter.temperature.max, 'f', 2));
    }

    // ---------------- DISTANCE ----------------
    if (filter.distance.active) {
        query += QString(" AND distance_ly BETWEEN %1 AND %2")
        .arg(c.toString(filter.distance.min, 'f', 4),
             c.toString(filter.distance.max, 'f', 4));
    }

    // ---------------- DIAMETER ----------------
    if (filter.diameter.active) {
        query += QString(" AND diameter_solar BETWEEN %1 AND %2")
        .arg(c.toString(filter.diameter.min, 'f', 4),
             c.toString(filter.diameter.max, 'f', 4));
    }

    // ---------------- SPECTRAL ----------------
    if (filter.spectralActive) {
        query += buildSpectralFilter(filter.selectedSpectralClasses);
    }

    return query;
}

QString FilterEngine::buildSpectralFilter(const QStringList &classes)
{
    // Krav: "Om jag väljer inga -> visa inga stjärnor"
    if (classes.isEmpty()) {
        return " AND 1=0"; // Gör hela queryn tom (False)
    }

    QStringList conditions;
    for (const QString &c : classes) {
        // OR-logik inom spektralklasser (G eller K eller M...)
        conditions << QString("spectral_type LIKE '%1%%'").arg(c);
    }

    return " AND (" + conditions.join(" OR ") + " OR spectral_type IS NULL OR spectral_type = '' OR (spectral_type NOT LIKE 'O%' AND spectral_type NOT LIKE 'B%' AND spectral_type NOT LIKE 'A%' AND spectral_type NOT LIKE 'F%' AND spectral_type NOT LIKE 'G%' AND spectral_type NOT LIKE 'K%' AND spectral_type NOT LIKE 'M%'))";
}