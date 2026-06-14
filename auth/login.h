#pragma once

#include <QDialog>

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

    // MainWindow ska kunna hämta ID:t
    int getUserId() const { return m_userId; }
    QString getUsername() const;

private:
    bool m_isAdmin = false;

public:
    bool isAdmin() const;

private slots:
    void on_log_button_create_clicked(); //
    void on_login_clicked();             //

private:
    Ui::Login *ui;                       //

    // Variabel för att lagra ID:t internt
    int m_userId = -1;
};
