#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <limits>
#include <iomanip>

using namespace std;

// Структура для хранения информации о детали
struct Detail {
    char name[50];      // Название детали
    int quantity;       // Количество
    double price;       // Цена за единицу
    bool isDeleted;     // Флаг удаления (для логического удаления)
};

// Константы для файла
const char FILENAME[] = "warehouse.dat";
const int RECORD_SIZE = sizeof(Detail);

// Функция для очистки буфера ввода
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Функция для отображения меню
void displayMenu() {
    cout << "\n=== WAREHOUSE MANAGEMENT SYSTEM ===\n";
    cout << "1. Add part\n";
    cout << "2. Show all parts\n";
    cout << "3. Total cost of all parts\n";
    cout << "4. Parts with max/min quantity\n";
    cout << "5. Search part by name\n";
    cout << "6. Issue parts from warehouse\n";
    cout << "7. Save to text file\n";
    cout << "0. Exit\n";
    cout << "Choose action: ";
}

// Функция для добавления детали в файл
void addDetail() {
    ofstream file(FILENAME, ios::binary | ios::app);
    if (!file) {
        cerr << "Error opening file!" << endl;
        return;
    }

    Detail detail;
    detail.isDeleted = false;

    cout << "\n--- Adding new part ---\n";
    cout << "Enter part name: ";
    clearInputBuffer();
    cin.getline(detail.name, 50);

    cout << "Enter quantity: ";
    cin >> detail.quantity;

    cout << "Enter price per unit: ";
    cin >> detail.price;

    file.write(reinterpret_cast<char*>(&detail), RECORD_SIZE);
    file.close();

    cout << "Part successfully added!" << endl;
}

// Функция для чтения всех деталей из файла
vector<Detail> readAllDetails() {
    vector<Detail> details;
    ifstream file(FILENAME, ios::binary);

    if (!file) {
        return details;
    }

    Detail detail;
    while (file.read(reinterpret_cast<char*>(&detail), RECORD_SIZE)) {
        if (!detail.isDeleted) {
            details.push_back(detail);
        }
    }

    file.close();
    return details;
}

//   отображение всех деталей
void displayAllDetails() {
    vector<Detail> details = readAllDetails();

    if (details.empty()) {
        cout << "\nWarehouse is empty!" << endl;
        return;
    }

    cout << "\n--- All parts in warehouse ---\n";
    cout << left << setw(3) << "No" << setw(25) << "Name"
        << setw(12) << "Quantity" << setw(12) << "Price"
        << setw(15) << "Total Cost" << endl;
    cout << string(65, '-') << endl;

    for (size_t i = 0; i < details.size(); i++) {
        double total = details[i].quantity * details[i].price;
        cout << left << setw(3) << i + 1
            << setw(25) << details[i].name
            << setw(12) << details[i].quantity
            << setw(12) << fixed << setprecision(2) << details[i].price
            << setw(15) << fixed << setprecision(2) << total << endl;
    }
}

//   вычисление общей стоимости всех деталей
void calculateTotalCost() {
    vector<Detail> details = readAllDetails();
    double totalCost = 0;

    for (const auto& detail : details) {
        totalCost += detail.quantity * detail.price;
    }

    cout << "\n--- Total Cost ---\n";
    cout << "Total cost of all parts in warehouse: "
        << fixed << setprecision(2) << totalCost << " USD" << endl;
}

//   поиск деталей с максимальным и минимальным количеством
void findExtremeQuantities() {
    vector<Detail> details = readAllDetails();

    if (details.empty()) {
        cout << "\nWarehouse is empty!" << endl;
        return;
    }

    vector<Detail> maxDetails, minDetails;
    int maxQty = details[0].quantity;
    int minQty = details[0].quantity;

    // Находим максимальное и минимальное количество
    for (const auto& detail : details) {
        if (detail.quantity > maxQty) {
            maxQty = detail.quantity;
        }
        if (detail.quantity < minQty) {
            minQty = detail.quantity;
        }
    }

    //  все детали с максимальным и минимальным количеством
    for (const auto& detail : details) {
        if (detail.quantity == maxQty) {
            maxDetails.push_back(detail);
        }
        if (detail.quantity == minQty) {
            minDetails.push_back(detail);
        }
    }

    cout << "\n--- Parts with maximum quantity (" << maxQty << " pcs) ---\n";
    for (const auto& detail : maxDetails) {
        cout << "• " << detail.name << " (price: "
            << fixed << setprecision(2) << detail.price << " USD)\n";
    }

    cout << "\n--- Parts with minimum quantity (" << minQty << " pcs) ---\n";
    for (const auto& detail : minDetails) {
        cout << "• " << detail.name << " (price: "
            << fixed << setprecision(2) << detail.price << " USD)\n";
    }
}

