#ifndef STARENTITY_H
#define STARENTITY_H

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QTextureImage>
#include <Qt3DExtras/QTextureMaterial>

#include <QUrl>
#include <QColor>
#include <QHash>

#include "starobject.h"

class StarEntity : public Qt3DCore::QEntity
{
    Q_OBJECT

public:
    explicit StarEntity(const StarObject &data,
                        Qt3DCore::QNode *parent = nullptr);

    const StarObject& data() const;
    int id() const;
    Qt3DCore::QTransform* transform() const;

    void setViewLocked(bool locked);
    bool isViewLocked() const;

    void setDimmed(bool dimmed);

signals:
    void clicked(StarEntity *entity);

private:
    void setupGraphics();
    void setupInteraction();

    void createSphere();
    void createGlowIfLarge();

    void updateTextureDimState(bool dimmed);
    void updateColorDimState(bool dimmed);

    StarObject m_data;
    Qt3DCore::QTransform *m_transform = nullptr;
    Qt3DRender::QObjectPicker *m_picker = nullptr;

    Qt3DRender::QMaterial *m_material = nullptr;
    Qt3DExtras::QPhongMaterial *m_colorMaterial = nullptr;

    Qt3DExtras::QPhongMaterial *m_dimMaterial = nullptr;
    bool m_isDimmed = false;

    Qt3DExtras::QPhongMaterial* createDimMaterial();
    void setActiveMaterial(Qt3DRender::QMaterial *material);

    Qt3DExtras::QTextureMaterial *m_textureMaterial = nullptr;
    QColor m_originalColor;

    Qt3DRender::QMaterial* createMaterial();
    Qt3DRender::QTexture2D* textureForPath(const QString &path);
    bool hasTexture() const;

    static QHash<QString, Qt3DRender::QTexture2D*> s_textureCache;

    QColor dimmedColor() const;
    void applyMaterialColor(const QColor &color, const QColor &specular);

    bool m_viewLocked = false;
    float m_radius = 1.0f;

private:
    static constexpr float glowSizeThreshold = 150.0f;
    static constexpr float glowRadiusMultiplier = 2.2f;
    static constexpr float glowAlpha = 0.15f;
    static constexpr int glowMeshRings = 16;
    static constexpr int glowMeshSlices = 16;
};

#endif
