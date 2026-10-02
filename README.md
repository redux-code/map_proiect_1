# Agenda de contacte

Proiect individual la disciplina Metode avansate de programare, anul universitar 2026-2027.

## Autor

- **Nume:** Museat Mihai-Ionut
- **Grupa:** 2.1
- **Marca:** LH615677
- **Tema:** 1 - Agenda de contacte

## Descriere

Serviciu web care functioneaza ca o agenda de contacte: se adauga contacte cu nume, email, telefon si categorie, se cauta dupa nume, se filtreaza pe categorie si se sterg. Datele se tin in memorie, iar aplicatia ruleaza intr-un container Docker publicat automat in GitHub Container Registry.

## Tehnologii

C++20 cu cpp-httplib si nlohmann/json

## Rulare

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

Se completeaza pe parcursul implementarii.