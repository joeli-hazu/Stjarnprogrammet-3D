#pragma once
#include <QSet>
#include <QWidget>
#include <QPropertyAnimation>
#include <QVector3D>
#include <QEvent>
#include <QColor>
#include <QPoint>
#include <QResizeEvent>
#include <QLabel>

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>
#include <Qt3DExtras/QOrbitCameraController>

#include "starobject.h"

class StarEntity;

class Star3DView : public QWidget
{
    Q_OBJECT

public:
    explicit Star3DView(QWidget *parent = nullptr);
    void setStars(const QList<StarObject> &starList);

public slots:
    void resetView();
    void resetStarView();
    void enterStarView();
    void flyToStarById(int id);
    void dimStarsExcept(const QSet<int> &visibleIds);
    void clearDim();

signals:
    void starClicked(const StarObject &star);
    void viewReset();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void setup3D();
    void setupLighting();

    void clearStars();
    void clearSelectedStar();
    StarEntity* createStarEntity(const StarObject &data);

    StarEntity* findStarEntityById(int id) const;
    void selectStar(StarEntity *entity);

    void exitStarViewMode();
    void setAllStarsViewLocked(bool locked);

    void flyToPosition(const QVector3D &targetPos);
    void updateStarViewCamera();

    void updateCameraProjection();
    void zoomCamera(float delta);

    void resetFov();
    void animateCameraTo(const QVector3D &position,
                         const QVector3D &viewCenter,
                         int durationMs);

    QVector3D currentViewDirection() const;

    bool canEnterStarView() const;
    void enterStarViewMode();

    QVector3D directionFromSelectedStarToCamera() const;
    QVector3D surfaceCameraPosition(const QVector3D &surfaceDir) const;
    void updateLookAnglesFromDirection(const QVector3D &direction);

    bool is3DInputObject(QObject *obj) const;
    void handleScrollWheelEvent(QWheelEvent *event);
    bool handleStarViewMouseEvent(QEvent *event);

    void startMouseLook(const QPoint &pos);
    void stopMouseLook();
    void updateMouseLook(const QPoint &pos);
    QLabel *m_hoverInfo = nullptr;

private:
    Qt3DExtras::Qt3DWindow *m_window3D = nullptr;
    QWidget *m_container = nullptr;

    Qt3DCore::QEntity *m_root = nullptr;
    Qt3DRender::QCamera *m_camera = nullptr;
    Qt3DExtras::QOrbitCameraController *m_camController = nullptr;

    QList<StarEntity*> m_starEntities;

    StarEntity *m_selectedStarEntity = nullptr;

    StarObject m_selectedStar;
    bool m_hasSelectedStar = false;

    bool m_starViewLocked = false;
    bool m_mouseLooking = false;

    QPoint m_lastMousePos;

    QVector3D m_cameraPositionBeforeStarView;
    QVector3D m_cameraViewCenterBeforeStarView;
    QVector3D m_cameraUpBeforeStarView;
    float m_cameraFovBeforeStarView = defaultFov;
    bool m_hasCameraBeforeStarView = false;

    float m_yaw = 0.0f;
    float m_pitch = 0.0f;
    float m_cameraFov = defaultFov;

    QVector3D m_starViewCameraPos;
    QVector3D m_worldUp = QVector3D(0.0f, 1.0f, 0.0f);

    const QVector3D m_defaultCameraPosition = QVector3D(0.0f, 0.0f, 250.0f);
    const QVector3D m_defaultCameraViewCenter = QVector3D(0.0f, 0.0f, 0.0f);
    const QVector3D m_defaultCameraUp = QVector3D(0.0f, 1.0f, 0.0f);
    static constexpr float defaultFov = 60.0f;
    static constexpr float minFov = 40.0f;
    static constexpr float maxFov = 130.0f;
    static constexpr float nearPlane = 0.1f;
    static constexpr float farPlane = 10000.0f;
    static constexpr float minPitch = -89.0f;
    static constexpr float maxPitch = 89.0f;
    static constexpr float zoomSensitivity = 0.05f;
    static constexpr int resetAnimationMs = 1500;
    static constexpr int flyAnimationMs = 1200;
    static constexpr float flyDistanceFromStar = 30.0f;
    static constexpr float starViewLookDistance = 5000.0f;
    static constexpr float starViewSurfaceOffset = 0.5f;
    static constexpr float mouseSensitivity = 0.25f;
    static constexpr float cameraLinearSpeed = 0.0f;
    static constexpr float cameraLookSpeed = 180.0f;
};
