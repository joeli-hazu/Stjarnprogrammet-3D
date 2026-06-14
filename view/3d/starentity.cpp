#include "starentity.h"
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QPhongAlphaMaterial>
#include <Qt3DRender/QPickEvent>
#include <QMatrix4x4>
#include <QUrl>

QHash<QString, Qt3DRender::QTexture2D*> StarEntity::s_textureCache;

// StarEntity
//    - ritar en stjärna
//    - håller StarObject-data
//    - skickar signal när den klickas
//    - kan aktiveras/deaktiveras

StarEntity::StarEntity(const StarObject &data,
                       Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent),
    m_data(data)
{
    setObjectName("star");
    setupGraphics();
    setupInteraction();
}

const StarObject& StarEntity::data() const
{
    return m_data;
}

int StarEntity::id() const
{
    return m_data.id;
}

Qt3DCore::QTransform* StarEntity::transform() const
{
    return m_transform;
}

void StarEntity::setViewLocked(bool locked)
{
    m_viewLocked = locked;
}

bool StarEntity::isViewLocked() const
{
    return m_viewLocked;
}


void StarEntity::setupGraphics() {

    m_radius = StarVisual::radiusFromSize(m_data.size);

    createSphere();
    //createGlowIfLarge();

}

void StarEntity::createSphere() {
    auto *mesh = new Qt3DExtras::QSphereMesh(this);
    mesh->setRadius(m_radius);
    mesh->setRings(32);
    mesh->setSlices(32);

    m_material = createMaterial();

    m_transform = new Qt3DCore::QTransform(this);
    m_transform->setTranslation(m_data.position);

    addComponent(mesh);
    addComponent(m_material);
    addComponent(m_transform);
}

void StarEntity::createGlowIfLarge() {
    if (m_data.size > glowSizeThreshold) {
        auto *glow = new Qt3DCore::QEntity(this);

        auto *glowMesh = new Qt3DExtras::QSphereMesh(glow);
        glowMesh->setRadius(m_radius * glowRadiusMultiplier);
        glowMesh->setRings(glowMeshRings);
        glowMesh->setSlices(glowMeshSlices);

        auto *glowMaterial = new Qt3DExtras::QPhongAlphaMaterial(glow);
        glowMaterial->setAmbient(m_originalColor);
        glowMaterial->setDiffuse(m_originalColor);
        glowMaterial->setAlpha(glowAlpha);

        glow->addComponent(glowMesh);
        glow->addComponent(glowMaterial);
    }
}

void StarEntity::setupInteraction() {
    m_picker = new Qt3DRender::QObjectPicker(this);
    m_picker->setDragEnabled(false);
    m_picker->setHoverEnabled(true);
    addComponent(m_picker);

    connect(m_picker, &Qt3DRender::QObjectPicker::clicked,
            this, [this]() {
        emit clicked(this);
    });
}
void StarEntity::setDimmed(bool dimmed) {
    if (m_isDimmed == dimmed) return;

    m_isDimmed = dimmed;

    if (m_textureMaterial) {
        updateTextureDimState(dimmed);
        return;
    }

    updateColorDimState(dimmed);
}

void StarEntity::updateTextureDimState(bool dimmed)
{
    if (dimmed) {
        setActiveMaterial(createDimMaterial());
    } else {
        setActiveMaterial(m_textureMaterial);
    }
}

void StarEntity::updateColorDimState(bool dimmed)
{
    if (!m_colorMaterial) {
        return;
    }

    if (dimmed) {
        applyMaterialColor(dimmedColor(), Qt::black);
    } else {
        applyMaterialColor(m_originalColor, Qt::white);
    }
}

Qt3DExtras::QPhongMaterial* StarEntity::createDimMaterial()
{
    if (m_dimMaterial) {
        return m_dimMaterial;
    }

    m_dimMaterial = new Qt3DExtras::QPhongMaterial(this);

    QColor dimColor = m_originalColor.darker(900);

    m_dimMaterial->setAmbient(dimColor.darker(150));
    m_dimMaterial->setDiffuse(dimColor);
    m_dimMaterial->setSpecular(Qt::black);
    m_dimMaterial->setShininess(0.0f);

    return m_dimMaterial;
}

void StarEntity::setActiveMaterial(Qt3DRender::QMaterial *material)
{
    if (!material || m_material == material) {
        return;
    }

    if (m_material) {
        removeComponent(m_material);
    }

    m_material = material;
    addComponent(m_material);
}

QColor StarEntity::dimmedColor() const {
    return m_originalColor.darker(500);
}

void StarEntity::applyMaterialColor(const QColor &color, const QColor &specular)
{
    if (!m_colorMaterial) return;

    m_colorMaterial->setAmbient(color);
    m_colorMaterial->setDiffuse(color);
    m_colorMaterial->setSpecular(specular);
}

bool StarEntity::hasTexture() const
{
    return !m_data.texturePath.trimmed().isEmpty();
}

Qt3DRender::QTexture2D* StarEntity::textureForPath(const QString &path)
{
    QString cleanPath = path.trimmed();

    if (s_textureCache.contains(cleanPath)) {
        return s_textureCache.value(cleanPath);
    }

    auto *texture = new Qt3DRender::QTexture2D();

    texture->setGenerateMipMaps(true);
    texture->setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);
    texture->setMinificationFilter(Qt3DRender::QAbstractTexture::LinearMipMapLinear);

    auto *image = new Qt3DRender::QTextureImage(texture);
    image->setSource(QUrl(cleanPath));

    texture->addTextureImage(image);

    s_textureCache.insert(cleanPath, texture);

    return texture;
}

Qt3DRender::QMaterial* StarEntity::createMaterial()
{
    m_originalColor = m_data.color;

    if (hasTexture()) {
        m_textureMaterial = new Qt3DExtras::QTextureMaterial(this);
        m_textureMaterial->setTexture(textureForPath(m_data.texturePath));

        return m_textureMaterial;
    }

    m_colorMaterial = new Qt3DExtras::QPhongMaterial(this);
    m_colorMaterial->setAmbient(m_originalColor);
    m_colorMaterial->setDiffuse(m_originalColor);
    m_colorMaterial->setSpecular(Qt::white);

    return m_colorMaterial;
}
