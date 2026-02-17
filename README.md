# Smart Farmer (2D) — C++ / SFML

Smart Farmer este o adaptare 2D a jocului logic omonim. Scopul este să separi animalele în țarcuri folosind garduri, respectând regulile impuse de fiecare nivel.

## Obiectiv
- Fiecare tip de animal trebuie să fie într-un țarc separat
- Animalele de același tip trebuie să fie în același țarc
- Fiecare țarc trebuie să aibă acces la o sursă de apă
- Fiecare nivel are limită de timp

## Tehnologii folosite
- C++17
- SFML (Graphics, System, Audio)
- Niveluri încărcate din fișiere `.txt`

## Funcționalități
- Main Menu (Start / How to Play / Exit)
- Sistem de 60 niveluri
- Timer progresiv (30–90 secunde în funcție de nivel)
- Plasare și eliminare garduri pe grilă
- Validare automată a soluției
- Ecran Win / Lose (Next Level, Retry, Levels Menu)
- Sunet on/off + muzică de fundal
