#include "star3dview.h"
#include "starentity.h"

#include <QEasingCurve>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QtMath>

#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DRender/QCameraLens>
#include <Qt3DRender/QPointLight>

// Star3DView
//    - äger kameran
//    - äger valt state
//    - bestämmer vad som händer när en stjärna väljs
//    - går in/ut ur stjärnvy
//    - flyttar kameran
//    - "m_variabel" innebär att det är en medlemsvariabel till klassen
//    - "variabel" innebär att det är lokal variabel inom den funktionen

// Konstruktör      start ------------------
Star3DView::Star3DView(QWidget *parent)
    : QWidget(parent)
{
    setup3D();

    m_container = QWidget::createWindowContainer(m_window3D, this);
    m_container->setFocusPolicy(Qt::StrongFocus);
    m_container->setMouseTracking(true);

    m_container->installEventFilter(this);
    m_window3D->installEventFilter(this);

    m_hoverInfo = new QLabel(this); // Skapa labeln som barn till denna widget
    m_hoverInfo->setObjectName("starHoverLabel");
    m_hoverInfo->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    m_hoverInfo->setAttribute(Qt::WA_ShowWithoutActivating);
    m_hoverInfo->setStyleSheet(R"(
        #starHoverLabel {
            background-color: rgba(15, 16, 40, 210);
            color: #ffffff;
            border: 1px solid #4a4b75;
            border-radius: 6px;
            padding: 8px;
            font-size: 12px;
        }
    )");
    m_hoverInfo->setVisible(false);
    // Gör att musklick går igenom rutan så de inte blockerar 3D-vyn
    m_hoverInfo->setAttribute(Qt::WA_TransparentForMouseEvents);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_container);

    updateCameraProjection();
}
// Konstruktör      slut ------------------

// Setup            start ------------------
void Star3DView::setup3D() {
    m_window3D = new Qt3DExtras::Qt3DWindow();
    m_window3D->defaultFrameGraph()->setClearColor(QColor(0, 0, 0));

    m_root = new Qt3DCore::QEntity();
    m_window3D->setRootEntity(m_root);

    setupLighting();

    m_camera = m_window3D->camera();
    updateCameraProjection();

    m_camera->setPosition(m_defaultCameraPosition);
    m_camera->setViewCenter(m_defaultCameraViewCenter);
    m_camera->setUpVector(m_defaultCameraUp);

    m_camController = new Qt3DExtras::QOrbitCameraController(m_root);
    m_camController->setCamera(m_camera);
    m_camController->setLinearSpeed(cameraLinearSpeed);
    m_camController->setLookSpeed(cameraLookSpeed);
}

void Star3DView::setupLighting()
{
    auto *lightEntity = new Qt3DCore::QEntity(m_root);

    auto *light = new Qt3DRender::QPointLight(lightEntity);
    light->setColor(Qt::white);
    light->setIntensity(2.0f);

    auto *lightTransform = new Qt3DCore::QTransform(lightEntity);
    lightTransform->setTranslation(QVector3D(0.0f, 0.0f, 250.0f));

    lightEntity->addComponent(light);
    lightEntity->addComponent(lightTransform);
}

void Star3DView::updateCameraProjection() {

    if (!m_camera) return;

    float aspectRatio = 16.0f / 9.0f;

    if (m_container && m_container->height() > 0) {
        aspectRatio =
            static_cast<float>(m_container->width()) /
            static_cast<float>(m_container->height());
    }

    m_camera->lens()->setPerspectiveProjection(
        m_cameraFov,
        aspectRatio,
        nearPlane,
        farPlane
        );
}

void Star3DView::resizeEvent(QResizeEvent *event)
{
    // Anpassar kamerans projektion när widgeten ändrar storlek, 
    // så att inte bilden sträcks eller klipps på ett konstigt sätt
    QWidget::resizeEvent(event);
    updateCameraProjection();
}
// Setup            slut ------------------

