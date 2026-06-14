#ifndef STAROBJECT_H
#define STAROBJECT_H

#include <QString>
#include <QVector3D>
#include <QColor>
#include <QtGlobal>

#include <algorithm>
#include <cmath>

struct StarObject {
    int id = -1;
    QString name;
    QVector3D position;
    float size = 1.0f;
    QColor color;
    QString mass, distance, temperature, diameter, spectralClass, texturePath;
};

Q_DECLARE_METATYPE(StarObject)

namespace StarVisual
{
constexpr float MinRadius = 1.0f;
constexpr float MaxRadius = 6.0f;
constexpr float RadiusScale = 2.0f;

inline float radiusFromSize(float size)
{
    const float safeSize = std::max(0.0f, size);
    const float radius = MinRadius + std::log1p(safeSize) * RadiusScale;

    return qBound(MinRadius, radius, MaxRadius);
}
}

#endif // STAROBJECT_H
