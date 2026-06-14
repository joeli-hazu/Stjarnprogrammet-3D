#include "menu.h"
#include "ui_menu.h"
#include "login.h"

#include <QApplication>

menu::menu(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::menu)
{
    ui->setupUi(this);

    ui->button_menu->raise(); // Tvingar fram knappen överst

    // 1. Göm panelen direkt så den inte syns vid start
    ui->sidePanel->hide();

    // 2. Koppla meny-knappen till funktionen som öppnar menyn
    connect(ui->button_menu, SIGNAL(clicked()), this, SLOT(toggleMenu()));

    // Koppla logga in-knappen till funktionen
    connect(ui->button_login, SIGNAL(clicked()), this, SLOT(handleLoginClick()));
}

// Destructor
menu::~menu()
{
    delete ui;
}

// Logiken för att visa/dölja menyn
void menu::toggleMenu()
{
    if (ui->sidePanel->isVisible()) {
        ui->sidePanel->hide();
    } else {
        ui->sidePanel->show();
        ui->sidePanel->raise(); // Lägger menyn överst
    }
}

void menu::handleLoginClick()
{
    if (isLoggedIn) {
        // Om vi redan är inloggade -> Logga ut
        isLoggedIn = false;
        ui->button_login->setText("Logga in");
    } else {
        // Skapa en instans av din nya inloggningsruta
        Login *loginWindow = new Login();

        // Gör så att fönstret raderas från minnet när det stängs
        loginWindow->setAttribute(Qt::WA_DeleteOnClose);

        // Detta gör att man inte kan klicka på huvudfönstret förrän man stängt inloggningen
        loginWindow->setWindowModality(Qt::ApplicationModal);

        loginWindow->show();

        // För att testa kan vi låta knappen ändras direkt,
        // men senare vill ni nog att detta bara sker om lösenordet är rätt!
        isLoggedIn = true;
        ui->button_login->setText("Logga ut");
    }
}