// Stjärnhantering  start ------------------
void Star3DView::setStars(const QList<StarObject> &starList) {
    // Lämnar stjärnvyn om sant och rensar alla stjärnor
    if (m_starViewLocked) {
        exitStarViewMode();
    }

    clearStars();
    clearSelectedStar();
    
    // Tar sedan emot alla stjärnor som ska visas och lägger till dem i createStarEntity
    for (const auto &data : starList) {
        m_starEntities.append(createStarEntity(data));
    }
}

void Star3DView::clearStars()
{
    qDeleteAll(m_starEntities);
    m_starEntities.clear();
}

void Star3DView::clearSelectedStar()
{
    m_selectedStarEntity = nullptr;
    m_hasSelectedStar = false;
}

StarEntity* Star3DView::createStarEntity(const StarObject &data)
{
    // Funktionen skapar en ny stjärna i 3D-vyn
    auto *star = new StarEntity(data, m_root);
    star->setViewLocked(m_starViewLocked);
    // och när stjärnan klickas på så anropas selectStar med den stjärnan som argument, 
    // vilket gör att den stjärnan blir vald och kameran flyger dit enligt selectStar()
    connect(star, &StarEntity::clicked, this, [this](StarEntity *entity) {
        selectStar(entity);
    });

    return star;
}

StarEntity* Star3DView::findStarEntityById(int id) const
{
    // Funktionen letar igenom alla stjärnor i 3D-vyn och returnerar den som har det matchande id:t
    for (auto *star : m_starEntities) {
        if (!star) continue;

        if (star->id() == id) {
            return star;
        }
    }

    return nullptr;
}

void Star3DView::flyToStarById(int id) 
{
    // Funktionen hittar stjärnan med det angivna id:t och flyger kameran dit genom att anropa selectStar
    StarEntity *entity = findStarEntityById(id);
    selectStar(entity);
}
// Stjärnhantering  slut ------------------

// Val av stjärna   start ------------------
void Star3DView::selectStar(StarEntity *entity)
{
    // Funktionen väljer en stjärna i 3D-vyn
    if (!entity) return;

    if (m_starViewLocked) {
        exitStarViewMode();
    }

    if (m_selectedStarEntity && m_selectedStarEntity != entity) {
        m_selectedStarEntity->setEnabled(true);
    }

    entity->setEnabled(true);

    // Sparar vilken stjärna som är vald
    m_selectedStarEntity = entity;
    m_selectedStar = entity->data();
    m_hasSelectedStar = true;

    QString infoText = QString(
                           "<div style='line-height: 140%;'>"
                           "<b style='font-size: 14px; color: #ffffff;'>%1</b><br>"
                           "<hr style='border: 0; border-top: 1px solid #4a4b75;'>"
                           "<span style='color: #a0a0ff;'>Klass:</span> %2<br>"
                           "<span style='color: #a0a0ff;'>Avstånd:</span> %3<br>"
                           "<span style='color: #a0a0ff;'>Temp:</span> %4<br>"
                           "<span style='color: #a0a0ff;'>Massa:</span> %5<br>"
                           "<span style='color: #a0a0ff;'>Diameter:</span> %6<br>"
                           "<span style='color: #a0a0ff;'>Position:</span> <span style='font-family: monospace; color: #ccffcc;'>"
                           "X:%7 Y:%8 Z:%9</span>"
                           "</div>"
                           )
                           .arg(m_selectedStar.name)
                           .arg(m_selectedStar.spectralClass)
                           .arg(m_selectedStar.distance)
                           .arg(m_selectedStar.temperature)
                           .arg(m_selectedStar.mass)
                           .arg(m_selectedStar.diameter)
                           .arg(m_selectedStar.position.x(), 0, 'f', 1)
                           .arg(m_selectedStar.position.y(), 0, 'f', 1)
                           .arg(m_selectedStar.position.z(), 0, 'f', 1);

    // Sätt faktiskt texten i labeln!
    m_hoverInfo->setText(infoText);
    // Räkna ut storleken baserat på den nya texten
    m_hoverInfo->adjustSize();

    QPoint globalPos = m_container->mapToGlobal(QPoint(20, 20));
    m_hoverInfo->move(globalPos);

    m_hoverInfo->show();
    m_hoverInfo->raise();

    emit starClicked(m_selectedStar);

    flyToPosition(m_selectedStar.position);
}
// Val av stjärn    slut ------------------

