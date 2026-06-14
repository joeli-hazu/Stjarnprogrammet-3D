#include <QtTest>
#include "../auth/login.h"
#include "../auth/registerpage.h"
#include "../database/favoriteRepository.h"
#include "controller.h"
#include "models/StarFilter.h"
#include "../core/filterengine.h"
#include "../utils/csvimporter.h"
#include "../database/starrepository.h"
#include "widgets/filterpanel.h"
#include "../view/3d/star3dview.h"
#include "../view/3d/starentity.h"
#include "../view/3d/starobject.h"
#include "widgets/searchpanel.h"

#include <QSignalSpy>
#include <QFile>
#include <QTextStream>

#include <QLineEdit>
#include <QtSql>
#include <QPoint>
#include <QListWidgetItem>
#include <QListWidget>

class TestAll : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testIsFavorite();
    void testRegisterUser();
    void testLoginAdmin();
    void testLoginUser();

    void testFilterEngineDeep();
    void testFilterPanelInput();
    void testCsvImport();
    void testStarRepositoryIntegration();

    // ---------- 3D View och StarEntity ----------
    void testStarVisualRadius();
    void testStarEntityData();
    void testStarEntityTransform();
    void testStarEntityViewLocked();
    void testStar3DViewResetSignal();
    void testStar3DViewFlyToExistingStar();
    void testStar3DViewFlyToInvalidStar();

    // ---------- SÖK --------------
    void testSearchPanelInit();
    void testSearchPanelEmptyInput();
    void testSearchPanelSuccessfulSearch();
    void testSearchPanelNoMatch();
    void testSearchPanelItemClickedSignal();
    void testSearchPanelUpdateSearchList();
};

// ---------- GLOBAL TEST DB ----------
void TestAll::initTestCase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    QVERIFY(db.open());

    QSqlQuery q;

    // USERS
    QVERIFY(q.exec("CREATE TABLE users (id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT, password_hash TEXT)"));

    // FAVORITES
    QVERIFY(q.exec("CREATE TABLE favorites (user_id INTEGER, star_id INTEGER)"));

    //STARS
    QVERIFY(q.exec(
        "CREATE TABLE stars ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "main_id TEXT,"
        "ra REAL,"
        "dec REAL,"
        "distance_ly REAL,"
        "x REAL,"
        "y REAL,"
        "z REAL,"
        "spectral_type TEXT,"
        "diameter_solar REAL,"
        "diameter_km REAL,"
        "temperature REAL,"
        "mass REAL)"
        ));
}

void TestAll::cleanupTestCase()
{
    QSqlDatabase::database().close();
}

// ---------- FavoriteRepository ----------
void TestAll::testIsFavorite()
{
    FavoriteRepository repo;
    repo.addFavorite(1, 10);

    QVERIFY(repo.isFavorite(1, 10));
    QVERIFY(!repo.isFavorite(1, 99));
}

// ---------- Register (Controller via UI) ----------
void TestAll::testRegisterUser()
{
    Controller c;

    bool result = c.registreraAnvandare("testuser", "1234");
    QVERIFY(result);

}

// ---------- Login ADMIN ----------
void TestAll::testLoginAdmin()
{
    Login login;

    auto name = login.findChild<QLineEdit*>("Log_lineEdit_Name");
    auto pass = login.findChild<QLineEdit*>("Log_lineEdit_Los");

    QVERIFY(name);
    QVERIFY(pass);

    name->setText("admin");
    pass->setText("admin123");

    QMetaObject::invokeMethod(&login, "on_login_clicked");

    QCOMPARE(login.result(), (int)QDialog::Accepted);
}

// ---------- Login USER ----------
void TestAll::testLoginUser()
{
    Controller c;
    c.registreraAnvandare("user1", "pass1");

    Login login;

    auto name = login.findChild<QLineEdit*>("Log_lineEdit_Name");
    auto pass = login.findChild<QLineEdit*>("Log_lineEdit_Los");

    name->setText("user1");
    pass->setText("pass1");

    QMetaObject::invokeMethod(&login, "on_login_clicked");

    QCOMPARE(login.result(), (int)QDialog::Accepted);
}

