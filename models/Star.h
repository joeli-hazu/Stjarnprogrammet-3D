#ifndef STAR_H
#define STAR_H


#include <QString>

struct Star {
    int     id             = 0;
    QString main_id;            // stjärnans namn, t.ex. "Sirius"
    double  ra             = 0.0;  // rektascension (vinkelposition)
    double  dec            = 0.0;  // deklination (vinkelposition)
    double  distance_ly    = 0.0;  // avstånd i ljusår
    double  x              = 0.0;  // 3D-position
    double  y              = 0.0;
    double  z              = 0.0;
    QString spectral_type;         // t.ex. "M8.5V", "K8Vk:"
    double  diameter_solar = 0.0;  // diameter i soldiametrar
    double  diameter_km    = 0.0;  // diameter i km
    double  temperature    = 0.0;  // yttemperatur i Kelvin
    double  mass           = 0.0;  // massa i solmassor
};

#endif