// Kamerarörelse    start ------------------
void Star3DView::animateCameraTo(const QVector3D &position,
                                 const QVector3D &viewCenter,
                                 int durationMs) 
{
    // Animerar kameran till en ny position och synpunkt
    auto *posAnim = new QPropertyAnimation(m_camera, "position");
    posAnim->setDuration(durationMs);
    posAnim->setStartValue(m_camera->position());
    posAnim->setEndValue(position);
    posAnim->setEasingCurve(QEasingCurve::InOutCubic);

    auto *centerAnim = new QPropertyAnimation(m_camera, "viewCenter");
    centerAnim->setDuration(durationMs);
    centerAnim->setStartValue(m_camera->viewCenter());
    centerAnim->setEndValue(viewCenter);
    centerAnim->setEasingCurve(QEasingCurve::InOutCubic);

    posAnim->start(QAbstractAnimation::DeleteWhenStopped);
    centerAnim->start(QAbstractAnimation::DeleteWhenStopped);
} // Refaktorisera till två olika metoder

void Star3DView::flyToPosition(const QVector3D &targetPos) {
    if (!m_camera) return;

    // Flyttar kameran till en position nära den valda stjärnan, så att den syns tydligt i mitten av skärmen
    QVector3D endPos = targetPos + currentViewDirection() * flyDistanceFromStar;

    animateCameraTo(endPos, targetPos, flyAnimationMs);
}

void Star3DView::resetView() 
{
    // Återställer kameran till utgångsläget och lämnar stjärnvyn om den är aktiv
    exitStarViewMode();
    resetFov();
    animateCameraTo(m_defaultCameraPosition, m_defaultCameraViewCenter, resetAnimationMs);
    clearSelectedStar();

    m_camera->setUpVector(m_defaultCameraUp);

    emit viewReset();
}

void Star3DView::resetStarView()
{
    // Den här funktionen återställer bara stjärnvyn.
    // Den gör inte samma sak som resetView(), eftersom resetView()
    // återställer hela scenen och rensar vald stjärna.

    if (!m_camera) return;
    if (!m_hasSelectedStar || !m_selectedStarEntity) return;

    exitStarViewMode();

    if (m_hasCameraBeforeStarView) {
        m_cameraFov = m_cameraFovBeforeStarView;
        updateCameraProjection();

        m_camera->setUpVector(m_cameraUpBeforeStarView);

        animateCameraTo(
            m_cameraPositionBeforeStarView,
            m_cameraViewCenterBeforeStarView,
            resetAnimationMs
        );

        return;
    }

    // Fallback om användaren av någon anledning trycker på återställ
    // utan att vi har sparat ett tidigare stjärnvy-läge.
    m_camera->setUpVector(m_defaultCameraUp);
    flyToPosition(m_selectedStar.position);
}

void Star3DView::resetFov()
{
    // Återställer kamerans field of view till standardvärdet
    m_cameraFov = defaultFov;
    updateCameraProjection();
}

QVector3D Star3DView::currentViewDirection() const 
{
    // Beräknar riktningen som kameran tittar i, baserat på dess position och synpunkt
    QVector3D viewDir = m_camera->position() - m_camera->viewCenter();

    if (viewDir.lengthSquared() < 0.0001f) {
        return QVector3D(0.0f, 0.0f, 1.0f);
    }

    return viewDir.normalized();
}