// ---------- Filter ----------
void TestAll::testFilterEngineDeep() {
    StarFilter filter;

    /** * Verifierar att query-byggaren inte lägger till onödiga SQL-villkor
     * när inga filter har aktiverats av användaren.
     */
    QString baseQuery = FilterEngine::buildQuery(filter);
    QVERIFY(baseQuery.contains("SELECT id, main_id"));
    QVERIFY(!baseQuery.contains("AND mass"));

    /** * Verifierar att numeriska filter översätts korrekt till SQL BETWEEN-satser.
     * Säkerställer att punkt används som decimaltecken oavsett systemets språkinställning.
     */
    filter.mass.active = true;
    filter.mass.min = 0.5;
    filter.mass.max = 10.0;
    QString massQuery = FilterEngine::buildQuery(filter);
    QVERIFY(massQuery.contains("mass BETWEEN 0.5000 AND 10.0000"));

    /** * Testar specialkravet: Om användaren avmarkerar alla spektralklasser
     * ska inga stjärnor visas. Detta löses med det logiska villkoret AND 1=0.
     */
    filter.spectralActive = true;
    filter.selectedSpectralClasses.clear();
    QString emptySpec = FilterEngine::buildQuery(filter);
    QVERIFY(emptySpec.contains("AND 1=0"));
}

// ---------- UI-mappning och Signaler ----------
void TestAll::testFilterPanelInput() {
    FilterPanel panel;

    // Använder QSignalSpy för att fånga upp signalen som skickas när man klickar på Använd Filter
    QSignalSpy spy(&panel, &FilterPanel::filterChanged);

    QLineEdit* distMin = panel.findChild<QLineEdit*>("m_distMin");

    if(distMin) {
        /** * Testar robusthet vid inmatning: Kontrollerar att koden kan konvertera
         * strängar med kommatecken (t.ex "12,5") till korrekta flyttal (12.5).
         */
        distMin->setText("12,5");

        // Simulerar programmets logikflöde vid knapptryck
        QMetaObject::invokeMethod(&panel, "onApplyButtonClicked");

        // Kontrollera att exakt en signal skickades och att värdet konverterats rätt
        QCOMPARE(spy.count(), 1);
        StarFilter receivedFilter = qvariant_cast<StarFilter>(spy.at(0).at(0));
        QCOMPARE(receivedFilter.distance.min, 12.5);
    }
}

// ---------- Dataimport (Integrationstest) ----------
void TestAll::testCsvImport() {
    /** * Skapar en temporär virtuell CSV-fil för att testa importfunktionen.
     * Innehåller en korrekt rad och en trasig rad för att testa felhantering.
     */
    QString testCsvContent =
        "id,main_id,ra,dec,dist,x,y,z,spec,diam_s,diam_k,temp,mass\n"
        "1,\"Sirius\",0,0,8.6,1,1,1,\"A1V\",1.7,2300000,9940,2.1\n"
        "2,\"KnasigRad\",0,0,0\n";

    QString tempFile = "test_stars.csv";
    QFile file(tempFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << testCsvContent;
        file.close();
    }

    /** * Verifierar att CsvImporter korrekt identifierar och importerar giltiga rader
     * men hoppar över rader som saknar nödvändiga kolumner utan att krascha.
     */
    CsvImporter importer;
    int importedCount = importer.importStars(tempFile);

    // Endast den första raden ska ha godkänts
    QCOMPARE(importedCount, 1);

    // Rensar upp temporärfilen efter avslutat test
    QFile::remove(tempFile);
}

// ---------- Repository och Databas ----------
void TestAll::testStarRepositoryIntegration() {
    /** * Testar mappningen mellan SQL-databasen och C++-objektet Star.
     * Säkerställer att datatyperna (double, QString, int) bevaras korrekt vid hämtning.
     */
    QSqlQuery q;
    q.prepare("INSERT INTO stars (main_id, temperature, spectral_type) VALUES (:name, :temp, :spec)");
    q.bindValue(":name", "TestStar");
    q.bindValue(":temp", 5500.0);
    q.bindValue(":spec", "G2V");
    QVERIFY(q.exec());

    StarRepository repo;
    QList<Star> results = repo.searchByName("TestStar");

    // Validerar att attributen i det returnerade objektet matchar de insatta värdena
    QCOMPARE(results.size(), 1);
    QCOMPARE(results[0].main_id, QString("TestStar"));
    QCOMPARE(results[0].temperature, 5500.0);
}

