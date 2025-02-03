import sys
import requests
import json
import pandas as pd
from PyQt5.QtWidgets import (
    QApplication, QWidget, QVBoxLayout, QPushButton, QFileDialog, QLabel,
    QComboBox, QLineEdit, QMessageBox, QTextEdit, QTableWidget, QTableWidgetItem, QDialog, QHBoxLayout
)
from PyQt5.QtCore import Qt
import matplotlib.pyplot as plt
import numpy as np

class DataEditorDialog(QDialog):
    def __init__(self, data, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Podgląd i Edycja Danych")
        self.resize(600, 400)
        self.data = data.copy()  # Kopia danych do edycji

        layout = QVBoxLayout()
        self.table = QTableWidget()
        self.load_data_into_table()
        layout.addWidget(self.table)

        # Przyciski do zapisu lub odrzucenia zmian
        button_layout = QHBoxLayout()
        self.save_button = QPushButton("Zapisz")
        self.save_button.clicked.connect(self.save_changes)
        self.cancel_button = QPushButton("Anuluj")
        self.cancel_button.clicked.connect(self.reject)
        button_layout.addWidget(self.save_button)
        button_layout.addWidget(self.cancel_button)
        layout.addLayout(button_layout)

        self.setLayout(layout)

    def load_data_into_table(self):
        df = pd.DataFrame(self.data)
        self.table.setRowCount(df.shape[0])
        self.table.setColumnCount(df.shape[1])
        self.table.setHorizontalHeaderLabels([f"M{j+1}" for j in range(df.shape[1])])

        for i in range(df.shape[0]):
            for j in range(df.shape[1]):
                item = QTableWidgetItem(str(df.iloc[i, j]))
                item.setFlags(item.flags() | Qt.ItemIsEditable)
                self.table.setItem(i, j, item)

    def save_changes(self):
        df = pd.DataFrame(columns=[f"M{j+1}" for j in range(self.table.columnCount())])
        for i in range(self.table.rowCount()):
            for j in range(self.table.columnCount()):
                df.at[i, f"M{j+1}"] = self.table.item(i, j).text()
        self.data = df.to_dict(orient='records')
        self.accept()

    def get_data(self):
        return self.data

class SchedulerApp(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Scheduler GUI")
        self.setGeometry(100, 100, 800, 600)
        self.data = []
        self.initUI()

    def initUI(self):
        layout = QVBoxLayout()

        # Wczytywanie pliku instancji
        self.load_button = QPushButton("Wczytaj Instancję Problemu")
        self.load_button.clicked.connect(self.load_instance)
        layout.addWidget(self.load_button)

        self.instance_label = QLabel("Nie załadowano pliku.")
        layout.addWidget(self.instance_label)
        
        # Dodaj ten fragment w metodzie initUI() przed dodaniem przycisku "Podgląd i Edycja Danych"
        self.generate_button = QPushButton("Generuj Losowe Dane")
        self.generate_button.clicked.connect(self.generate_random_data)
        layout.addWidget(self.generate_button)

        # Przyciski do podglądu i edycji danych
        self.view_edit_button = QPushButton("Podgląd i Edycja Danych")
        self.view_edit_button.clicked.connect(self.view_edit_data)
        self.view_edit_button.setEnabled(False)
        layout.addWidget(self.view_edit_button)

        # Wybór algorytmu
        self.alg_label = QLabel("Wybierz Algorytm:")
        layout.addWidget(self.alg_label)

        self.alg_combo = QComboBox()
        self.alg_combo.addItems(["NEH", "Symulowane Wyżarzanie", "Brute Force"])
        self.alg_combo.currentIndexChanged.connect(self.alg_selected)
        layout.addWidget(self.alg_combo)

        # Parametry dla Symulowanego Wyżarzania
        self.param_layout = QVBoxLayout()

        self.temp_input = QLineEdit()
        self.temp_input.setPlaceholderText("Temperatura Początkowa")
        self.temp_input.setVisible(False)
        self.param_layout.addWidget(self.temp_input)

        self.cooling_input = QLineEdit()
        self.cooling_input.setPlaceholderText("Współczynnik Chłodzenia (0-1)")
        self.cooling_input.setVisible(False)
        self.param_layout.addWidget(self.cooling_input)

        self.iter_input = QLineEdit()
        self.iter_input.setPlaceholderText("Liczba Iteracji")
        self.iter_input.setVisible(False)
        self.param_layout.addWidget(self.iter_input)

        layout.addLayout(self.param_layout)

        # Uruchomienie algorytmu
        self.run_button = QPushButton("Uruchom Algorytm")
        self.run_button.clicked.connect(self.run_algorithm)
        self.run_button.setEnabled(False)
        layout.addWidget(self.run_button)

        # Wyświetlanie wyników
        self.result_label = QLabel("Wyniki:")
        layout.addWidget(self.result_label)

        self.result_text = QTextEdit()
        self.result_text.setReadOnly(True)
        layout.addWidget(self.result_text)

        # Log postępu
        self.log_label = QLabel("Log:")
        layout.addWidget(self.log_label)

        self.log_text = QTextEdit()
        self.log_text.setReadOnly(True)
        layout.addWidget(self.log_text)

        self.setLayout(layout)

    def load_instance(self):
        options = QFileDialog.Options()
        fileName, _ = QFileDialog.getOpenFileName(self, "Wybierz plik instancji", "", "JSON Files (*.json);;All Files (*)", options=options)
        if fileName:
            self.instance_label.setText(f'Załadowano plik: {fileName}')
            self.instance_file = fileName
            self.log_text.append(f"Załadowano plik instancji: {fileName}")
            # Wczytanie danych z pliku
            try:
                with open(fileName, 'r') as f:
                    loaded_json = json.load(f)
                    self.data = loaded_json.get("czasy_przetwarzania", self.generate_random_data1())
                self.view_edit_button.setEnabled(True)
                self.run_button.setEnabled(True)
                self.log_text.append("Dane wczytane pomyślnie.")
            except Exception as e:
                QMessageBox.critical(self, "Błąd", f"Nie można wczytać pliku: {e}")
                self.log_text.append(f"Error wczytywania pliku: {e}")
    
    def generate_random_data1(self):
        import random
        liczba_zadan = 10
        liczba_maszyn = 5
        czasy_przetwarzania = [[random.randint(1,5) for _ in range(liczba_maszyn)] for _ in range(liczba_zadan)]
        self.data = czasy_przetwarzania
        return self.data
    
    def generate_random_data(self):
        import random
        liczba_zadan = random.randint(5, 20)  # Możesz dostosować zakres
        liczba_maszyn = random.randint(3, 10)
        self.data = [[random.randint(1, 5) for _ in range(liczba_maszyn)] for _ in range(liczba_zadan)]
        self.view_edit_button.setEnabled(True)
        self.run_button.setEnabled(True)
        self.instance_label.setText(f"Generowano losowe dane: {liczba_zadan} zadań, {liczba_maszyn} maszyn")
        self.log_text.append(f"Generowano losowe dane: {liczba_zadan} zadań, {liczba_maszyn} maszyn")


    def view_edit_data(self):
        dialog = DataEditorDialog(self.data, self)
        if dialog.exec_() == QDialog.Accepted:
            self.data = dialog.get_data()
            QMessageBox.information(self, "Informacja", "Dane zostały zaktualizowane.")
            self.log_text.append("Dane zostały zaktualizowane.")
        else:
            self.log_text.append("Edycja danych anulowana.")

    def alg_selected(self, index):
        if index == 1:  # Symulowane Wyżarzanie
            self.temp_input.setVisible(True)
            self.cooling_input.setVisible(True)
            self.iter_input.setVisible(True)
        else:
            self.temp_input.setVisible(False)
            self.cooling_input.setVisible(False)
            self.iter_input.setVisible(False)

    def run_algorithm(self):
        if not self.data:
            QMessageBox.warning(self, "Błąd", "Brak danych do przetworzenia.")
            return

        alg_index = self.alg_combo.currentIndex() + 1
        payload = {
            "liczba_zadan": len(self.data),
            "liczba_maszyn": len(self.data[0]) if self.data else 0,
            "algorytm": alg_index,
            "czasy_przetwarzania": self.data
        }

        if alg_index == 2:
            try:
                temp = float(self.temp_input.text())
                cooling = float(self.cooling_input.text())
                iterations = int(self.iter_input.text())
                if not (0 < cooling < 1):
                    raise ValueError("Współczynnik chłodzenia musi być w zakresie (0,1).")
                payload.update({
                    "temperatura_poczatkowa": temp,
                    "wspolczynnik_chlodzenia": cooling,
                    "liczba_iteracji": iterations
                })
            except ValueError as ve:
                QMessageBox.warning(self, "Błąd", f"Nieprawidłowe parametry: {ve}")
                return

        # Wysyłanie żądania do API
        try:
            self.log_text.append("Wysyłanie żądania do serwera C++...")
            response = requests.post("http://localhost:8080/api/run", json=payload)
            if response.status_code == 200:
                result = response.json()
                rozwiazanie = result.get("rozwiazanie", [])
                najlepszy_czas = result.get("najlepszy_czas", 0)
                self.result_text.setText(f"Najlepsze rozwiazanie: {' '.join(map(str, rozwiazanie))}\nNajlepszy czas: {najlepszy_czas}")

                # Opcjonalna wizualizacja harmonogramu
                self.create_gantt_chart(rozwiazanie, self.data)
                self.log_text.append("Otrzymano wyniki.")
            else:
                self.log_text.append(f"Serwer zwrócił błąd: {response.status_code}")
        except requests.exceptions.RequestException as e:
            QMessageBox.critical(self, "Błąd", f"Nie można połączyć się z serwerem C++: {e}")
            self.log_text.append(f"Błąd połączenia z serwerem: {e}")
        
    def create_gantt_chart(self, harmonogram, czasy_przetwarzania):
        num_machines = len(czasy_przetwarzania[0])  # liczba maszyn
        num_tasks = len(czasy_przetwarzania)         # liczba zadań
    
        # Przygotowanie figure
        fig, axs = plt.subplots(figsize=(12, 6))
    
        # Zmienne do przechowywania czasu zakończenia
        finish_times = np.zeros(num_machines)
    
        # Rysowanie wykresów Gantta
        for i in range(num_tasks):
            task_index = harmonogram[i]  # Indeks zadania
            for j in range(num_machines):
                duration = czasy_przetwarzania[task_index][j]  # Czas trwania na danej maszynie
                if duration > 0:
                    start_time = finish_times[j]
                    
                    # Rysowanie paska
                    axs.barh(f'Maszyna {j + 1}', duration, left=start_time,
                            color=plt.cm.viridis(i / num_tasks), edgecolor='black')
                    
                    # Adnotacja do paska
                    axs.annotate(f'Z{task_index + 1}', (start_time + duration / 2, f'Maszyna {j + 1}'),
                                ha='center', va='center', color='white', fontsize=10)
    
                    # Aktualizacja czasu zakończenia dla maszyny
                    finish_times[j] += duration  # Zaktualizuj czas zakończenia
    
        # Ustawienia wykresu
        axs.set_xlabel('Czas wykonywania')
        axs.set_ylabel('Maszyny')
        axs.set_title('Wykres Gantta dla zadań')
        axs.set_xticks(range(0, int(np.max(finish_times) + 1), 1))
        axs.grid(True)
    
        plt.tight_layout()
        plt.show()
            
    

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = SchedulerApp()
    window.show()
    sys.exit(app.exec_())
