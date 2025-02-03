Projekt ten to aplikacja do harmonogramowania zadań, składająca się z dwóch głównych komponentów: interfejsu graficznego stworzonego w Pythonie za pomocą PyQt5 
oraz serwera backendowego napisanego w C++ wykorzystującego framework Crow. Dzięki zastosowaniu Docker, aplikacja jest konteneryzowana, co zapewnia spójne i przenośne środowisko uruchomieniowe.

Kluczowe Funkcjonalności:
Interfejs Graficzny (GUI) w Pythonie:

PyQt5 umożliwia użytkownikom łatwe wczytywanie, edytowanie oraz generowanie danych dotyczących harmonogramowania zadań.
Użytkownicy mogą wybierać spośród różnych algorytmów optymalizacyjnych, takich jak NEH, Symulowane Wyżarzanie czy Metoda Brute Force.
Aplikacja oferuje wizualizację wyników w postaci wykresów Gantta, co ułatwia analizę harmonogramów.
Backend w C++ z Frameworkiem Crow:

Crow obsługuje żądania HTTP oraz zarządza danymi JSON, umożliwiając komunikację z interfejsem graficznym.
Backend przetwarza dane harmonogramowania, uruchamia wybrane algorytmy oraz zwraca wyniki do GUI.
Konteneryzacja z Dockerem:

Docker zapewnia, że aplikacja działa w spójnym środowisku niezależnie od systemu operacyjnego hosta.
Ułatwia to wdrażanie oraz skalowanie aplikacji w różnych środowiskach.
