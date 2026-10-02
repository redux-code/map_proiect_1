# Note proiect MAP

## Pe unde sunt
- Am repo-ul, CI-ul merge (test + build verzi) si imaginea e publicata in GHCR, publica.
- Am facut validarea pentru contact (nume, email, telefon, categorie) si testele ei.
- Urmeaza clasele Contact si ContactBook, dupa aia rutele.
- Mai am de completat: HomePage() din app.hpp, LABEL din Dockerfile, badge-ul CI in README si foaia de inscriere.

## Decizii pe care le-am luat
- Validatorii intorc std::optional<std::string> in loc de bool, ca sa pot trimite direct mesajul de eroare in raspunsul 400.
- La nume numar caractere, nu octeti. Un "ă" are 2 octeti in UTF-8 si altfel un nume cu diacritice ar fi respins degeaba.
- Logica e in fisiere .hpp separate, nu in main.cpp, ca sa o pot testa fara sa pornesc serverul.

## Probleme pe care le-am avut
- CI-ul a picat la prima rulare: fisierul test_contacts.cpp ajunsese in tests/tests/ si CMake nu il gasea. L-am mutat in tests/ si a mers. (commit: de pus SHA-ul)

## De tinut minte
- Commit mic, la cateva zile, nu totul intr-o seara.
- Codul si comentariile in engleza, README si lucrarea in romana.
- Ce nu pot explica singur, nu raman in proiect.