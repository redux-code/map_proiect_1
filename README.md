# Agenda de contacte

![CI](https://github.com/redux-code/map_proiect_1/actions/workflows/ci.yml/badge.svg)

Proiect individual la disciplina Metode avansate de programare, anul universitar 2026-2027.

## Autor

- **Nume:** Museat Mihai-Ionut
- **Grupa:** 2.1
- **Marca:** LH615677
- **Tema:** 1 - Agenda de contacte

## Descriere

Serviciu web care functioneaza ca o agenda de contacte: se adauga contacte cu nume, email, telefon si categorie, se cauta dupa nume, se filtreaza pe categorie si se sterg. Datele se tin in memorie, iar aplicatia ruleaza intr-un container Docker publicat automat in GitHub Container Registry.

## Tehnologii

- C++20, cpp-httplib, nlohmann/json
- doctest pentru teste, CMake pentru build
- Docker (build multi-stage), imagine publicata in GHCR
- GitHub Actions: testele ruleaza la fiecare push, imaginea se publica doar daca trec

## Rulare

Din imaginea publicata:

```
docker run -d -p 8080:8080 ghcr.io/redux-code/map_proiect_1:latest
```

Sau construita local:

```
docker build -t map-proiect .
docker run -d -p 8080:8080 map-proiect
```

Aplicatia asculta pe portul 8080. Verificati:

```
curl http://localhost:8080/health
curl http://localhost:8080/version
```

## Testare

```
cmake -B build -DBUILD_TESTS=ON
cmake --build build -j
./build/tests
```

## Rutele implementate

| Ruta | Metoda | Descriere |
|---|---|---|
| `/health` | GET | Starea serviciului |
| `/version` | GET | Versiunea si commit-ul din care a fost construita imaginea |
| `/` | GET | Pagina de prezentare |
| `/reset` | POST | Goleste datele din memorie |
| `/contacts` | POST | Adauga un contact (in curs de implementare) |
| `/contacts` | GET | Listeaza contactele, cu filtre `category` si `q` (in curs de implementare) |
| `/contacts/{id}` | GET | Intoarce un contact (in curs de implementare) |
| `/contacts/{id}` | DELETE | Sterge un contact (in curs de implementare) |
| `/stats` | GET | Numarul de contacte, pe categorii (in curs de implementare) |

## Decizii de implementare

- **Validarea intoarce mesajul de eroare, nu doar adevarat/fals.** Fiecare validator intoarce `std::optional<std::string>`: gol daca valoarea e buna, altfel mesajul care ajunge in raspunsul 400.
- **Lungimea numelui se masoara in caractere, nu in octeti.** In UTF-8 o litera cu diacritice ocupa doi octeti, iar o numaratoare pe octeti ar respinge nume valide.
- **Logica e separata de server.** Regulile stau in headere proprii (`contacts.hpp`, `contact.hpp`, `contact_book.hpp`), iar `main.cpp` doar leaga rutele HTTP de ele. Asa se pot testa fara sa pornesc serverul.
- **Emailul se pastreaza exact cum a fost scris.** Unicitatea se verifica pe o copie cu litere mici, dar la citire se intoarce forma originala.
- **Verificarea emailului si adaugarea se fac atomic.** Serverul trateaza cereri in paralel, asa ca `ContactBook` verifica emailul si adauga contactul sub acelasi lock. Altfel doua cereri simultane cu acelasi email ar putea trece amandoua.
- **Id-urile sunt atribuite de server si nu se refolosesc.** Un contact respins nu consuma un id, iar dupa stergere numerotarea continua. Dupa `/reset` reincepe de la 1.