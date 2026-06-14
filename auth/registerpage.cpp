#include "controller.h"
#include "registerpage.h"
#include "ui_Registerpage.h"
#include <QMessageBox>
#include <QLineEdit>

RegisterPage::RegisterPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterPage)
{
    ui->setupUi(this);

    ui->lineEdit_reg_los->setEchoMode(QLineEdit::Password);
}

RegisterPage::~RegisterPage()
{
    delete ui;
}

void RegisterPage::on_pushButton_reg_skapa_clicked()
{
    QString anvandare = ui->lineEdit_reg_anv->text();
    QString losenord = ui->lineEdit_reg_los->text();

    if(anvandare.isEmpty() || losenord.isEmpty()) {
        QMessageBox::warning(this, "Input saknas", "Du måste fylla i både namn och lösenord!");
        return;
    }

    if(anvandare.contains(" ") || losenord.contains(" ")) {
        QMessageBox::warning(this, "Fel", "Användarnamn och lösenord får inte innehålla mellanslag");
        return;
    }

    anvandare = anvandare.trimmed();
    losenord = losenord.trimmed();


    // Kontrollera om användaren försöker registrera 'admin'
    if (anvandare.toLower() == "admin") {
        QMessageBox::warning(this, "Fel", " Otillåtet Användarnamn");
        return;
    }


    Controller authController;

    if(authController.registreraAnvandare(anvandare, losenord)) {
        QMessageBox::information(this, "Success", "Kontot har skapats!");

        accept();
    } else {
        QMessageBox::critical(this, "Fel", "Kunde inte skapa konto. Namnet kan vara upptaget.");
    }
}