void Star3DView::zoomCamera(float delta) 
{
    // Zoomar kameran in eller ut baserat på scrollwheel rörelse, genom att ändra field of view
    if (!m_camera) return;

    m_cameraFov -= delta * zoomSensitivity;
    m_cameraFov = qBound(minFov, m_cameraFov, maxFov);

    updateCameraProjection();
}
//Kamerarörelse     slut ------------------

//Stjärnvy          start ------------------
void Star3DView::enterStarView() 
{
    // Försöker gå in i stjärnvyn om det är möjligt
    if (!canEnterStarView()) return;

    // Spara kamerans läge innan vi går ner i stjärnvyn.
    // Detta används av resetStarView(), så att vi kan komma tillbaka
    // till läget där användaren tittade på den valda stjärnan.
    m_cameraPositionBeforeStarView = m_camera->position();
    m_cameraViewCenterBeforeStarView = m_camera->viewCenter();
    m_cameraUpBeforeStarView = m_camera->upVector();
    m_cameraFovBeforeStarView = m_cameraFov;
    m_hasCameraBeforeStarView = true;

    enterStarViewMode(); // Låser vyn och stänger av 'rotering' av kameran

    QVector3D surfaceDir = directionFromSelectedStarToCamera();
    m_starViewCameraPos = surfaceCameraPosition(surfaceDir);

    updateLookAnglesFromDirection(surfaceDir);

    QVector3D lookTarget = m_starViewCameraPos + surfaceDir * starViewLookDistance;
    animateCameraTo(m_starViewCameraPos, lookTarget, resetAnimationMs);
}

bool Star3DView::canEnterStarView() const
{
    // Kontrollerar om det är möjligt att gå in i stjärnvyn
    return m_camera && m_hasSelectedStar && m_selectedStarEntity;
}

void Star3DView::enterStarViewMode()
{
    // Låser vyn och stänger av 'rotering' av kameran
    m_starViewLocked = true;
    m_mouseLooking = false;

    setAllStarsViewLocked(true);

    if (m_camController) {
        m_camController->setEnabled(false);
    }

    m_selectedStarEntity->setEnabled(false);
}

void Star3DView::exitStarViewMode()
{
    if (m_selectedStarEntity) {
        m_selectedStarEntity->setEnabled(true);
    }
    if (m_hoverInfo) {
        m_hoverInfo->hide();
    }

    m_starViewLocked = false;
    m_mouseLooking = false;

    setAllStarsViewLocked(false);

    if (m_camController) {
        m_camController->setEnabled(true);
        m_camController->setLinearSpeed(cameraLinearSpeed);
        m_camController->setLookSpeed(cameraLookSpeed);
    }
}

void Star3DView::setAllStarsViewLocked(bool locked)
{
    for (auto *star : m_starEntities) {
        if (star) {
            star->setViewLocked(locked);
        }
    }
}

QVector3D Star3DView::directionFromSelectedStarToCamera() const
{
    QVector3D direction = m_camera->position() - m_selectedStar.position;

    if (direction.lengthSquared() < 0.0001f) {
        return QVector3D(0.0f, 0.0f, 1.0f);
    }

    return direction.normalized();
}

QVector3D Star3DView::surfaceCameraPosition(const QVector3D &surfaceDir) const
{
    float radius = StarVisual::radiusFromSize(m_selectedStar.size);
    return m_selectedStar.position + surfaceDir * (radius + starViewSurfaceOffset);
}

void Star3DView::updateLookAnglesFromDirection(const QVector3D &direction)
{
    m_yaw = qRadiansToDegrees(std::atan2(direction.x(), direction.z()));
    m_pitch = qRadiansToDegrees(std::asin(direction.y()));
}

