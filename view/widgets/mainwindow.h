#pragma once
#include <QMenu>
#include <QPushButton>
#include <QMainWindow>
#include <QTableWidget>
#include <QGroupBox>
#include "starobject.h"
#include <QMainWindow>
#include <QResizeEvent>




class QLabel;
class SearchPanel;
class FilterPanel;
class Star3DView;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

    QMenu *m_favMenu = nullptr;
    QPushButton *m_favButton = nullptr;
    QPushButton *m_loginBtn = nullptr;
    QPushButton *m_adminBtn = nullptr;
    int m_totalStarCount = 0;
    QPushButton *m_favStarBtn = nullptr;




    QGroupBox *m_rightSideDrawer = nullptr;
    QPushButton *m_helpBtn = nullptr;
    QPushButton *m_overlayResetBtn = nullptr;
    QPushButton *m_overlayStarViewBtn = nullptr;
    QLabel *m_starCountLabel = nullptr;
    QPushButton *m_overlayResetStarViewBtn = nullptr;


    int m_currentUserId = -1; // tills login finns
    StarObject m_lastClickedStar;
    void refreshInitialData();
    void updateFavoritesMenu();
    void updateDrawerGeometry();


    bool m_isAdmin = false;

private slots:
    void on_adminBtn_clicked();

protected:
    // används för att panelen ska flytta sig med fönstret
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
private:

    QGroupBox *m_topPanel;
    void setupUI();
    void setupConnections();
    void loadStars();
    Star3DView *m_view = nullptr;  // 3D-vyn



    // används för att kunna styra panelerna
    SearchPanel *m_searchPanel  = nullptr;
    FilterPanel *m_filterPanel = nullptr;

    bool m_filterVisible = false;
    bool m_searchVisible = false;
    void updateFavoriteUI(int starId);
    void updateInfoVisibility();

    QString textureForSpectralClass(const QString &spectralClass) const;





};
