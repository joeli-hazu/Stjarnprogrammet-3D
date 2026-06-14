# StellarView 3D / Stjärnprogrammet

An interactive 3D star visualization system built with C++ and Qt, featuring astronomical data integration, search capabilities, and multi-dimensional filtering. Developed as part of the Software Engineering course (DT507G) at Örebro University.

*Scrolla ner för den svenska versionen / Scroll down for the Swedish version.*

---

## English Version

### 🚀 Features
* **3D Star Visualization:** Renders star objects in a dynamic 3D environment with camera navigation and a dedicated "star perspective" camera lock.
* **Smart Search Engine:** Fast, optimized partial and exact text-matching against the star database.
* **Multi-Dimensional Filtering:** Real-time filtering based on mass, temperature, distance (0–100 light-years), and spectral class, using alpha-blending transparency (dimming) for filtered-out stars.
* **User Authentication & Favorites:** Secure login validation system with custom SQL database tables managing persistent user accounts and favorite lists via a many-to-many relationship.
* **Dynamic Info Panel:** Instant detailed telemetry display (mass, temperature, diameter, etc.) upon clicking any 3D star object using Qt's signal-slot mechanisms.

### 🛠️ Architecture & Technologies
* **Language:** C++
* **Framework:** Qt Creator (Qt 3D, QTest)
* **Database:** SQL / SQLite (Data aggregated and cross-referenced from professional Simbad and Gaia astronomical catalogs)
* **Methodology:** Agile Scrum with strict Git/GitHub branch management, peer code reviews, and automated Unit Testing.

### 📂 Repository Structure
* `/src` - C++ source code and header files.
* `/database` - Database initialization scripts, ER-diagrams, and `Stars.csv` parsing logic.
* `/tests` - Automated unit tests utilizing the `QTest` framework.

---

## Svensk version

### 🚀 Funktioner
* **3D-Visualisering:** Renderar stjärnor i en dynamisk 3D-miljö med kameranavigering och ett "stjärnperspektiv" som låser kameran vid valda objekt.
* **Smart Sökmotor:** Snabb och optimerad sökfunktion med partiell och exakt textmatchning mot databasen.
* **Flerdimensionell Filtrering:** Realtidsfiltrering baserad på massa, temperatur, avstånd (0–100 ljusår) och spektralklass, där bortfiltrerade stjärnor görs genomskinliga (dimmas ut) i realtid.
* **Inloggning & Favorithantering:** Säkert inloggningssystem med SQL-databastabeller som hanterar användarkonton och personliga favoritlistor via en stabil många-till-många-relation.
* **Dynamisk Informationspanel:** Visar direkt stjärnans detaljerade mätdata (massa, temperatur, diameter, etc.) när användaren klickar på ett objekt i 3D-vyn med hjälp av Qt:s signal-slot-mekanismer.

### 🛠️ Arkitektur & Tekniker
* **Språk:** C++
* **Ramverk:** Qt Creator (Qt 3D, QTest)
* **Databas:** SQL / SQLite (Data sammanställd och kombinerad från de professionella astronomiska katalogerna Simbad och Gaia)
* **Metodik:** Agilt arbetssätt (Scrum) med strikt hantering av Git-grenar, kodgranskningar och automatiserade enhetstester.

### 📂 Projektstruktur
* `/src` - C++ källkod och header-filer.
* `/database` - Databasfiler, ER-diagram och inläsningslogik för `Stars.csv`.
* `/tests` - Automatiserade enhetstester utvecklade i `QTest`-ramverket.

---
