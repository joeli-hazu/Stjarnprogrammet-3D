#include "mainwindow.h"
#include "login.h"
#include "qmessagebox.h"
#include "searchpanel.h"
#include "filterpanel.h"
#include <QLabel>
#include <QSqlQuery>
#include "database/favoriteRepository.h"
#include "database/starrepository.h"
#include "star3dview.h"
#include "utils/StarColorHelper.h"
#include "filterpanel.h"
#include "starobject.h"
#include "adminpanel.h"
#include <QTimer>
#include <QStringListModel>


void MainWindow::setupConnections() {
    // LOGIN LOGIK
    connect(m_loginBtn, &QPushButton::clicked, this, [this]() {
        // --- 1. LOGGA UT ---
        if (m_currentUserId != -1) {
            QMenu postLoginMenu(this);

            QAction *logoutAction = postLoginMenu.addAction("Logga ut");

            QAction *selectedItem = postLoginMenu.exec(m_loginBtn->mapToGlobal(QPoint(0, m_loginBtn->height())));


            if (selectedItem == logoutAction) {
                // --- LOGGA UT LOGIK ---
                m_currentUserId = -1;
                m_isAdmin = false;

                // Återställ knappen till standardutseende
                m_loginBtn->setStyleSheet(m_loginBtn->styleSheet().replace("#00ffcc", "#ff4444"));
                m_loginBtn->setToolTip("Logga in");

                // Dölj admin och favoriter
                m_adminBtn->setVisible(false);
                m_favButton->setVisible(true);
                m_favButton->setVisible(true);
                m_favButton->setEnabled(false);
                m_favButton->setText("🤍 Logga in för favoriter");
                if (m_favMenu) { m_favMenu->menuAction()->setVisible(false); }

                updateFavoritesMenu();
            }
            return;
        }

        // --- 2. LOGGA IN ---
        Login login(this);
        login.setWindowModality(Qt::ApplicationModal);
        if (login.exec() == QDialog::Accepted) {
            m_currentUserId = login.getUserId();

            //LOGGA IN ADMIN
            QString username = login.getUsername();

            m_loginBtn->setStyleSheet(m_loginBtn->styleSheet().replace("#ff4444", "#00ffcc"));
            m_loginBtn->setToolTip("Inloggad");

            if (username == "admin") {
                m_isAdmin = true;
                m_adminBtn->setVisible(true);
                m_favButton->setVisible(false);
            } else {
                m_isAdmin = false;
                m_adminBtn->setVisible(false);
                m_favButton->setVisible(true);
            }

            //LOGGA UT OCH FAVORIT KNAPP
            if (m_lastClickedStar.id > 0) {

                bool fav = FavoriteRepository::isFavorite(m_currentUserId, m_lastClickedStar.id);
                m_favButton->setText(fav ? "❤️ Ta bort favorit" : "🤍 Lägg till favorit");
                m_favButton->setEnabled(true);
            }
            if (m_favMenu) { m_favMenu->menuAction()->setVisible(true); }
            updateFavoritesMenu();
        }
    });

    //ADMINPANEL
    connect(m_adminBtn, &QPushButton::clicked, this, [this]() {
        // Ändra 'this' till 'nullptr' här för att göra fönstret fristående
        AdminPanel *panel = new AdminPanel(nullptr);

        // Vi sätter denna flagga för att fönstret ska rensa sig självt
        panel->setAttribute(Qt::WA_DeleteOnClose);

        // Visa fönstret utan exec() för att se om det hjälper
        panel->show();
    });

    // FILTER LOGIK
    connect(m_filterPanel, &FilterPanel::filterChanged, this, [this](const StarFilter &filter) {
        StarRepository repo;
        QList<Star> filteredStars = repo.getFiltered(filter);

        QSet<int> visibleIds;

        for (const Star& s : std::as_const(filteredStars)) {
            visibleIds.insert(s.id);
        }
        m_view->dimStarsExcept(visibleIds);
        if (m_starCountLabel) {
            m_starCountLabel->setText(QString("Antal stjärnor: %1").arg(visibleIds.size()));
        }
    });

    // FAVORIT KNAPP LOGIK
    connect(m_favButton, &QPushButton::clicked, this, [this]() {

        if (m_isAdmin) return;

        if (m_currentUserId == -1) return;

        int starId = m_lastClickedStar.id;
        if (starId <= 0) return;

        if (FavoriteRepository::isFavorite(m_currentUserId, starId)) {
            FavoriteRepository::removeFavorite(m_currentUserId, starId);
            m_favButton->setText("🤍 Lägg till favorit");
        } else {
            FavoriteRepository::addFavorite(m_currentUserId, starId);
            m_favButton->setText("❤️ Ta bort favorit");
        }

        updateFavoritesMenu();
    });

    // SÖKVAL LOGIK
    connect(m_searchPanel, &SearchPanel::starSelected, this, [this](const Star &star) {
        m_view->flyToStarById(star.id);
        bool fav = FavoriteRepository::isFavorite(m_currentUserId, star.id);
        m_favButton->setText(fav ? "❤️ Ta bort favorit" : "🤍 Lägg till favorit");
        m_favButton->setEnabled(true);
    });

    // KLICK PÅ STJÄRNA I 3D
    connect(m_view, &Star3DView::starClicked, this, [this](const StarObject &star) {

        m_lastClickedStar = star;

        if (star.id <= 0) { m_favButton->setEnabled(false); return; }
        if (m_currentUserId != -1) {
            bool fav = FavoriteRepository::isFavorite(m_currentUserId, star.id);
            m_favButton->setText(fav ? "❤️ Ta bort favorit" : "🤍 Lägg till favorit");
            m_favButton->setEnabled(true);
        } else {
            m_favButton->setText("🤍 Logga in för favoriter");
            m_favButton->setEnabled(false);
        }
    });

    // ÅTERSTÄLL VY LOGIK
    connect(m_view, &Star3DView::viewReset, this, [this]() {
        m_favButton->setText("🤍 Lägg till favorit");
        m_favButton->setEnabled(false);
        m_lastClickedStar = StarObject{};
        // Visa alla stjärnor igen och uppdatera räknaren
        m_lastClickedStar = StarObject{};

    });

    connect(m_helpBtn, &QPushButton::clicked, this, [=]() {
        QMessageBox helpBox(this);
        helpBox.setWindowTitle("Guide: Hur man använder Stjärnsystemet");

        QString guideText = R"(
<h3>Välkommen till Stjärnsystemet!</h3>
    <p>Här är en snabbguide för att komma igång:</p>

    <ol>
        <b><li>Navigera i rymden:</li></b>
        Använd <b>piltangenterna</b> för att röra dig upp, ner, vänster och höger.
        Håll in <b>Ctrl och dra med musen eller på styrplattan</b> för att rotera 360° runt en punkt.
        Använd <b> scrollhjulet eller två fingrar på styrplattan</b> för att zooma in och ut.

        <b><li>Sök efter stjärnor:</li></b>
        Skriv <b>hela eller delar av namnet</b> på en stjärna i sökfältet längst upp.
        En <b>lista med träffar</b> dyker upp automatiskt, klicka på en stjärna i listan
        för att <b>flyga direkt dit</b> i rymden.

        <b><li>Sidopanelen och filter:</li></b>
        Klicka på <b>"Sidopanel ⌵"</b> längst upp till höger för att öppna sidopanelen.
        Klicka på <b>"Visa filter"</b> för att filtrera på avstånd, temperatur, massa,
        diameter och spektralklass. Tryck på <b>"Använd Filter"</b> för att tillämpa,
        eller <b>"Återställ filter"</b> för att rensa. Antalet synliga stjärnor
        visas direkt i panelen.

        <b><li>Vyknappar:</li></b>
        <b>"Återställ Vy"</b>: återgår till startläget.<br>
        <b>"Stjärnans Vy"</b>: se hur himlen ser ut från den valda stjärnan.

        <b><li>Favoriter:</li></b>
        Logga in eller skapa ett konto via <b>"Logga in för favoriter"</b>.
        Klicka på en stjärna och markera den som favorit.
        Dina favoriter sparas i en lista – klicka på en för att flyga direkt dit!
    </ol>

    <p><i>Tips: Klicka på en stjärna i rymden för att se detaljerad info om den!</i></p>
    )";

        helpBox.setTextFormat(Qt::RichText);
        helpBox.setText(guideText);
        helpBox.setStandardButtons(QMessageBox::Ok);

        helpBox.setStyleSheet(R"(
        QMessageBox {
            background-color: #0f1028;
        }
        QLabel {
            color: #e0e0e0;
        }
        QPushButton {
            background-color: #1a1b3a;
            color: #00ffcc;
            padding: 5px 15px;
            border-radius: 4px;
        }
    )");

        helpBox.exec();
    });
    // FAVORIT STJÄRN-KNAPP LOGIK
    connect(m_favStarBtn, &QPushButton::clicked, this, [this]() {
        if (m_currentUserId == -1) return;

        QMenu *menu = new QMenu(this);
        QList<int> favIds = FavoriteRepository::getFavorites(m_currentUserId);

        if (favIds.isEmpty()) {
            QAction *empty = menu->addAction("Inga favoriter än");
            empty->setEnabled(false);
        } else {
            for (int id : std::as_const(favIds)) {
                QSqlQuery q;
                q.prepare("SELECT main_id FROM stars WHERE id = :id");
                q.bindValue(":id", id);
                q.exec();
                if (!q.next()) continue;
                QString name = q.value(0).toString();
                QAction *a = menu->addAction(name);
                connect(a, &QAction::triggered, this, [this, id]() {
                    m_view->flyToStarById(id);
                    m_lastClickedStar.id = id;
                    updateFavoriteUI(id);
                });
            }
        }

        menu->exec(m_favStarBtn->mapToGlobal(QPoint(0, m_favStarBtn->height())));
        delete menu;
    });
}

