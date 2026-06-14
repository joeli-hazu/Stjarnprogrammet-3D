#include "login.h"
#include "ui_login.h"
#include "registerpage.h"
#include "controller.h"
#include <QMessageBox>
#include <QLineEdit>

Login::Login(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    // större textfält:
    ui->Log_lineEdit_Name->setMinimumHeight(32);
    ui->Log_lineEdit_Los->setMinimumHeight(32);
    ui->Log_lineEdit_Name->setMinimumWidth(260);
    ui->Log_lineEdit_Los->setMinimumWidth(260);

    // Tema ändring
    setStyleSheet(R"(
    QWidget {
        background-color: #08091e;
        color: #e0e0e0;
        font-size: 14px;
    }

    QLineEdit {
        background-color: #1a1b2e;
        border: 1px solid #2a2b45;
        color: #ffffff;
        border-radius: 4px;
        padding: 6px;
    }

    QPushButton {
        background-color: #0f1028;
        border: 1px solid #2a2b45;
        color: #c8c8d8;
        border-radius: 4px;
        padding: 6px;
    }

    QPushButton:hover {
        border-color: #4a4b75;
        color: #ffffff;
    }

    QLabel {
        color: #d0d0e0;
        font-weight: bold;
    }
)");

    setMinimumSize(420, 320); // större fönster



    // LOGGA IN
    connect(ui->Log_Button_Log, &QPushButton::clicked,
            this, &Login::on_login_clicked);

    QLabel *forgotLabel = new QLabel("Kontakta admin vid glömt lösenord!", this);

    forgotLabel->setStyleSheet(R"(
        color: white;
        font-size: 12px;
        font-weight: bold;
)");

    forgotLabel->resize(300, 20);
    forgotLabel->move(10, 350);
    forgotLabel->setAlignment(Qt::AlignCenter);;

    ui->Log_lineEdit_Los->setEchoMode(QLineEdit::Password);
}

Login::~Login()
{
    delete ui;
}

void Login::on_log_button_create_clicked()
{
    RegisterPage regPage(this);

    if (regPage.exec() == QDialog::Accepted) {
        this->show();

    }
}

void Login::on_login_clicked()
{
    QString anvandare = ui->Log_lineEdit_Name->text();
    QString losenord  = ui->Log_lineEdit_Los->text();

    if(anvandare.isEmpty() || losenord.isEmpty()) {
        QMessageBox::warning(this, "Fel", "Fyll i allt");
        return;
    }

    // Kontrollera mellanslag
    if(anvandare.contains(" ") || losenord.contains(" ")) {
        QMessageBox::warning(this, "Fel", "Användarnamn och lösenord får inte innehålla mellanslag");
        return;
    }

    //ADMIN LOGGA IN
    if (anvandare == "admin" && losenord == "admin123") {
        m_userId = 0; // admin kan ha id 0
        QMessageBox::information(this, "Admin", "Inloggad som admin");
        accept();
        return;
    }
    anvandare = anvandare.trimmed();
    losenord = losenord.trimmed();
    if(anvandare == "admin" && losenord == "admin123") {
        m_isAdmin = true;
    }

    //VANLIGA ANVÄNDARE LOGGA IN
    Controller controller;

    // Vi anropar den nya versionen av loggaIn som returnerar ett int ID
    int id = controller.loggaIn(anvandare, losenord);

    if(id != -1) {
        // Spara det unika ID:t i variabeln vi skapade i login.h
        m_userId = id;

        QMessageBox::information(this, "Success", "Inloggad!");
        accept(); // Stänger fönstret och returnerar QDialog::Accepted
    } else {
        QMessageBox::critical(this, "Fel", "Fel användarnamn/lösenord");
    }
}

QString Login::getUsername() const {
    return ui->Log_lineEdit_Name->text();
}