// ---------- 3D View och StarEntity ----------
void TestAll::testStarVisualRadius()
{
    QCOMPARE(StarVisual::radiusFromSize(-10.0f), 1.0f);
    QCOMPARE(StarVisual::radiusFromSize(0.0f), 1.0f);

    QVERIFY(StarVisual::radiusFromSize(10.0f) > 1.0f);
    QVERIFY(StarVisual::radiusFromSize(10.0f) <= 6.0f);

    QCOMPARE(StarVisual::radiusFromSize(1000000.0f), 6.0f);
}

void TestAll::testStarEntityData()
{
    StarObject star;
    star.id = 42;
    star.name = "Sirius";
    star.position = QVector3D(1.0f, 2.0f, 3.0f);
    star.size = 10.0f;
    star.color = Qt::white;

    StarEntity entity(star);

    QCOMPARE(entity.id(), 42);
    QCOMPARE(entity.data().name, QString("Sirius"));
    QCOMPARE(entity.data().position, QVector3D(1.0f, 2.0f, 3.0f));
}

void TestAll::testStarEntityTransform()
{
    StarObject star;
    star.id = 1;
    star.name = "Vega";
    star.position = QVector3D(10.0f, 20.0f, 30.0f);
    star.size = 5.0f;
    star.color = Qt::yellow;

    StarEntity entity(star);

    QVERIFY(entity.transform() != nullptr);
    QCOMPARE(entity.transform()->translation(), QVector3D(10.0f, 20.0f, 30.0f));
}

void TestAll::testStarEntityViewLocked()
{
    StarObject star;
    star.id = 1;
    star.name = "TestStar";
    star.position = QVector3D(0.0f, 0.0f, 0.0f);
    star.size = 5.0f;
    star.color = Qt::white;

    StarEntity entity(star);

    QCOMPARE(entity.isViewLocked(), false);

    entity.setViewLocked(true);
    QCOMPARE(entity.isViewLocked(), true);

    entity.setViewLocked(false);
    QCOMPARE(entity.isViewLocked(), false);
}

void TestAll::testStar3DViewResetSignal()
{
    Star3DView view;

    QSignalSpy spy(&view, &Star3DView::viewReset);

    view.resetView();

    QCOMPARE(spy.count(), 1);
}

void TestAll::testStar3DViewFlyToExistingStar()
{
    qRegisterMetaType<StarObject>("StarObject");

    Star3DView view;

    StarObject star;
    star.id = 7;
    star.name = "Vega";
    star.position = QVector3D(1.0f, 2.0f, 3.0f);
    star.size = 5.0f;
    star.color = Qt::white;

    view.setStars({ star });

    QSignalSpy spy(&view, &Star3DView::starClicked);

    view.flyToStarById(7);

    QCOMPARE(spy.count(), 1);

    StarObject received = qvariant_cast<StarObject>(spy.at(0).at(0));

    QCOMPARE(received.id, 7);
    QCOMPARE(received.name, QString("Vega"));
}

void TestAll::testStar3DViewFlyToInvalidStar()
{
    qRegisterMetaType<StarObject>("StarObject");

    Star3DView view;

    StarObject star;
    star.id = 3;
    star.name = "Altair";
    star.position = QVector3D(5.0f, 5.0f, 5.0f);
    star.size = 5.0f;
    star.color = Qt::white;

    view.setStars({ star });

    QSignalSpy spy(&view, &Star3DView::starClicked);

    view.flyToStarById(999);

    QCOMPARE(spy.count(), 0);
}
// ---------- 3D View och StarEntity ----------



// ---------- SÖK --------------

void TestAll::testSearchPanelInit()
{
    SearchPanel panel;

    // Hitta interna komponenter via findChild
    auto input = panel.findChild<QLineEdit*>();
    auto list = panel.findChild<QListWidget*>();

    QVERIFY(input);
    QVERIFY(list);
    QCOMPARE(input->text(), QString(""));
    QCOMPARE(list->isVisible(), false);
}