void MainWindow::loadStars() {
    qDebug() << "loadStars START";
    StarRepository repo;
    QList<Star> dbStars = repo.getAll();
    qDebug() << "Hämtat från DB:" << dbStars.size();
    QList<StarObject> starObjects;

    const float spaceScale = 80.0f;

    for (const Star& s : std::as_const(dbStars)) {
        StarObject obj;
        obj.id = s.id;
        obj.name = s.main_id;
        obj.position = QVector3D(s.x * spaceScale, s.y * spaceScale, s.z * spaceScale);


        // Information för info-rutan
        obj.spectralClass = s.spectral_type;
        obj.mass = QString::number(s.mass, 'f', 2) + " solmassor";
        obj.distance = QString::number(s.distance_ly, 'f', 1) + " ly";
        obj.temperature = QString::number(s.temperature, 'f', 0) + " K";
        obj.diameter = QString::number(s.diameter_solar, 'f', 2) + " solradier";

        // HÄR ÄR STORLEKSLOGIKEN
        float size = 1.0f;
        QString type = s.spectral_type.trimmed().toUpper();

        if (type.startsWith("O"))      size = 2500.0f;
        else if (type.startsWith("B")) size = 2000.0f;
        else if (type.startsWith("A")) size = 1200.0f;
        else if (type.startsWith("F")) size = 750.0f;
        else if (type.startsWith("G")) size = 500.0f;
        else if (type.startsWith("K")) size = 200.0f;
        else if (type.startsWith("M")) size = 100.0f;

        obj.size = size; // Spara storleken i objektet
        // ------------------------------

        obj.color = spectralColor(s.spectral_type);
        obj.texturePath = textureForSpectralClass(s.spectral_type);
        // Separera stjärnor som ligger för nära varandra
        for (const StarObject &existing : std::as_const(starObjects)) {
            QVector3D diff = obj.position - existing.position;
            float dist = diff.length();
            if (dist < 0.001f) {
                // Exakt samma position – skjut ut åt sidan
                obj.position += QVector3D(20.0f, 15.0f, 15.0f);
            } else if (dist < 20.0f) {
                // För nära – skjut ut lite mer
                obj.position += diff.normalized() * (30.0f - dist + 10.0f);
            }
        }

        starObjects.append(obj);


    }
    m_totalStarCount = starObjects.size();
    if (m_starCountLabel) {
        m_starCountLabel->setText(QString("Antal stjärnor: %1").arg(m_totalStarCount));
    }
    m_view->setStars(starObjects);

    m_view->update();

    // --- UPPDATERA SÖKLISTAN VIA SEARCHPANEL ---
    if (m_searchPanel) {
        QStringList starNames;
        for (const StarObject& obj : starObjects) {
            starNames << obj.name;
        }

        // Vi anropar en funktion i searchpanel för att uppdatera listan
        m_searchPanel->updateSearchList(starNames);
        qDebug() << "Skickade" << starNames.size() << "namn till SearchPanel.";
    }
}

