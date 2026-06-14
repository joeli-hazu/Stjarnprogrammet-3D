#ifndef REGISTERPAGE_H
#define REGISTERPAGE_H
#include <QDialog>

#include <QWidget>

namespace Ui {
class RegisterPage; // Detta måste matcha namnet i din .ui-fil
}

class RegisterPage : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterPage(QWidget *parent = nullptr);
    ~RegisterPage();

private slots:
    void on_pushButton_reg_skapa_clicked(); // För "SKAPA KONTO"-knapp

private:
    Ui::RegisterPage *ui;
};

#endif // REGISTERPAGE_H
