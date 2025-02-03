#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <limits>
#include <fstream>
#include <iomanip>
#include "crow_all.h" // Dodanie biblioteki Crow do tworzenia REST API
#include <nlohmann/json.hpp> // Biblioteka do obsługi JSON

using json = nlohmann::json;
using namespace std;

// Stałe dla ograniczeń
const int MAX_ITERACJI = 1000000;
const int MAX_CZAS_PRZETWARZANIA = 1000000;

int oblicz_calkowity_czas_zakonczenia(const vector<vector<int>> &czasy_przetwarzania, const vector<int> &permutacja)
{
    // Tworzymy kopię permutacji określającej kolejność zadań
    vector<int> Kolejnosc = permutacja;
    int liczbaZadan = permutacja.size();

    // Jeśli nie ma zadań, czas zakończenia wynosi 0
    if (liczbaZadan == 0)
        return 0;

    int liczbaMaszyn = czasy_przetwarzania[0].size();

    // Inicjalizacja: najbliższy dostępny czas dla każdej maszyny wynosi początkowo 0
    vector<int> terminy_zwolnienia_maszyn(liczbaMaszyn, 0);

    // Tablica przechowująca skumulowane czasy przetwarzania dla każdego zadania
    vector<vector<int>> skumulowaneCzasyPrzetwarzania(liczbaZadan, vector<int>(liczbaMaszyn, 0));

    // Obliczamy skumulowane czasy przetwarzania dla wszystkich zadań
    for (int indeksZadania = 0; indeksZadania < liczbaZadan; ++indeksZadania)
    {
        int idZadania = permutacja[indeksZadania];

        // Czas przetwarzania na pierwszej maszynie to czas samego przetwarzania
        skumulowaneCzasyPrzetwarzania[indeksZadania][0] = czasy_przetwarzania[idZadania][0];

        // Dla kolejnych maszyn sumujemy czasy przetwarzania
        for (int indeksMaszyny = 1; indeksMaszyny < liczbaMaszyn; ++indeksMaszyny)
        {
            skumulowaneCzasyPrzetwarzania[indeksZadania][indeksMaszyny] =
                skumulowaneCzasyPrzetwarzania[indeksZadania][indeksMaszyny - 1] +
                czasy_przetwarzania[idZadania][indeksMaszyny];
        }
    }
    
    int koniec_poprzedniego_zadania = 0;

    // Tablica przechowująca czasy zakończenia zadań
    vector<int> czasyZakonczeniaZadan(liczbaZadan, 0);

    // Główna pętla obliczająca czasy zakończenia dla każdego zadania
    for (int indeksZadania = 0; indeksZadania < liczbaZadan; ++indeksZadania)
    {
        int idZadania = permutacja[indeksZadania];

        // Ustalamy najwcześniejszy możliwy czas rozpoczęcia bieżącego zadania
        int najwczesniejszy_mozliwy_start_zadania = koniec_poprzedniego_zadania;

        for (int indeksMaszyny = 0; indeksMaszyny < liczbaMaszyn; ++indeksMaszyny)
        {
            // Jeśli indeks maszyny jest większy od 0, bierzemy pod uwagę sumę czasów przetwarzania na poprzednich maszynach
            int sumaCzasowPrzetwarzaniaPoprzednichMaszyn = 0;

            if (indeksMaszyny > 0)
            {
                sumaCzasowPrzetwarzaniaPoprzednichMaszyn =skumulowaneCzasyPrzetwarzania[indeksZadania][indeksMaszyny - 1];
            }

            // Obliczamy najwcześniejszy możliwy czas rozpoczęcia uwzględniając dostępność maszyny
            najwczesniejszy_mozliwy_start_zadania = max(najwczesniejszy_mozliwy_start_zadania,terminy_zwolnienia_maszyn[indeksMaszyny] - sumaCzasowPrzetwarzaniaPoprzednichMaszyn);
        }

        // Zapewniamy, że czas rozpoczęcia nie jest ujemny
        najwczesniejszy_mozliwy_start_zadania = max(najwczesniejszy_mozliwy_start_zadania, 0);

        for (int indeksMaszyny = 0; indeksMaszyny < liczbaMaszyn; ++indeksMaszyny)
        {
            // Obliczamy czas zakończenia zadania na bieżącej maszynie
            int czasZakonczeniaNaMaszynie =najwczesniejszy_mozliwy_start_zadania +skumulowaneCzasyPrzetwarzania[indeksZadania][indeksMaszyny];

            // Aktualizujemy najbliższy dostępny czas maszyny
            terminy_zwolnienia_maszyn[indeksMaszyny] = czasZakonczeniaNaMaszynie;

            // Jeśli to ostatnia maszyna, zapisujemy czas zakończenia zadania
            if (indeksMaszyny == liczbaMaszyn - 1)
            {
                czasyZakonczeniaZadan[indeksZadania] = czasZakonczeniaNaMaszynie;
            }
        }

        // Aktualizujemy czas zakończenia dla poprzedniego zadania
        koniec_poprzedniego_zadania = najwczesniejszy_mozliwy_start_zadania;
    }

    // Zwracamy czas zakończenia ostatniego zadania
    return czasyZakonczeniaZadan[liczbaZadan - 1];
}


