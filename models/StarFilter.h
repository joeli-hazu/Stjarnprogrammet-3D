#ifndef STARFILTER_H
#define STARFILTER_H

#include <QStringList>

struct NumericFilter {
    bool active = false;
    double min = 0.0;
    double max = 0.0;
};

struct StarFilter {

    NumericFilter mass;
    NumericFilter temperature;
    NumericFilter distance;
    NumericFilter diameter;

    QStringList selectedSpectralClasses;
    bool spectralActive = false;
};

#endif // STARFILTER_H