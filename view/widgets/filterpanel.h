#ifndef FILTERPANEL_H
#define FILTERPANEL_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QList>
#include "models/StarFilter.h"

class FilterPanel : public QWidget {
    Q_OBJECT
public:
    explicit FilterPanel(QWidget *parent = nullptr);

signals:
    void filterChanged(const StarFilter &filter);

private slots:
    void onApplyButtonClicked();
    void onResetButtonClicked();

private:
    // Inmatningsfält för Min/Max
    QLineEdit *m_distMin;  QLineEdit *m_distMax;
    QLineEdit *m_tempMin;  QLineEdit *m_tempMax;
    QLineEdit *m_massMin;  QLineEdit *m_massMax;
    QLineEdit *m_diamMin;  QLineEdit *m_diamMax;

    // Checkboxar för spektralklasser
    QList<QCheckBox*> m_specChecks;

    QPushButton *m_applyBtn;
    QPushButton *m_resetBtn;
};

#endif