void MainWindow::updateFavoritesMenu() {
    m_favMenu->clear();
    QList<int> favIds = FavoriteRepository::getFavorites(m_currentUserId);
    for (int id : std::as_const(favIds)) {
        QSqlQuery q;
        q.prepare("SELECT main_id FROM stars WHERE id = :id");
        q.bindValue(":id", id);
        q.exec();
        if (!q.next()) continue;
        QString name = q.value(0).toString();
        QAction *a = new QAction(name, this);
        m_favMenu->addAction(a);
        connect(a, &QAction::triggered, this, [this, id]() {
        m_view->flyToStarById(id);
        m_lastClickedStar.id = id;
        updateFavoriteUI(id);
    });
    }
}
void MainWindow::updateFavoriteUI(int starId)
{
    if (m_currentUserId == -1) {
        m_favButton->setText("🤍 Logga in för favoriter");
        m_favButton->setEnabled(false);
        return;
    }

    bool fav = FavoriteRepository::isFavorite(m_currentUserId, starId);
    m_favButton->setText(fav ? "❤️ Ta bort favorit" : "🤍 Lägg till favorit");
    m_favButton->setEnabled(true);
}

QString MainWindow::textureForSpectralClass(const QString &spectralClass) const
{
    QString type = spectralClass.trimmed().toUpper();

    if (type.startsWith("O")) {
        return "qrc:/new/prefix1/blue_star.png";
    }

    if (type.startsWith("B")) {
        return "qrc:/new/prefix1/blue_star.png";
    }

    if (type.startsWith("A")) {
        return "qrc:/new/prefix1/white_star.png";
    }

    if (type.startsWith("F")) {
        return "qrc:/new/prefix1/yellow_white_star.png";
    }

    if (type.startsWith("G")) {
        return "qrc:/new/prefix1/yellow_sun_star.png";
    }

    if (type.startsWith("K")) {
        return "qrc:/new/prefix1/orange_star.jpg";
    }

    if (type.startsWith("M")) {
        return "qrc:/new/prefix1/red_orange_star.jpg";
    }

    return "qrc:/new/prefix1/moon.jpg";
}
