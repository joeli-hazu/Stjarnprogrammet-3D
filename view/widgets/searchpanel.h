#ifndef SEARCHPANEL_H
#define SEARCHPANEL_H

#include <QWidget>
#include <QLineEdit>
#include <QListWidget>
#include "models/Star.h"

class SearchPanel : public QWidget {
    Q_OBJECT
public:
    explicit SearchPanel(QWidget *parent = nullptr);
    void updateSearchList(const QStringList &names);

signals:
    void starSelected(const Star &star);

private slots:
    void onTextChanged(const QString &text);
    void onItemClicked(QListWidgetItem *item);

private:
    void populateList(const QList<Star> &stars);

    QLineEdit   *m_input   = nullptr;
    QListWidget *m_list    = nullptr;
    QList<Star>  m_current;
};

#endif // SEARCHPANEL_H