// Функция для поиска детали по названию
void searchDetailByName() {
    char searchName[50];
    cout << "\n--- Part Search ---\n";
    cout << "Enter part name to search: ";
    clearInputBuffer();
    cin.getline(searchName, 50);

    vector<Detail> details = readAllDetails();
    bool found = false;

    for (const auto& detail : details) {
        if (strcmp(detail.name, searchName) == 0) {
            if (!found) {
                cout << "\nFound parts:\n";
                cout << left << setw(25) << "Name" << setw(12) << "Quantity"
                    << setw(12) << "Price" << setw(15) << "Total Cost" << endl;
                cout << string(60, '-') << endl;
                found = true;
            }
            double total = detail.quantity * detail.price;
            cout << left << setw(25) << detail.name
                << setw(12) << detail.quantity
                << setw(12) << fixed << setprecision(2) << detail.price
                << setw(15) << fixed << setprecision(2) << total << endl;
        }
    }

    if (!found) {
        cout << "Part with name '" << searchName << "' not found!" << endl;
    }
}

// Функция для выдачи деталей со склада (прямой доступ к файлу)
void issueDetails() {
    fstream file(FILENAME, ios::binary | ios::in | ios::out);
    if (!file) {
        cerr << "Error opening file!" << endl;
        return;
    }

    char detailName[50];
    int issueQty;

    cout << "\n--- Issuing parts from warehouse ---\n";
    cout << "Enter part name to issue: ";
    clearInputBuffer();
    cin.getline(detailName, 50);

    cout << "Enter quantity to issue: ";
    cin >> issueQty;

    Detail detail;
    bool found = false;
    streampos foundPos = -1;

    // Поиск детали в файле
    while (file.read(reinterpret_cast<char*>(&detail), RECORD_SIZE)) {
        if (!detail.isDeleted && strcmp(detail.name, detailName) == 0) {
            found = true;
            foundPos = file.tellg() - streampos(RECORD_SIZE);
            break;
        }
    }

    if (!found) {
        cout << "Part not found!" << endl;
        file.close();
        return;
    }

    // Обработка выдачи
    if (detail.quantity < issueQty) {
        cout << "Not enough parts in warehouse! Available: " << detail.quantity << endl;
        file.close();
        return;
    }

    detail.quantity -= issueQty;

    if (detail.quantity == 0) {
        // Логическое удаление записи
        detail.isDeleted = true;
        cout << "Part '" << detailName << "' completely issued from warehouse. Record deleted." << endl;
    }
    else {
        cout << "Issued " << issueQty << " parts '" << detailName << "'. Remaining: " << detail.quantity << endl;
    }

    // Запись изменений обратно в файл 
    file.seekp(foundPos);
    file.write(reinterpret_cast<char*>(&detail), RECORD_SIZE);
    file.close();
}

// Функция для сохранения данных в текстовый файл
void saveToTextFile() {
    vector<Detail> details = readAllDetails();
    ofstream textFile("warehouse_report.txt");

    if (!textFile) {
        cerr << "Error creating text file!" << endl;
        return;
    }

    textFile << "WAREHOUSE STATUS REPORT\n";
    textFile << "=======================\n\n";

    double totalCost = 0;

    // Заголовок таблицы
    textFile << left << setw(25) << "Part Name" << setw(12) << "Quantity"
        << setw(12) << "Price" << setw(15) << "Total Cost" << endl;
    textFile << string(60, '-') << endl;

    for (size_t i = 0; i < details.size(); i++) {
        double itemTotal = details[i].quantity * details[i].price;
        totalCost += itemTotal;

        textFile << left << setw(25) << details[i].name
            << setw(12) << details[i].quantity
            << setw(12) << fixed << setprecision(2) << details[i].price
            << setw(15) << fixed << setprecision(2) << itemTotal << endl;
    }

    textFile << "\nTOTAL COST OF ALL PARTS: " << fixed << setprecision(2) << totalCost << " USD\n";
    textFile.close();

    cout << "\nReport saved to file 'warehouse_report.txt'" << endl;
}

// Функция для инициализации тестовых данных
void initializeSampleData() {
    ofstream file(FILENAME, ios::binary | ios::app);
    if (!file) return;

    Detail samples[5] = {
        {"Bolt M6", 100, 0.15, false},
        {"Nut M6", 80, 0.10, false},
        {"Washer 6mm", 150, 0.05, false},
        {"Screw 4x20", 200, 0.08, false},
        {"Bearing 6200", 10, 5.50, false}
    };

    for (int i = 0; i < 5; i++) {
        file.write(reinterpret_cast<char*>(&samples[i]), RECORD_SIZE);
    }
    file.close();
}

int main() {
    int choice;

    // Инициализация тестовыми данными, если файл не существует
    ifstream testFile(FILENAME);
    if (!testFile) {
        initializeSampleData();
        cout << "Sample data initialized for testing." << endl;
    }
    testFile.close();

    do {
        displayMenu();
        cin >> choice;

        switch (choice) {
        case 1:
            addDetail();
            break;
        case 2:
            displayAllDetails();
            break;
        case 3:
            calculateTotalCost();
            break;
        case 4:
            findExtremeQuantities();
            break;
        case 5:
            searchDetailByName();
            break;
        case 6:
            issueDetails();
            break;
        case 7:
            saveToTextFile();
            break;
        case 0:
            cout << "Exiting program..." << endl;
            break;
        default:
            cout << "Invalid choice! Please try again." << endl;
        }

    } while (choice != 0);

    return 0;
}