void TestAll::testSearchPanelEmptyInput()
{
    SearchPanel panel;
    auto input = panel.findChild<QLineEdit*>();
    auto list = panel.findChild<QListWidget*>();

    // Simulera inmatning och trigga förändring
    input->setText("   ");
    QMetaObject::invokeMethod(&panel, "onTextChanged", Q_ARG(QString, input->text()));

    QCOMPARE(list->isVisible(), false);
    QCOMPARE(list->count(), 0);
}



void TestAll::testSearchPanelSuccessfulSearch()
{
    // Förbered testdata i databasen
    QSqlQuery q;
    q.prepare("INSERT INTO stars (main_id, ra, dec, distance_ly) VALUES (:name, 0, 0, 10.0)");
    q.bindValue(":name", "Vega");
    QVERIFY(q.exec());

    SearchPanel panel;
    auto input = panel.findChild<QLineEdit*>();
    auto list = panel.findChild<QListWidget*>();

    // Simulera att användaren skriver "Veg"
    input->setText("Veg");
    QMetaObject::invokeMethod(&panel, "onTextChanged", Q_ARG(QString, input->text()));

    // Kontrollera att listan poppar upp och innehåller vår stjärna
    QCOMPARE(list->isVisible(), true);
    QCOMPARE(list->count(), 1);
    QCOMPARE(list->item(0)->text(), QString("Vega"));

    // Städa databasen
    q.exec("DELETE FROM stars WHERE main_id = 'Vega'");
}



void TestAll::testSearchPanelNoMatch()
{
    SearchPanel panel;
    auto input = panel.findChild<QLineEdit*>();
    auto list = panel.findChild<QListWidget*>();

    input->setText("DennaStjärnaFinnsInte123");
    QMetaObject::invokeMethod(&panel, "onTextChanged", Q_ARG(QString, input->text()));

    QCOMPARE(list->isVisible(), false);
}


void TestAll::testSearchPanelItemClickedSignal()
{
    // Förbered testdata
    QSqlQuery q;
    q.prepare("INSERT INTO stars (main_id, distance_ly) VALUES ('Altair', 16.7)");
    QVERIFY(q.exec());

    qRegisterMetaType<Star>("Star");
    SearchPanel panel;

    // Sätt upp spion på signalen starSelected
    QSignalSpy spy(&panel, &SearchPanel::starSelected);

    auto input = panel.findChild<QLineEdit*>();
    auto list = panel.findChild<QListWidget*>();

    // Utför sökning så att listan populeras
    input->setText("Altair");
    QMetaObject::invokeMethod(&panel, "onTextChanged", Q_ARG(QString, input->text()));

    QCOMPARE(list->count(), 1);
    QListWidgetItem *item = list->item(0);

    // Simulera klick på listobjektet
    QMetaObject::invokeMethod(&panel, "onItemClicked", Q_ARG(QListWidgetItem*, item));

    // Verifiera att exakt 1 signal skickades iväg
    QCOMPARE(spy.count(), 1);

    // Kontrollera att rätt stjärna skickades med i signalen
    Star receivedStar = qvariant_cast<Star>(spy.at(0).at(0));
    QCOMPARE(receivedStar.main_id, QString("Altair"));

    // Kontrollera återställning av gränssnittet
    QCOMPARE(list->isVisible(), false);
    QCOMPARE(input->text(), QString(""));

    // Städa databasen
    q.exec("DELETE FROM stars WHERE main_id = 'Altair'");
}



void TestAll::testSearchPanelUpdateSearchList()
{
    SearchPanel panel;
    auto list = panel.findChild<QListWidget*>();

    QStringList nyaNamn;
    nyaNamn << "Stjärna A" << "Stjärna B" << "Stjärna C";

    // Kör funktionen direkt
    panel.updateSearchList(nyaNamn);

    // Kontrollera att listan uppdaterats korrekt
    QCOMPARE(list->count(), 3);
    QCOMPARE(list->item(0)->text(), QString("Stjärna A"));
    QCOMPARE(list->item(2)->text(), QString("Stjärna C"));
}

QTEST_MAIN(TestAll)
#include "tst_all.moc"
