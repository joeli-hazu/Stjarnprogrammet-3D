#ifndef MENU_H
#define MENU_H

#include <QWidget>
#include "View_global.h" // Se till att denna ligger precis här!

namespace Ui {
class menu;
}

// Skriv raden exakt så här, utan extra kolon eller mellanslag i makrot:
class VIEW_EXPORT menu : public QWidget
{
    Q_OBJECT

public:
    explicit menu(QWidget *parent = nullptr);
    ~menu();

private:
    Ui::menu *ui;

    bool isLoggedIn = false;

private slots:
    void toggleMenu(); // Ta fram meny

    void handleLoginClick(); // Ta fram logga in meny

};

#endif // MENU_H
