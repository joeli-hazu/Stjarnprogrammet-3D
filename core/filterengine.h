#ifndef FILTERENGINE_H
#define FILTERENGINE_H

#include <QString>
#include <QStringList>
#include "models/StarFilter.h"

class FilterEngine
{
public:
    static QString buildQuery(const StarFilter &filter);

private:
    static QString buildNumberFilter(const QString &column, double min, double max);
    static QString buildSpectralFilter(const QStringList &classes);
};

#endif // FILTERENGINE_H