// Algorytm NEH
vector<int> algorytm_NEH(const vector<vector<int>> &czasy_przetwarzania)
{
    int liczba_zadan = czasy_przetwarzania.size();
    int liczba_maszyn = czasy_przetwarzania[0].size();

    // Krok 1: Oblicz całkowite czasy przetwarzania dla każdego zadania
    vector<int> dlugosc_zadan(liczba_zadan, 0);
    vector<int> indeksy_zadan(liczba_zadan);

    for (int i = 0; i < liczba_zadan; ++i)
    {
        int suma = 0;
        for (int j = 0; j < liczba_maszyn; ++j)
        {
            suma += czasy_przetwarzania[i][j];
        }
        dlugosc_zadan[i] = suma;
        indeksy_zadan[i] = i;
    }

    // Krok 2: Sortuj indeksy zadań według malejących czasów przetwarzania
    for (int i = 0; i < liczba_zadan - 1; ++i)
    {
        for (int j = i + 1; j < liczba_zadan; ++j)
        {
            // Porównaj sumy czasów przetwarzania dla zadania i oraz zadania j
            if (dlugosc_zadan[indeksy_zadan[i]] < dlugosc_zadan[indeksy_zadan[j]])
            {
                // Jeśli suma czasów zadania i jest mniejsza, zamień indeksy
                swap(indeksy_zadan[i], indeksy_zadan[j]);
            }
        }
    }

    // Krok 3: Buduj rozwiązanie, dodając zadania jedno po drugim
    vector<int> najlepsza_kolejnosc;
    for (int i = 0; i < liczba_zadan; ++i)
    {
        int zadanie_do_dodania = indeksy_zadan[i];
        vector<int> najlepsza_lokalna_kolejnosc;
        int najlepszy_czas = MAX_CZAS_PRZETWARZANIA; // duża wartość do inicjalizacji, aby pierwszy czas na pewno był lepszy

        // Wstawianie zadania na każdą możliwą pozycję
        for (int j = 0; j <= najlepsza_kolejnosc.size(); ++j)
        {
            vector<int> testowa_kolejnosc = najlepsza_kolejnosc;
            testowa_kolejnosc.insert(testowa_kolejnosc.begin() + j, zadanie_do_dodania);
            int czas = oblicz_calkowity_czas_zakonczenia(czasy_przetwarzania, testowa_kolejnosc);
            if (czas < najlepszy_czas)
            {
                najlepszy_czas = czas;
                najlepsza_lokalna_kolejnosc = testowa_kolejnosc;
            }
        }

        // Uaktualnij najlepszą kolejność
        najlepsza_kolejnosc = najlepsza_lokalna_kolejnosc;
    }

    return najlepsza_kolejnosc;
}

// Funkcja generująca losowe sąsiednie rozwiązanie przez zamianę dwóch zadań
vector<int> generuj_sasiednie_rozwiazanie(const vector<int> &obecne_rozwiazanie)
{
    vector<int> nowe_rozwiazanie = obecne_rozwiazanie;
    int n = nowe_rozwiazanie.size();
    int i = rand() % n; // zwraca numer indeksu zadania, [liczba zadań - 1]
    int j;

    do
    {
        j = rand() % n;
    } while (i == j); // losuje indeks j aż nie będzie inny niż indeks i

    swap(nowe_rozwiazanie[i], nowe_rozwiazanie[j]); // zamienia kolejnością 2 zadania w wektorze
    return nowe_rozwiazanie;
}

// Funkcja symulowanego wyżarzania
vector<int> algorytm_symulowanego_wyzarzania(const vector<vector<int>> &czasy_przetwarzania, vector<int> kolejnosc_startowa, double temperatura_poczatkowa, double wspolczynnik_chlodzenia, int liczba_iteracji)
{
    vector<int> aktualne_rozwiazanie = kolejnosc_startowa;
    int aktualny_czas = oblicz_calkowity_czas_zakonczenia(czasy_przetwarzania, aktualne_rozwiazanie);
    vector<int> najlepsze_rozwiazanie = aktualne_rozwiazanie;
    int najlepszy_czas = aktualny_czas;
    double temperatura = temperatura_poczatkowa;
    double prawdopodobienstwo;
    double losowa_liczba;

    for (int iteracja = 0; iteracja < liczba_iteracji; iteracja++)
    {
        vector<int> nowe_rozwiazanie = generuj_sasiednie_rozwiazanie(aktualne_rozwiazanie);
        int nowy_czas = oblicz_calkowity_czas_zakonczenia(czasy_przetwarzania, nowe_rozwiazanie);

        if (nowy_czas < najlepszy_czas)
        {
            najlepsze_rozwiazanie = nowe_rozwiazanie; // Dzięki rozbiciu na 2 warunki najlepsze rozwiązanie jest cały czas przechowywane, a zaakceptowane gorsze rozwiązanie działa na 'aktualnym_rozwiązaniu'
            najlepszy_czas = nowy_czas;
        }

        prawdopodobienstwo = exp((aktualny_czas - nowy_czas) / temperatura);
        losowa_liczba = (double)rand() / RAND_MAX;

        if (nowy_czas < aktualny_czas || prawdopodobienstwo > losowa_liczba) // jeśli nowy czas jest mniejszy to akceptuje od razu, a jeśli jest gorszy to liczy prawdopodobieństwo do akceptacji,
        {                                                                    // exp((aktualny_czas - nowy_czas) / temperatura) zwraca wartość z [0, 1] i porównuje z wartością((double)rand() / RAND_MAX)) też[0, 1]
            aktualne_rozwiazanie = nowe_rozwiazanie;
            aktualny_czas = nowy_czas;
        }

        temperatura *= wspolczynnik_chlodzenia;
    }

    return najlepsze_rozwiazanie;
}

