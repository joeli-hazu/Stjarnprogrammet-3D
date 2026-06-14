#include "mainwindow.h"
#include <QResizeEvent>
#include <QCloseEvent>
#include <QApplication>
#include "adminpanel.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // 1. Skapa gränssnittet (koden i mainwindow_ui.cpp)
    setupUI();

    // 2. Koppla ihop knappar och händelser (koden i mainwindow_logik.cpp)
    setupConnections();

    // 3. Ladda in stjärnorna från databasen (koden i mainwindow_logik.cpp)
    loadStars();

    // 4. Se till att favoritmenyn är redo
    updateFavoritesMenu();
}


void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    if (m_rightSideDrawer && m_rightSideDrawer->isVisible()) {
        updateDrawerGeometry();
    }
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
    QApplication::quit();
}


void MainWindow::on_adminBtn_clicked() {
    AdminPanel admin(this);
    admin.exec(); // Detta öppnar fönstret MODALT
}