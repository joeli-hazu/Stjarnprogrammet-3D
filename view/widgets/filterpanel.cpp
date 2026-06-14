#include "filterpanel.h"
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QLocale>
#include <QCheckBox>
#include <QSpacerItem>

FilterPanel::FilterPanel(QWidget *parent) : QWidget(parent) {

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // 1. Siffer-filter (Avstånd, Temp, Massa, Diameter)
    QGroupBox *numGroup = new QGroupBox("Filter (Min - Max)", this);
    QGridLayout *grid = new QGridLayout(numGroup);

    grid->addItem(new QSpacerItem(0, 20), 0, 0);

    grid->setHorizontalSpacing(8);
    grid->setVerticalSpacing(8);

    grid->setColumnStretch(0, 1); // label
    grid->setColumnStretch(1, 2); // min
    grid->setColumnStretch(2, 2); // max



    // Avstånd
    m_distMin = new QLineEdit();
    m_distMax = new QLineEdit();
    m_distMin->setPlaceholderText("Min ly");
    m_distMax->setPlaceholderText("Max ly");
    m_distMin->setMinimumHeight(28);
    m_distMax->setMinimumHeight(28);
    grid->addWidget(new QLabel("Avstånd:"), 1, 0);
    grid->addWidget(m_distMin, 1, 1);
    grid->addWidget(m_distMax, 1, 2);

    // Temperatur
    m_tempMin = new QLineEdit();
    m_tempMax = new QLineEdit();
    m_tempMin->setPlaceholderText("Min K");
    m_tempMax->setPlaceholderText("Max K");
    m_tempMin->setMinimumHeight(28);
    m_tempMax->setMinimumHeight(28);
    grid->addWidget(new QLabel("Temp (K):"), 2, 0);
    grid->addWidget(m_tempMin, 2, 1);
    grid->addWidget(m_tempMax, 2, 2);

    // Massa
    m_massMin = new QLineEdit();
    m_massMax = new QLineEdit();
    m_massMin->setPlaceholderText("Min sol");
    m_massMax->setPlaceholderText("Max sol");
    m_massMin->setMinimumHeight(28);
    m_massMax->setMinimumHeight(28);
    grid->addWidget(new QLabel("Massa:"), 3, 0);
    grid->addWidget(m_massMin, 3, 1);
    grid->addWidget(m_massMax, 3, 2);

    // Diameter
    m_diamMin = new QLineEdit();
    m_diamMax = new QLineEdit();
    m_diamMin->setPlaceholderText("Min sol");
    m_diamMax->setPlaceholderText("Max sol");
    m_diamMin->setMinimumHeight(28);
    m_diamMax->setMinimumHeight(28);
    grid->addWidget(new QLabel("Diameter:"), 4, 0);
    grid->addWidget(m_diamMin, 4, 1);
    grid->addWidget(m_diamMax, 4, 2);

    grid->setRowMinimumHeight(1, 35);
    grid->setRowMinimumHeight(2, 35);
    grid->setRowMinimumHeight(3, 35);
    grid->setRowMinimumHeight(4, 35);



    numGroup->setStyleSheet("QGroupBox { margin-top: 25px; padding: 10px; } "
                            "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 3px; }");

    // --- 2. Spektralklasser ---
    QGroupBox *specGroup = new QGroupBox("Spektralklasser", this);
    QGridLayout *specLayout = new QGridLayout(specGroup);
    specLayout->setContentsMargins(10, 25, 10, 10);

    QStringList classes = {"B", "A", "F", "G", "K", "M"};
    int col = 0;
    for (const QString &c : classes) {
        QCheckBox *cb = new QCheckBox(c);
        cb->setChecked(true);
        m_specChecks.append(cb);
        specLayout->addWidget(cb, 0, col++);
    }

    // --- 3. Filtrera-knapp ---
    m_applyBtn = new QPushButton("Använd Filter");
    m_applyBtn->setStyleSheet("background-color: #27ae60; color: white; font-weight: bold; padding: 8px;");
    m_applyBtn->setMinimumHeight(40);
    m_resetBtn = new QPushButton("Återställ filter");
    m_resetBtn->setMinimumHeight(36);
    m_resetBtn->setStyleSheet("color: #e07070; border: 1px solid #e07070; padding: 6px; background: transparent;");

    mainLayout->addWidget(numGroup);
    mainLayout->addSpacing(15);
    mainLayout->addWidget(specGroup);
    mainLayout->addSpacing(15);
    mainLayout->addWidget(m_applyBtn);
    mainLayout->addWidget(m_resetBtn);
    mainLayout->addStretch();

    connect(m_applyBtn, &QPushButton::clicked, this, &FilterPanel::onApplyButtonClicked);
    connect(m_resetBtn, &QPushButton::clicked, this, &FilterPanel::onResetButtonClicked);
}

void FilterPanel::onApplyButtonClicked() {
    StarFilter filter;
    QLocale cLocale(QLocale::C);

    // Hjälpfunktion för att läsa numeriska fält korrekt
    // Krav: "Om jag lämnar fält tomt -> det filtret ignoreras"
    auto capture = [&](QLineEdit* minF, QLineEdit* maxF, NumericFilter& out) {
        bool hasMin = !minF->text().isEmpty();
        bool hasMax = !maxF->text().isEmpty();

        if (hasMin || hasMax) {
            out.active = true;
            // byter ut komma mot punkt
            QString minStr = minF->text().replace(',', '.');
            QString maxStr = maxF->text().replace(',', '.');
            // Om min saknas sätt 0, om max saknas sätt jättehögt (ignorerar gränsen)
            out.min = hasMin ? minStr.toDouble(): 0.0;
            out.max = hasMax ? maxStr.toDouble(): 99999999.0;
        } else {
            out.active = false; // Ignorera helt om båda är tomma
        }
    };

    // Fyll i filtret rad för rad
    capture(m_distMin, m_distMax, filter.distance);
    capture(m_tempMin, m_tempMax, filter.temperature);
    capture(m_massMin, m_massMax, filter.mass);
    capture(m_diamMin, m_diamMax, filter.diameter);

    // Spektralklasser (Krav: Inga valda = inga stjärnor)
    filter.spectralActive = true;
    filter.selectedSpectralClasses.clear();
    for (QCheckBox *cb : m_specChecks) {
        if (cb->isChecked()) {
            filter.selectedSpectralClasses.append(cb->text());
        }
    }

    emit filterChanged(filter);
}
void FilterPanel::onResetButtonClicked() {
    m_distMin->clear(); m_distMax->clear();
    m_tempMin->clear(); m_tempMax->clear();
    m_massMin->clear(); m_massMax->clear();
    m_diamMin->clear(); m_diamMax->clear();

    for (QCheckBox *cb : m_specChecks)
        cb->setChecked(true);

    emit filterChanged(StarFilter{});
}