vector<int> zamien(const vector<int> &obecne_rozwiazanie)
{
    vector<int> nowe_rozwiazanie = obecne_rozwiazanie;
    int n = nowe_rozwiazanie.size();
    int i = rand() % n; // zwraca numer indeksu zadania, [liczba zadań - 1]
    int j;

    do
    {
        j = rand() % n;
    } while (i == j); // losuje indeks j aż nie będzie inny niż indeks i

    swap(nowe_rozwiazanie[i], nowe_rozwiazanie[j]); // zamienia kolejnością 2 zadania w wektorze
    return nowe_rozwiazanie;
}

vector<int> brute(const vector<vector<int>> &czasy_przetwarzania, vector<int> kolejnosc_startowa, int liczba_iteracji)
{
    vector<int> najlepsze_rozwiazanie = kolejnosc_startowa;
    int najlepszy_czas = oblicz_calkowity_czas_zakonczenia(czasy_przetwarzania, najlepsze_rozwiazanie);

    // Generate all possible permutations
    int iteracja = 0;
    sort(kolejnosc_startowa.begin(), kolejnosc_startowa.end());
    do {
        if (iteracja >= liczba_iteracji && liczba_iteracji != -1) {
            break; // Stop if reached the maximum number of iterations
        }

        int obecny_czas = oblicz_calkowity_czas_zakonczenia(czasy_przetwarzania, kolejnosc_startowa);
        if (obecny_czas < najlepszy_czas) {
            najlepszy_czas = obecny_czas;
            najlepsze_rozwiazanie = kolejnosc_startowa;
        }

        iteracja++;
    } while (next_permutation(kolejnosc_startowa.begin(), kolejnosc_startowa.end()));

    return najlepsze_rozwiazanie;
}

int main()
{
    srand(static_cast<unsigned int>(time(0)));

    crow::SimpleApp app;

    // Definicja endpointa do obsługi żądań POST
    CROW_ROUTE(app, "/api/run").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
        auto body = json::parse(req.body);
        // Parsowanie danych wejściowych
        int liczba_zadan = body["liczba_zadan"];
        int liczba_maszyn = body["liczba_maszyn"];
        int algorytm = body["algorytm"];

        // Wczytywanie czasów przetwarzania
        std::vector<std::vector<int>> czasy_przetwarzania = body["czasy_przetwarzania"].get<std::vector<std::vector<int>>>();

        // Inicjalizacja startowej kolejności
        std::vector<int> kolejnosc_startowa(liczba_zadan);
        for (int i = 0; i < liczba_zadan; ++i)
        {
            kolejnosc_startowa[i] = i;
        }

        // Wybór i uruchomienie algorytmu
        std::vector<int> rozwiazanie;
        int najlepszy_czas = 0;

        if (algorytm == 1)
        {
            // Algorytm NEH
            rozwiazanie = algorytm_NEH(czasy_przetwarzania);
            najlepszy_czas = oblicz_calkowity_czas_zakonczenia(czasy_przetwarzania, rozwiazanie);
        }
        else if (algorytm == 2)
        {
            // Parametry dla Symulowanego Wyżarzania
            double temperatura_poczatkowa = body["temperatura_poczatkowa"];
            double wspolczynnik_chlodzenia = body["wspolczynnik_chlodzenia"];
            int liczba_iteracji = body["liczba_iteracji"];

            // Uruchomienie algorytmu symulowanego wyżarzania
            rozwiazanie = algorytm_symulowanego_wyzarzania(
                czasy_przetwarzania, kolejnosc_startowa, temperatura_poczatkowa, wspolczynnik_chlodzenia, liczba_iteracji);
            najlepszy_czas = oblicz_calkowity_czas_zakonczenia(czasy_przetwarzania, rozwiazanie);
        }
        else if (algorytm == 3)
        {
            // Algorytm Brute Force
            rozwiazanie = brute(czasy_przetwarzania, kolejnosc_startowa, 1000);
            najlepszy_czas = oblicz_calkowity_czas_zakonczenia(czasy_przetwarzania, rozwiazanie);
        }
        else
        {
            return crow::response(400, "Nieznany algorytm.");
        }

        // Przygotowanie odpowiedzi
        json response;
        response["rozwiazanie"] = rozwiazanie;
        response["najlepszy_czas"] = najlepszy_czas;

        return crow::response{response.dump()};
    });

    app.port(8080).multithreaded().run();
}