#include "adminpanel.h"
#include "qlabel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QSqlQuery>
#include <QMessageBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QSqlError>
#include <QComboBox>
#include <QLineEdit>

AdminPanel::AdminPanel(QWidget *parent)
    : QDialog(parent)
{
    setModal(true);
    setWindowTitle("Admin Panel");
    resize(400, 500);

    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Användare:", this));

    m_userList = new QListWidget(this);
    layout->addWidget(m_userList);

    m_deleteUserBtn = new QPushButton("Ta bort användare", this);
    m_changePassBtn = new QPushButton("Ändra lösenord", this);
    m_deleteUserBtn->setEnabled(false);
    m_changePassBtn->setEnabled(false);

    layout->addWidget(m_deleteUserBtn);
    layout->addWidget(m_changePassBtn);
    connect(m_userList, &QListWidget::itemClicked, this,
            [this](QListWidgetItem *) {
                m_deleteUserBtn->setEnabled(true);
                m_changePassBtn->setEnabled(true);
            });



    connect(m_changePassBtn, &QPushButton::clicked, this, [this]() {
        QListWidgetItem *item = m_userList->currentItem();
        if (!item) return;

        QString username = item->text();
        bool ok;
        QString newPass = QInputDialog::getText(this, "Nytt lösenord",
                                                "Ange nytt lösenord för " + username + ":",
                                                QLineEdit::Password, "", &ok);

        if (ok && !newPass.isEmpty()) {
            QSqlQuery q;
            q.prepare("UPDATE users SET password_hash = :p WHERE username = :u");
            q.bindValue(":p", newPass);
            q.bindValue(":u", username);

            if (!q.exec()) {
                QMessageBox::critical(this, "Fel", "Kunde inte ändra lösenord");
            } else {
                QMessageBox::information(this, "Klar", "Lösenordet har uppdaterats för " + username);
            }
        }
    });




    QLineEdit *searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("Sök stjärna...");
    layout->addWidget(searchEdit);

    m_list = new QListWidget(this);
    layout->addWidget(m_list);

    QHBoxLayout *btnLayout = new QHBoxLayout();

    m_addBtn = new QPushButton("Lägg till stjärna", this);
    m_editBtn = new QPushButton("Redigera stjärna", this);
    m_deleteBtn = new QPushButton("Ta bort stjärna", this);


    btnLayout->addWidget(m_addBtn);
    btnLayout->addWidget(m_editBtn);
    btnLayout->addWidget(m_deleteBtn);

    layout->addLayout(btnLayout);

    layout->addStretch();

    layout->addStretch(); // Skapar mellanrum så knappen hamnar längst ner
    QPushButton *closeBtn = new QPushButton("Stäng", this);
    closeBtn->setMinimumHeight(40);
    layout->addWidget(closeBtn);

    loadStars();
    loadUsers();


    connect(closeBtn, &QPushButton::clicked, this, &AdminPanel::reject);


    connect(m_deleteUserBtn, &QPushButton::clicked, this, [this]() {
        QListWidgetItem *item = m_userList->currentItem();
        if (!item) return;

        QString username = item->text();

        QMessageBox::StandardButton svar;
        svar = QMessageBox::question(
            this,
            "Bekräfta borttagning",
            "Är du säker på att användaren " + username + " ska tas bort?",
            QMessageBox::Ok | QMessageBox::Cancel
            );

        if (svar == QMessageBox::Ok) {
            QSqlQuery q;
            q.prepare("DELETE FROM users WHERE username = :u");
            q.bindValue(":u", username);

            if (!q.exec()) {
                QMessageBox::warning(this, "Fel", "Kunde inte ta bort användaren.");
                return;
            }

            loadUsers();

            m_deleteUserBtn->setEnabled(false);
            m_changePassBtn->setEnabled(false);
        }
    });

    connect(searchEdit, &QLineEdit::textChanged, this,
            [this](const QString &text)
            {
                for (int i = 0; i < m_list->count(); ++i) {
                    QListWidgetItem *item = m_list->item(i);
                    bool match = item->text().contains(text, Qt::CaseInsensitive);
                    item->setHidden(!match);
                }
            });


    connect(m_addBtn, &QPushButton::clicked, this, [this]() {
        QDialog addDlg(this);
        addDlg.setWindowTitle("Lägg till ny stjärna");

        QFormLayout *form = new QFormLayout(&addDlg);

        QLineEdit *nameEdit = new QLineEdit();
        QDoubleSpinBox *xEdit = new QDoubleSpinBox();
        QDoubleSpinBox *yEdit = new QDoubleSpinBox();
        QDoubleSpinBox *zEdit = new QDoubleSpinBox();
        QComboBox *specEdit = new QComboBox();
        specEdit->addItems({"Välj", "B", "A", "F", "G", "K", "M"});
        specEdit->setCurrentIndex(0);


        QDoubleSpinBox *diamEdit = new QDoubleSpinBox();
        QDoubleSpinBox *distEdit = new QDoubleSpinBox();
        QDoubleSpinBox *tempEdit = new QDoubleSpinBox();
        QDoubleSpinBox *massEdit = new QDoubleSpinBox();

        QLocale sweden(QLocale::Swedish, QLocale::Sweden);
        QList<QDoubleSpinBox*> spinBoxes = {xEdit, yEdit, zEdit, diamEdit, distEdit, tempEdit, massEdit};

        for(auto spin : spinBoxes) {
            spin->setLocale(sweden); // Gör att boxen förstår svenska inställningar (komma/punkt)
            spin->setRange(-1000000.0, 1000000.0);
            spin->setSpecialValueText(" "); // Visar tomt vid värde 0
        }

        xEdit->setRange(-1000000.0, 1000000.0);
        yEdit->setRange(-1000000.0, 1000000.0);
        zEdit->setRange(-1000000.0, 1000000.0);

        diamEdit->setRange(0.00, 10000.0);
        distEdit->setRange(0.0, 1000000.0);

        tempEdit->setRange(0, 200000);
        tempEdit->setDecimals(0);

        massEdit->setRange(0.0, 2000.0);


        xEdit->setValue(xEdit->minimum());
        yEdit->setValue(yEdit->minimum());
        zEdit->setValue(zEdit->minimum());
        diamEdit->setValue(0.0);
        distEdit->setValue(0.0);
        tempEdit->setValue(0.0);
        massEdit->setValue(0.0);

        form->addRow("Namn (ID):", nameEdit);
        form->addRow("X-koordinat:", xEdit);
        form->addRow("Y-koordinat:", yEdit);
        form->addRow("Z-koordinat:", zEdit);
        form->addRow("Spektralklass (B, A, F, G, K, M):", specEdit);
        form->addRow("Diameter (solradier):", diamEdit);
        form->addRow("Avstånd (ljusår):", distEdit);
        form->addRow("Temperatur (K):", tempEdit);
        form->addRow("Massa (solmassa):", massEdit);



        QLabel *errorLabel = new QLabel("");
        errorLabel->setStyleSheet("color: red; font-weight: bold;");
        form->addRow(errorLabel);

        QDialogButtonBox *buttons =
            new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

        form->addRow(buttons);


        connect(buttons->button(QDialogButtonBox::Ok), &QPushButton::clicked, [&]() {
            if (nameEdit->text().trimmed().isEmpty() || specEdit->currentText() == "Välj" ||
                diamEdit->value() == 0.0 || distEdit->value() == 0.0 ||
                tempEdit->value() == 0.0 || massEdit->value() == 0.0) {
                errorLabel->setText("Fel: Alla fält måste fyllas i!");
            } else {
                addDlg.accept();
            }
        });
        connect(buttons, &QDialogButtonBox::rejected, &addDlg, &QDialog::reject);

        if (addDlg.exec() == QDialog::Accepted) {
            if (nameEdit->text().trimmed().isEmpty() ||
                specEdit->currentText() == "Välj" ||
                diamEdit->value() == 0.0 ||
                distEdit->value() == 0.0 ||
                tempEdit->value() == 0.0 ||
                massEdit->value() == 0.0) {

                QMessageBox::warning(this, "Fel", "Du måste fylla i alla fält innan stjärnan kan läggas till.");
                return;
            }

            QSqlQuery checkQ;
            checkQ.prepare("SELECT COUNT(*) FROM stars WHERE LOWER(main_id) = LOWER(:name)");
            checkQ.bindValue(":name", nameEdit->text().trimmed());

            if (!checkQ.exec() || !checkQ.next()) {
                QMessageBox::warning(this, "Fel", "Kunde inte kontrollera om namnet redan finns.");
                return;
            }

            if (checkQ.value(0).toInt() > 0) {
                QMessageBox::warning(this, "Fel", "Det finns redan en stjärna med detta namn. Välj ett annat namn.");
                return;
            }

            QSqlQuery q;
            q.prepare("INSERT INTO stars "
                      "(main_id, spectral_type, diameter_solar, distance_ly, temperature, mass, x, y, z) "
                      "VALUES "
                      "(:name, :spec, :diam, :dist, :temp, :mass, :x, :y, :z)");

            q.bindValue(":name", nameEdit->text().trimmed());
            q.bindValue(":spec", specEdit->currentText());
            q.bindValue(":diam", diamEdit->value());
            q.bindValue(":dist", distEdit->value());
            q.bindValue(":temp", tempEdit->value());
            q.bindValue(":mass", massEdit->value());
            q.bindValue(":x", xEdit->value());
            q.bindValue(":y", yEdit->value());
            q.bindValue(":z", zEdit->value());

            if (!q.exec()) {
                QMessageBox::warning(this, "Fel", "Kunde inte skapa stjärnan: " + q.lastError().text());
            }

            loadStars();
        }
    });

    connect(m_deleteBtn, &QPushButton::clicked, this, [this]() {
        auto item = m_list->currentItem();
        if (!item) return;

        QString name = item->text();

        QMessageBox::StandardButton svar;
        svar = QMessageBox::question(
            this,
            "Bekräfta borttagning",
            "Är du säker på att stjärnan " + name + " ska tas bort?",
            QMessageBox::Ok | QMessageBox::Cancel
            );

        if (svar == QMessageBox::Ok) {
            QSqlQuery q;
            q.prepare("DELETE FROM stars WHERE main_id = :name");
            q.bindValue(":name", name);

            if (!q.exec()) {
                QMessageBox::warning(this, "Fel", "Kunde inte ta bort");
                return;
            }

            int currentRow = m_list->currentRow();

            loadStars();

            if (currentRow >= m_list->count()) {
                currentRow = m_list->count() - 1;
            }

            if (currentRow >= 0) {
                m_list->setCurrentRow(currentRow);
            }
        }
    });



    connect(m_editBtn, &QPushButton::clicked, this, [this]() {
        QListWidgetItem *item = m_list->currentItem();
        if (!item) return;

        QString oldName = item->text();

        // 1. Hämta nuvarande data så formuläret inte är tomt
        QSqlQuery query;
        query.prepare("SELECT main_id, spectral_type, diameter_solar, distance_ly, temperature, mass, x, y, z "
                      "FROM stars WHERE main_id = :name");
        query.bindValue(":name", oldName);

        if (!query.exec() || !query.next()) return;

        // 2. Skapa en popup-dialog med ett formulär
        QDialog editDlg(this);
        editDlg.setWindowTitle("Redigera stjärna: " + oldName);
        QFormLayout *form = new QFormLayout(&editDlg);

        QLineEdit *nameEdit = new QLineEdit(query.value("main_id").toString());
        QComboBox *specEdit = new QComboBox();
        specEdit->addItems({"Välj", "B", "A", "F", "G", "K", "M"});

        QString dbSpec = query.value("spectral_type").toString();


        QString firstLetter = dbSpec.left(1).toUpper();

        int index = specEdit->findText(firstLetter);
        if (index >= 0) {
            specEdit->setCurrentIndex(index);
        } else {
            specEdit->setCurrentIndex(0); // "Välj" om inget matchar
        }


        // --- ÄNDRING START: SAMMA LOCALE FÖR REDIGERA ---
        QLocale sweden(QLocale::Swedish, QLocale::Sweden);
        auto setupEditSpin = [&](QDoubleSpinBox* s, double val, double min, double max, int dec = 2) {
            s->setLocale(sweden);
            s->setRange(min, max);
            s->setDecimals(dec);
            s->setValue(val);
        };

        QDoubleSpinBox *diamEdit = new QDoubleSpinBox(); setupEditSpin(diamEdit, query.value("diameter_solar").toDouble(), 0.01, 10000.0);
        QDoubleSpinBox *distEdit = new QDoubleSpinBox(); setupEditSpin(distEdit, query.value("distance_ly").toDouble(), 0.0, 1000000.0);
        QDoubleSpinBox *tempEdit = new QDoubleSpinBox(); setupEditSpin(tempEdit, query.value("temperature").toDouble(), 0, 200000, 0);
        QDoubleSpinBox *massEdit = new QDoubleSpinBox(); setupEditSpin(massEdit, query.value("mass").toDouble(), 0.01, 2000.0);
        QDoubleSpinBox *xEdit = new QDoubleSpinBox(); setupEditSpin(xEdit, query.value("x").toDouble(), -1000000, 1000000);
        QDoubleSpinBox *yEdit = new QDoubleSpinBox(); setupEditSpin(yEdit, query.value("y").toDouble(), -1000000, 1000000);
        QDoubleSpinBox *zEdit = new QDoubleSpinBox(); setupEditSpin(zEdit, query.value("z").toDouble(), -1000000, 1000000);
        // --- ÄNDRING SLUT ---

        form->addRow("X-koordinat:", xEdit);
        form->addRow("Y-koordinat:", yEdit);
        form->addRow("Z-koordinat:", zEdit);
        form->addRow("Namn (ID):", nameEdit);
        form->addRow("Spektralklass:", specEdit);
        form->addRow("Diameter:", diamEdit);
        form->addRow("Avstånd:", distEdit);
        form->addRow("Temperatur:", tempEdit);
        form->addRow("Massa:", massEdit);

        QLabel *errorLabel = new QLabel("");
        errorLabel->setStyleSheet("color: red; font-weight: bold;");
        form->addRow(errorLabel);

        QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        form->addRow(buttons);

        connect(buttons->button(QDialogButtonBox::Ok), &QPushButton::clicked, [&]() {
            if (nameEdit->text().trimmed().isEmpty() || specEdit->currentText() == "Välj") {
                errorLabel->setText("Fel: Namn och Spektralklass krävs!");
            } else {
                editDlg.accept();
            }
        });
        connect(buttons, &QDialogButtonBox::rejected, &editDlg, &QDialog::reject);


        // 3. Om användaren trycker OK => Spara till databasen
        if (editDlg.exec() == QDialog::Accepted) {
            if (nameEdit->text().trimmed().isEmpty() || specEdit->currentText() == "Välj") {
                QMessageBox::warning(this, "Fel", "Du måste ange ett namn och välja en giltig spektralklass.");
                return;
            }

            QSqlQuery checkQ;
            checkQ.prepare(
                "SELECT COUNT(*) FROM stars "
                "WHERE LOWER(main_id) = LOWER(:name) "
                "AND LOWER(main_id) != LOWER(:oldName)"
                );

            checkQ.bindValue(":name", nameEdit->text().trimmed());
            checkQ.bindValue(":oldName", oldName);

            if (checkQ.exec() && checkQ.next()) {
                if (checkQ.value(0).toInt() > 0) {
                    QMessageBox::warning(this, "Fel", "Namnet är upptaget.");
                    return;
                }
            }

            QSqlQuery updateQ;
            updateQ.prepare("UPDATE stars SET "
                            "main_id = :name, "
                            "spectral_type = :spec, "
                            "diameter_solar = :diam, "
                            "distance_ly = :dist, "
                            "temperature = :temp, "
                            "mass = :mass, "
                            "x = :x, y = :y, z = :z "
                            "WHERE main_id = :oldName");

            updateQ.bindValue(":name", nameEdit->text());
            updateQ.bindValue(":spec", specEdit->currentText());
            updateQ.bindValue(":diam", diamEdit->value());
            updateQ.bindValue(":dist", distEdit->value());
            updateQ.bindValue(":temp", tempEdit->value());
            updateQ.bindValue(":mass", massEdit->value());
            updateQ.bindValue(":x", xEdit->value()); // NYTT
            updateQ.bindValue(":y", yEdit->value()); // NYTT
            updateQ.bindValue(":z", zEdit->value()); // NYTT
            updateQ.bindValue(":oldName", oldName);

            if (!updateQ.exec()) {
                QMessageBox::critical(this, "Fel", "Kunde inte uppdatera: " + updateQ.lastError().text());
            }
            loadStars();
        }


    });
}

void AdminPanel::loadStars()
{
    m_list->clear();

    QSqlQuery q("SELECT main_id FROM stars");

    while (q.next()) {
        m_list->addItem(q.value(0).toString());
    }
}

void AdminPanel::loadUsers()
{
    m_userList->clear();

    QSqlQuery q;
    // Vi använder exec() explicit för att kunna fånga upp eventuella felmeddelanden
    if (!q.exec("SELECT username FROM users")) {
        // Detta skriver ut det exakta databasfelet i din konsol
        qDebug() << "SQL-fel i loadUsers:" << q.lastError().text();
        return;
    }

    while (q.next()) {
        m_userList->addItem(q.value(0).toString());
    }
}

