#ifndef ADMINPANEL_H
#define ADMINPANEL_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>

class AdminPanel : public QDialog
{
    Q_OBJECT

public:
    explicit AdminPanel(QWidget *parent = nullptr);

private:
    //STJÄRNOR
    QPushButton *m_editBtn;
    QListWidget *m_list;
    QPushButton *m_addBtn;
    QPushButton *m_deleteBtn;

    void loadStars();


    //ANVÄNDARE
    QListWidget *m_userList;
    QPushButton *m_deleteUserBtn;
    QPushButton *m_changePassBtn;

    void loadUsers();
};

#endif