void Star3DView::updateStarViewCamera() 
{
    // Uppdaterar kamerans position och synpunkt baserat på den aktuella yaw/pitch och avståndet från stjärnan
    // Används när man rör musen i stjärnvyn för att titta runt stjärnan, 
    // så att kameran flyttar sig i en sfärisk bana runt stjärnan
    if (!m_camera) return;

    float yawRad = qDegreesToRadians(m_yaw);
    float pitchRad = qDegreesToRadians(m_pitch);

    QVector3D forward;
    forward.setX(std::sin(yawRad) * std::cos(pitchRad));
    forward.setY(std::sin(pitchRad));
    forward.setZ(std::cos(yawRad) * std::cos(pitchRad));
    forward.normalize();

    m_camera->setPosition(m_starViewCameraPos);
    m_camera->setViewCenter(m_starViewCameraPos + forward * starViewLookDistance);
    m_camera->setUpVector(m_worldUp);
}
// Stjärnvy         slut ------------------

// Input hantering  start ------------------
bool Star3DView::eventFilter(QObject *obj, QEvent *event) 
{
    // Kontrollerar om eventet ska hanteras av 3D-vyn
    if (!is3DInputObject(obj)) {
        return QWidget::eventFilter(obj, event);
    }

    if (event->type() == QEvent::Wheel) {
        handleScrollWheelEvent(static_cast<QWheelEvent*>(event));
        return true;
    }

    if (m_starViewLocked && handleStarViewMouseEvent(event)) {
        return true;
    }
    
    return QWidget::eventFilter(obj, event);
}

//“Är det här objektet ett objekt som tillhör 3D-vyn
// och som ska få mus-/scroll-input?”
bool Star3DView::is3DInputObject(QObject *obj) const
{
    return obj == m_container || obj == m_window3D;
}

void Star3DView::handleScrollWheelEvent(QWheelEvent *event)
{
    zoomCamera(event->angleDelta().y());
}

bool Star3DView::handleStarViewMouseEvent(QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        auto *mouseEvent = static_cast<QMouseEvent*>(event);

        if (mouseEvent->button() == Qt::LeftButton) {
            startMouseLook(mouseEvent->pos());
            return true;
        }
    }

    if (event->type() == QEvent::MouseButtonRelease) {
        auto *mouseEvent = static_cast<QMouseEvent*>(event);

        if (mouseEvent->button() == Qt::LeftButton) {
            stopMouseLook();
            return true;
        }
    }

    if (event->type() == QEvent::MouseMove && m_mouseLooking) {
        auto *mouseEvent = static_cast<QMouseEvent*>(event);
        updateMouseLook(mouseEvent->pos());
        return true;
    }

    return false;
}

void Star3DView::startMouseLook(const QPoint &pos)
{
    m_container->setFocus();
    m_mouseLooking = true;
    m_lastMousePos = pos;
}

void Star3DView::stopMouseLook()
{
    m_mouseLooking = false;
}

void Star3DView::updateMouseLook(const QPoint &pos)
{
    QPoint delta = pos - m_lastMousePos;
    m_lastMousePos = pos;

    m_yaw += delta.x() * mouseSensitivity;
    m_pitch -= delta.y() * mouseSensitivity;
    m_pitch = qBound(minPitch, m_pitch, maxPitch);

    updateStarViewCamera();
}
// Input hantering  slut ------------------

// Dimning          start ------------------
void Star3DView::dimStarsExcept(const QSet<int> &visibleIds) 
{
    // Dimmar alla stjärnor utom de som är i den givna mängden
    for (auto *star : m_starEntities) {
        if (!star) continue;

        bool shouldStayVisible = visibleIds.contains(star->id());
        star->setDimmed(!shouldStayVisible);
    }
}

void Star3DView::clearDim() 
{
    // Tar bort dimning från alla stjärnor
    for (auto *star : m_starEntities) {
        if (!star) {
            continue;
        }

        star->setDimmed(false);
    }
}
// Dimning          slut ------------------
