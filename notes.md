# Note de lucru

## Unde am ajuns
- Repo-ul e pus la punct, CI-ul ruleaza testele si publica imaginea in GHCR.
- Validarea campurilor e gata (nume, email, telefon, categorie), cu teste.
- Clasa Contact e gata, cu teste.
- ContactBook e gata, cu teste: email unic, id-uri de la 1, filtrare, sortare, reset.
- Urmeaza rutele din main.cpp.
- Mai am de completat pagina de start, LABEL-ul din Dockerfile si badge-ul in README.

## Decizii
- Validatorii intorc std::optional<std::string> in loc de bool. Asa am direct mesajul de eroare pentru raspunsul 400.
- La nume numar caractere, nu octeti. Un nume cu diacritice are mai multi octeti decat litere si ar fi fost respins degeaba.
- Logica e in headere separate de main.cpp, ca sa o pot testa fara server.
- Contact nu are setteri, pentru ca un contact nu se editeaza dupa creare. Nici nu valideaza, doar tine datele.
- Emailul se pastreaza cum a fost scris. Pentru comparatie folosesc o copie cu litere mici.
- In ContactBook, verificarea emailului si adaugarea sunt sub acelasi lock, altfel doua cereri simultane cu acelasi email ar trece amandoua.
- Find intoarce o copie, nu o referinta in vector.
- Un contact respins nu consuma un id, iar id-urile sterse nu se refolosesc.
- Lista e sortata dupa nume, la egalitate dupa id.

## De lamurit
- Cautarea dupa q ignora majusculele doar la litere fara diacritice.
- La /stats, by_category are acum doar categoriile folosite. De vazut daca trebuie toate patru, cu 0.
- O categorie invalida la GET /contacts intoarce lista goala, nu 400.

## Probleme
- prima rulare de CI a picat: fisierul de teste ajunsese in tests/tests/ si CMake nu il gasea. L-am mutat in tests/.
- contacts.hpp a ajuns gol intr-un commit fara sa observ. CI-ul a picat abia cand testele pentru ContactBook l-au inclus (03fd9f2). L-am restaurat in 8cfdd75.