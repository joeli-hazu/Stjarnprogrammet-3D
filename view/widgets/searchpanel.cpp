#include "searchpanel.h"
#include "database/starrepository.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidgetItem>


SearchPanel::SearchPanel(QWidget *parent) : QWidget(parent)
{

    this->setFixedWidth(200);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 4, 0, 4);
    layout->setSpacing(6);


    // Sökfält
    m_input = new QLineEdit(this);
    m_input->setPlaceholderText("SÖK");
    m_input->setClearButtonEnabled(true);
    layout->addWidget(m_input);

    // resultatlista visar 8 rader, går att scrolla
    m_list = new QListWidget(this);
    m_list->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_list->setFixedHeight(14 * 20); // 8 rader á 26px
    m_list->setUniformItemSizes(true);

    m_list->setVisible(false);

    layout->addWidget(m_list);

    // Varje gång användaren skriver något => sök automatiskt
    connect(m_input, &QLineEdit::textChanged,
            this,    &SearchPanel::onTextChanged);

    // när användaren klickar på en stjärna i listan
    connect(m_list, &QListWidget::itemClicked,
            this,   &SearchPanel::onItemClicked);

    m_list->setFocusPolicy(Qt::NoFocus);

    m_list->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
}

void SearchPanel::onTextChanged(const QString &text)
{


    if (text.trimmed().isEmpty()) {
        m_list->setVisible(false);
        m_list->clear();
        return;
    }

    StarRepository repo;
    m_current = repo.searchByName(text.trimmed());


    if (!m_current.isEmpty()) {

        populateList(m_current);

        QPoint globalPos = m_input->mapToGlobal(QPoint(0, 0));
        m_list->move(globalPos.x(), globalPos.y() + m_input->height());
        m_list->setFixedWidth(m_input->width());
        m_list->setVisible(true);

        m_input->setFocus();

    } else {
        m_list->setVisible(false); // dölj om inga träffar finns
    }
}

void SearchPanel::onItemClicked(QListWidgetItem *item)
{
    // Hämta index som vi sparade
    int idx = item->data(Qt::UserRole).toInt();
    if (idx >= 0 && idx < m_current.size())
        emit starSelected(m_current[idx]); // skicka stjärnan till MainWindow

    m_list->setVisible(false);
    m_input->clear();
    m_input->clearFocus();
}

void SearchPanel::populateList(const QList<Star> &stars)
{
    m_list->clear();
    for (int i = 0; i < stars.size(); ++i) {
        QListWidgetItem *item = new QListWidgetItem(stars[i].main_id);
        item->setData(Qt::UserRole, i); // spara index så vi kan hämta Star bjektet
        m_list->addItem(item);
    }
}

void SearchPanel::updateSearchList(const QStringList &names) {
    if (!m_list) return;

    // 1. Rensa den nuvarande listan i sökpanelen
    m_list->clear();

    // 2. Lägg till de nya namnen i listan
    for (const QString &name : names) {
        m_list->addItem(name);
    }

    qDebug() << "Sökpanelens lista uppdaterad med" << names.size() << "stjärnor.";
}
