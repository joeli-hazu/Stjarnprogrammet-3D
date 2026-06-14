#include "mainwindow.h"
#include <QHeaderView>
#include "qmenubar.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>

#include "star3dview.h"
#include "searchpanel.h"
#include "filterpanel.h"


void MainWindow::updateDrawerGeometry()
{
    if (!m_rightSideDrawer || !m_topPanel) return;

    int topOffset = m_topPanel->height();
    int drawerWidth = 320;

    // Hämta positionen för programmets övre högra hörn i globala koordinater
    QPoint topRight = this->mapToGlobal(QPoint(this->width(), 0));


    m_rightSideDrawer->setGeometry(
        topRight.x() - drawerWidth,
        topRight.y() + topOffset,
        drawerWidth,
        this->height() - topOffset
        );

    // Tvinga panelen att ligga överst
    m_rightSideDrawer->raise();
}

void MainWindow::setupUI() {
    // Sätt storlek direkt
    this->resize(1200, 800);
    this->setMinimumSize(1000, 700);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    setStyleSheet(R"(
    QMainWindow {
        background-color: #08091e;
        color: #e0e0e0;
    }

    QGroupBox {
        background-color: #0f1028;
        border: 1px solid #1a1b3a;
        border-radius: 6px;
    }

    #infoPanel {
        background-color: #0f1028;
        border: 1px solid #1a1b3a;
    }

    QLabel {
        color: #d0d0e0;
    }
    QGroupBox {
        color: #e0e0e0;
    }

    QGroupBox::title {
         color: #ffffff;
         font-weight: bold;
    }

    QCheckBox {
        color: #e0e0e0;
    }

    QPushButton {
        background-color: transparent;
        color: #c8c8d8;
        border: 1px solid #2a2b45;
        border-radius: 4px;
        padding: 6px;
    }

    QPushButton:hover {
        border-color: #4a4b75;
        color: #ffffff;
    }

    QLineEdit {
        background-color: #1a1b2e;
        border: 1px solid #2a2b45;
        color: #ffffff;
        border-radius: 4px;
        padding: 4px;
    }

    QListWidget {
        background-color: #1a1b2e;
        border: 1px solid #2a2b45;
        color: #e0e0e0;
    }
)");

    m_favMenu = new QMenu(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);


    // Skapar själva Topp-panelen

    m_topPanel = new QGroupBox(central);
    m_topPanel->setObjectName("topPanel");
    m_topPanel->setFixedHeight(80);
    mainLayout->addWidget(m_topPanel);


    QHBoxLayout *topLayout = new QHBoxLayout(m_topPanel);
    topLayout->setContentsMargins(15, 0, 15, 0);

    // Skapa inloggningsknappen med en gubbe
    m_loginBtn = new QPushButton("👤", m_topPanel);
    m_loginBtn->setFixedSize(45, 45);
    m_loginBtn->setToolTip("Logga in / Profil");

    // styling för att göra den rund och fin
    m_loginBtn->setStyleSheet(R"(
    QPushButton {
        background-color: #1a1b3a;
        color: #ff4444;           /* Röd gubbe */
        border-radius: 22px;
        border: 1px solid #ff4444; /* Röd ram */
        font-size: 20px;
    }
    QPushButton:hover {
        background-color: #ff4444;
        color: #1a1b3a;
    }
)");

    topLayout->addWidget(m_loginBtn);


    topLayout->addWidget(m_loginBtn);
    topLayout->addStretch(); // Skjuter innehållet till mitten från vänster

    m_searchPanel = new SearchPanel(m_topPanel);
    m_searchPanel->setVisible(true);
    topLayout->addWidget(m_searchPanel);

    topLayout->addSpacing(10);

    m_favStarBtn = new QPushButton("⭐", m_topPanel);
    m_favStarBtn->setFixedSize(45, 45);

    m_favStarBtn->setToolTip("Favoriter");
    m_favStarBtn->setStyleSheet(R"(
    QPushButton {
        background-color: #1a1b3a;
        color: #ffcc00;
        border-radius: 22px;
        border: 1px solid #ffcc00;
        font-size: 20px;
    }
    QPushButton:hover {
        background-color: #ffcc00;
        color: #1a1b3a;
    }
)");
    topLayout->addWidget(m_favStarBtn);

    topLayout->addStretch();
    m_helpBtn = new QPushButton("?", m_topPanel);
    m_helpBtn->setFixedSize(30, 30);
    m_helpBtn->setToolTip("Hjälp & Guide");
    m_helpBtn->setStyleSheet(R"(
    QPushButton {
        background-color: #1a1b3a;
        color: #00ffcc;
        border-radius: 15px;
        font-weight: bold;
        border: 1px solid #00ffcc;
    }
    QPushButton:hover {
        background-color: #00ffcc;
        color: #1a1b3a;
    }
)");



    topLayout->addWidget(m_helpBtn);


    QPushButton *observeBtn = new QPushButton("Sidopanel ⌵", m_topPanel);
    observeBtn->setCheckable(true);
    observeBtn->setFixedWidth(120);
    observeBtn->setStyleSheet("background-color: #1a1b3a; font-weight: bold; color: #00ffcc;");
    topLayout->addWidget(observeBtn);

    // Skapa 3D-vyn direkt som barn till central
    m_view = new Star3DView(central);
    mainLayout->addWidget(m_view, 1);


    m_rightSideDrawer = new QGroupBox("Information", central);
    m_rightSideDrawer->setObjectName("infoPanel");

    // Sätt flaggor för sök logik
    m_rightSideDrawer->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    m_rightSideDrawer->setFixedWidth(320);

    // Tillåt fokus så att man kan skriva i QLineEdit
    m_rightSideDrawer->setAttribute(Qt::WA_ShowWithoutActivating, false);
    m_rightSideDrawer->setFocusPolicy(Qt::StrongFocus);

    m_rightSideDrawer->hide();

    QVBoxLayout *panelLayout = new QVBoxLayout(m_rightSideDrawer);
    panelLayout->setSpacing(18);
    panelLayout->setContentsMargins(12, 40, 12, 12);

    panelLayout->addSpacing(5);

    QPushButton *toggleFilterBtn = new QPushButton("Visa filter", m_rightSideDrawer);
    toggleFilterBtn->setCheckable(true);
    panelLayout->addWidget(toggleFilterBtn);

    m_filterPanel = new FilterPanel(m_rightSideDrawer);
    m_filterPanel->setVisible(false);
    m_filterPanel->setMaximumHeight(0);
    panelLayout->addWidget(m_filterPanel);
    m_filterPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // Koppling för filter-toggle
    connect(toggleFilterBtn, &QPushButton::clicked, this, [=](bool checked) {
        m_filterPanel->setVisible(checked);

        if (checked) {
            m_filterPanel->setMaximumHeight(16777215);
        } else {
            m_filterPanel->setMaximumHeight(0);
        }

        toggleFilterBtn->setText(checked ? "Dölj filter" : "Visa filter");
    });

    panelLayout->addStretch();
    // Skapa etiketten för stjärnantal
    m_starCountLabel = new QLabel("Visar alla stjärnor", m_rightSideDrawer);
    m_starCountLabel->setStyleSheet("color: #00ffcc; font-weight: bold;");
    panelLayout->addWidget(m_starCountLabel);
    panelLayout->addSpacing(20);

    // NAVIGERINGSKNAPPAR
    m_overlayResetBtn = new QPushButton("Återställ Vy", m_rightSideDrawer);
    panelLayout->addWidget(m_overlayResetBtn);
    connect(m_overlayResetBtn, &QPushButton::clicked, m_view, &Star3DView::resetView);

    m_overlayStarViewBtn = new QPushButton("Stjärnans vy", m_rightSideDrawer);
    panelLayout->addWidget(m_overlayStarViewBtn);
    connect(m_overlayStarViewBtn, &QPushButton::clicked, m_view, &Star3DView::enterStarView);

    m_overlayResetStarViewBtn = new QPushButton("Återställ stjärnvy", m_rightSideDrawer);
    panelLayout->addWidget(m_overlayResetStarViewBtn);
    connect(m_overlayResetStarViewBtn, &QPushButton::clicked,
        m_view, &Star3DView::resetStarView);

    panelLayout->addSpacing(20);

    // FAVORIT & LOGIN KNAPPAR
    m_favButton = new QPushButton("🤍 Logga in för favoriter", m_rightSideDrawer);
    m_favButton->setEnabled(false);
    panelLayout->addWidget(m_favButton);

    m_adminBtn = new QPushButton("Admin panel", m_rightSideDrawer);
    panelLayout->addWidget(m_adminBtn);

    m_adminBtn->setVisible(false);
    panelLayout->addStretch(1);

    connect(observeBtn, &QPushButton::toggled, this, [=](bool checked) {
        if (checked) {
            updateDrawerGeometry();
            m_rightSideDrawer->show();
            m_rightSideDrawer->raise();
            m_rightSideDrawer->activateWindow();
        } else {
            m_rightSideDrawer->hide();
        }
        observeBtn->setText(checked ? "Stäng ⌃" : "Sidopanel ⌵");
    });

}
