#ifndef STARCOLORHELPER_H
#define STARCOLORHELPER_H

#include <QColor>
#include <QString>

inline QColor spectralColor(const QString& type) {
    if (type.isEmpty()) return QColor(255, 255, 255);
    QChar cls = type[0].toUpper();

    if (cls == 'O') return QColor(155, 176, 255);
    if (cls == 'B') return QColor(170, 191, 255);
    if (cls == 'A') return QColor(202, 215, 255);
    if (cls == 'F') return QColor(248, 247, 255);
    if (cls == 'G') return QColor(255, 244, 234);
    if (cls == 'K') return QColor(255, 210, 161);
    if (cls == 'M') return QColor(255, 150, 100);
    if (cls == 'T') return QColor(100, 100, 200);
    return QColor(255, 255, 255);
}

#endif // STARCOLORHELPER_H
