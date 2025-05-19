#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <string>
#include <limits>
#include <cctype>

using namespace std;

// --- Inventory Product Struct & Globals ---
struct Product {
    int id;
    string name;
    string category;
    int quantity;
    double price;
};

vector<Product> products;

// --- Inventory Management Functions (from inventorymanagement.cpp) ---

string trim(const string& s) {
    size_t first = s.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, last - first + 1);
}
bool isDigits(const string& s) {
    if (s.empty()) return false;
    for (char c : s)
        if (!isdigit(static_cast<unsigned char>(c)))
            return false;
    return true;
}
bool containsLetter(const string& s) {
    for (char c : s)
        if (isalpha(static_cast<unsigned char>(c)))
            return true;
    return false;
}
bool isValidDouble(const string& s) {
    bool decimalPoint = false;
    int count = 0;
    for (char c : s) {
        if (c == '.') {
            if (decimalPoint) return false;
            decimalPoint = true;
            continue;
        }
        if (!isdigit(static_cast<unsigned char>(c))) return false;
        count++;
    }
    return count > 0;
}
int getNextID() {
    int maxID = 0;
    for (const auto& p : products)
        if (p.id > maxID) maxID = p.id;
    return maxID + 1;
}
void loadProducts(const string& filename) {
    products.clear();
    ifstream fin(filename);
    if (!fin) return;
    Product p;
    string idstr;
    while (getline(fin, idstr, '|')) {
        p.id = stoi(idstr);
        getline(fin, p.name, '|');
        getline(fin, p.category, '|');
        fin >> p.quantity;
        fin.ignore(1, '|');
        fin >> p.price;
        fin.ignore(1, '\n');
        products.push_back(p);
    }
    fin.close();
}
void saveProducts(const string& filename) {
    ofstream fout(filename);
    for (const auto& p : products) {
        fout << p.id << "|" << p.name << "|" << p.category << "|"
             << p.quantity << "|" << p.price << "\n";
    }
    fout.close();
}
void displayProducts() {
    cout << "\n--- Product List ---\n";
    cout << left << setw(8) << "ProdID"
         << setw(25) << "Name"
         << setw(15) << "Category"
         << setw(10) << "Qty"
         << setw(10) << "Price" << endl;
    cout << string(68, '-') << endl;
    for (const auto& p : products) {
        cout << left << setw(8) << p.id
             << setw(25) << p.name
             << setw(15) << p.category
             << setw(10) << p.quantity
             << setw(10) << fixed << setprecision(2) << p.price << endl;
    }
}
int findProduct(const string& key) {
    string tkey = trim(key);
    for (size_t i = 0; i < products.size(); ++i) {
        if (to_string(products[i].id) == tkey || trim(products[i].name) == tkey)
            return i;
    }
    return -1;
}
bool getStrictIntPositive(const string& prompt, int& out) {
    string s;
    while (true) {
        cout << prompt;
        getline(cin, s);
        if (s == "b" || s == "B") return false;
        if (isDigits(s)) {
            try {
                out = stoi(s);
                if (out > 0) return true;
                cout << "Value must be greater than zero. Try again.\n";
            } catch (...) {
                cout << "Number out of range. Try again.\n";
            }
        } else {
            cout << "Invalid input. Enter a number greater than zero or 'b' to go back.\n";
        }
    }
}
bool getStrictDoublePositive(const string& prompt, double& out) {
    string s;
    while (true) {
        cout << prompt;
        getline(cin, s);
        if (s == "b" || s == "B") return false;
        if (isValidDouble(s)) {
            try {
                out = stod(s);
                if (out > 0) return true;
                cout << "Value must be greater than zero. Try again.\n";
            } catch (...) {
                cout << "Number out of range. Try again.\n";
            }
        } else {
            cout << "Invalid input. Enter a valid price greater than zero or 'b' to go back.\n";
        }
    }
}
bool getStringWithLetter(const string& prompt, string& out) {
    string s;
    while (true) {
        cout << prompt;
        getline(cin, s);
        if (s == "b" || s == "B") return false;
        if (containsLetter(s)) {
            out = s;
            return true;
        } else {
            cout << "Input must contain at least one letter and cannot be only numbers. Try again or 'b' to go back.\n";
        }
    }
}
bool getMenuChoice(const string& prompt, int& out, int minv, int maxv) {
    string s;
    while (true) {
        cout << prompt;
        getline(cin, s);
        if (s == "b" || s == "B") return false;
        if (isDigits(s)) {
            try {
                out = stoi(s);
                if (out >= minv && out <= maxv) return true;
            } catch (...) {}
        }
        cout << "Invalid input. Enter a number between " << minv << " and " << maxv << ", or 'b' to go back.\n";
    }
}
void addProduct() {
    Product p;
    p.id = getNextID();
    cout << "\nAssigned Product ID: " << p.id << endl;

    if (!getStringWithLetter("Enter Product Name (must contain a letter, or 'b' to go back): ", p.name)) return;
    if (!getStringWithLetter("Enter Category (must contain a letter, or 'b' to go back): ", p.category)) return;
    if (!getStrictIntPositive("Enter Quantity (number > 0, or 'b' to go back): ", p.quantity)) return;
    if (!getStrictDoublePositive("Enter Price (number > 0, or 'b' to go back): ", p.price)) return;

    products.push_back(p);
    saveProducts("products.txt");
    cout << "Product added successfully!\n";
}
void updateStock() {
    string key;
    cout << "\nEnter Product ID or Name to update (or 'b' to go back): ";
    getline(cin, key);
    if (key == "b" || key == "B") return;
    int idx = findProduct(key);
    if (idx == -1) {
        cout << "Product not found.\n";
        return;
    }
    int opt;
    if (!getMenuChoice("1. Stock In\n2. Stock Out\nEnter choice (or 'b' to go back): ", opt, 1, 2)) return;

    int qty;
    if (!getStrictIntPositive("Enter quantity (number > 0, or 'b' to go back): ", qty)) return;

    if (opt == 1) {
        products[idx].quantity += qty;
        cout << "Stock increased.\n";
    } else if (opt == 2) {
        if (qty > products[idx].quantity) {
            cout << "Not enough stock.\n";
            return;
        }
        products[idx].quantity -= qty;
        cout << "Stock decreased.\n";
    }
    saveProducts("products.txt");
}
void searchProduct() {
    string key;
    cout << "\nEnter Product ID or Name to search (or 'b' to go back): ";
    getline(cin, key);
    if (key == "b" || key == "B") return;
    int idx = findProduct(key);
    if (idx == -1) {
        cout << "Product not found.\n";
        return;
    }
    cout << "\nProduct Found:\n";
    cout << "ID: " << products[idx].id << endl;
    cout << "Name: " << products[idx].name << endl;
    cout << "Category: " << products[idx].category << endl;
    cout << "Quantity: " << products[idx].quantity << endl;
    cout << "Price: " << products[idx].price << endl;
}
void inventoryValue() {
    double total = 0;
    for (const auto& p : products)
        total += p.quantity * p.price;
    cout << "\nTotal Inventory Value: " << fixed << setprecision(2) << total << endl;
}
void deleteProduct() {
    string key;
    cout << "\nEnter Product ID or Name to delete (or 'b' to go back): ";
    getline(cin, key);
    if (key == "b" || key == "B") return;
    int idx = findProduct(key);
    if (idx == -1) {
        cout << "Product not found.\n";
        return;
    }
    products.erase(products.begin() + idx);
    saveProducts("products.txt");
    cout << "Product deleted successfully.\n";
}
void inventoryMenu() {
    loadProducts("products.txt");
    while (true) {
        cout << "\n--- Inventory Management System ---\n";
        cout << "1. Add New Product\n";
        cout << "2. Update Product Stock (Stock In/Out)\n";
        cout << "3. Search Product\n";
        cout << "4. Display All Products\n";
        cout << "5. Calculate Inventory Value\n";
        cout << "6. Delete a Product\n";
        cout << "0. Exit Admin Panel\n";
        int choice;
        if (!getMenuChoice("Enter choice (or 'b' to go back): ", choice, 0, 6)) continue;
        switch (choice) {
            case 1: addProduct(); break;
            case 2: updateStock(); break;
            case 3: searchProduct(); break;
            case 4: displayProducts(); break;
            case 5: inventoryValue(); break;
            case 6: deleteProduct(); break;
            case 0: return;
            default: cout << "Invalid choice.\n";
        }
    }
}

// --- Order Logic (CAFET5.0(BETA), using inventory as menu) ---
double grossIncome = 0.0;
const int MAX_ORDERS = 100;
string orderItems[MAX_ORDERS];
int orderQuantities[MAX_ORDERS];
double orderCosts[MAX_ORDERS];
string orderPaymentMethods[MAX_ORDERS];
int orderCount = 0;

void displayMenuForOrder() {
    loadProducts("products.txt");
    if (products.empty()) {
        cout << "\n--- Menu ---\nNo items available. Please ask admin to add items.\n";
        return;
    }
    cout << "\n--- Menu ---\n";
    for (size_t i = 0; i < products.size(); i++) {
        cout << i + 1 << ". " << products[i].name << " - " << fixed << setprecision(2)
             << products[i].price << " pesos (Stock: " << products[i].quantity << ")\n";
    }
}
void generateBill(const vector<int>& orders, const string& paymentMethod) {
    double total = 0;
    cout << "\n--- Bill Summary ---\n";
    cout << left << setw(25) << "Item" << setw(10) << "Qty" << setw(20) << "Total Cost" << endl;
    cout << "------------------------------------------------\n";
    bool hasOrder = false;
    for (size_t i = 0; i < products.size(); i++) {
        if (orders[i] > 0) {
            double cost = orders[i] * products[i].price;
            total += cost;
            cout << left << setw(25) << products[i].name << setw(10) << orders[i] << cost << " pesos" << endl;
            hasOrder = true;
            orderItems[orderCount] = products[i].name;
            orderQuantities[orderCount] = orders[i];
            orderCosts[orderCount] = cost;
            orderPaymentMethods[orderCount] = paymentMethod;
            orderCount++;
        }
    }
    if (hasOrder) {
        cout << "------------------------------------------------\n";
        cout << "Grand Total: " << fixed << setprecision(2) << total << " pesos" << endl;
        cout << "Payment Method: " << paymentMethod << "\n";
        grossIncome += total;
    }
}
void selectPaymentMethod(string& paymentMethod, double total) {
    int paymentChoice;
    int attemptCounter = 0;
    const int maxAttempts = 3;
    while (true) {
        if (attemptCounter >= maxAttempts) {
            cout << "Maximum attempts reached. Defaulting to Cash payment.\n";
            paymentMethod = "Cash";
            break;
        }
        attemptCounter++;
        cout << "\nSelect Payment Method:\n";
        cout << "1. Cash\n";
        cout << "2. GCash/PayMaya\n";
        cout << "Enter choice: ";
        cin >> paymentChoice;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        if (paymentChoice == 1) {
            paymentMethod = "Cash";
            break;
        } else if (paymentChoice == 2) {
            paymentMethod = "GCash/PayMaya";
            cout << "\nTotal amount to send: " << fixed << setprecision(2) << total << " pesos\n";
            cout << "09123456789  YANEX GWAPO P.\n";
            char confirmation;
            int confirmCounter = 0;
            const int maxConfirmAttempts = 3;
            while (true) {
                if (confirmCounter >= maxConfirmAttempts) {
                    cout << "Maximum confirmation attempts reached. Assuming payment is sent.\n";
                    confirmation = 'Y';
                } else {
                    cout << "Have you sent the payment? (Y/N): ";
                    cin >> confirmation;
                    confirmation = toupper(confirmation);
                    confirmCounter++;
                }
                if (confirmation == 'Y') {
                    break;
                } else if (confirmation == 'N') {
                    char choice;
                    cout << "Do you want to reselect the payment method? (Y/N): ";
                    cin >> choice;
                    if (toupper(choice) == 'Y') {
                        return selectPaymentMethod(paymentMethod, total);
                    }
                } else {
                    cout << "Invalid choice. Please enter Y or N.\n";
                }
            }
            break;
        } else {
            cout << "Invalid choice. Try again.\n";
        }
    }
}
void placeOrder() {
    loadProducts("products.txt");
    if (products.empty()) {
        cout << "No items available to order. Please ask admin to add items first.\n";
        return;
    }
    char choice;
    int itemChoice, quantity;
    string paymentMethod;
    double total = 0;
    vector<int> tempOrders(products.size(), 0);
    vector<int> orders(products.size(), 0);
    int orderAttemptCounter = 0;
    const int maxOrderAttempts = 10;
    do {
        if (orderAttemptCounter >= maxOrderAttempts) {
            cout << "Maximum order attempts reached. Returning to main menu.\n";
            return;
        }
        orderAttemptCounter++;
        displayMenuForOrder();
        cout << "Enter item number to order (0 to finish, 'b' to go back): ";
        cin >> choice;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a valid item number.\n";
            continue;
        }
        if (choice == '0') break;
        if (choice == 'b') return;
        itemChoice = choice - '0';
        if (itemChoice < 1 || itemChoice > (int)products.size()) {
            cout << "Invalid choice. Try again.\n";
            continue;
        }
        cout << "Enter quantity: ";
        cin >> quantity;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        int index = itemChoice - 1;
        if (quantity <= 0 || quantity > products[index].quantity) {
            cout << "Invalid quantity or not enough stock.\n";
            continue;
        }
        products[index].quantity -= quantity;
        tempOrders[index] += quantity;
        total += products[index].price * quantity;
        cout << "Order added successfully!\n";
        saveProducts("products.txt");
    } while (true);
    char confirm;
    int confirmOrderCounter = 0;
    const int maxConfirmOrderAttempts = 3;
    do {
        if (confirmOrderCounter >= maxConfirmOrderAttempts) {
            cout << "Maximum confirmation attempts reached. Discarding order.\n";
            for (size_t i = 0; i < products.size(); i++) {
                products[i].quantity += tempOrders[i];
            }
            saveProducts("products.txt");
            cout << "Order discarded.\n";
            return;
        }
        confirmOrderCounter++;
        cout << "\n--- Order Summary ---\n";
        for (size_t i = 0; i < products.size(); i++) {
            if (tempOrders[i] > 0) {
                cout << i + 1 << ". " << products[i].name << " - " << tempOrders[i] << " pcs\n";
            }
        }
        cout << "Do you want to discard any item? (Y/N): ";
        cin >> confirm;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter Y or N.\n";
            continue;
        }
        if (toupper(confirm) == 'Y') {
            int discardCounter = 0;
            const int maxDiscardAttempts = 3;
            do {
                if (discardCounter >= maxDiscardAttempts) {
                    cout << "Maximum discard attempts reached. Proceeding with the remaining items.\n";
                    break;
                }
                discardCounter++;
                cout << "Enter item number to discard (0 to finish): ";
                cin >> itemChoice;
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Please enter a number.\n";
                    continue;
                }
                if (itemChoice == 0) break;
                if (itemChoice < 1 || itemChoice > (int)products.size() || tempOrders[itemChoice - 1] == 0) {
                    cout << "Invalid choice. Try again.\n";
                    continue;
                }
                int index = itemChoice - 1;
                products[index].quantity += tempOrders[index];
                total -= products[index].price * tempOrders[index];
                tempOrders[index] = 0;
                cout << "Item discarded successfully!\n";
                saveProducts("products.txt");
            } while (true);
            bool allDiscarded = true;
            for (size_t i = 0; i < products.size(); i++) {
                if (tempOrders[i] > 0) {
                    allDiscarded = false;
                    break;
                }
            }
            if (allDiscarded) {
                cout << "All items discarded. Returning to menu.\n";
                return;
            }
        }
        cout << "Do you want to confirm your orders? (Y/N): ";
        cin >> confirm;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter Y or N.\n";
            continue;
        }
    } while (toupper(confirm) != 'Y' && toupper(confirm) != 'N');
    if (toupper(confirm) == 'Y') {
        for (size_t i = 0; i < products.size(); i++) {
            orders[i] += tempOrders[i];
        }
        selectPaymentMethod(paymentMethod, total);
        generateBill(orders, paymentMethod);
    } else {
        for (size_t i = 0; i < products.size(); i++) {
            products[i].quantity += tempOrders[i];
        }
        saveProducts("products.txt");
        cout << "Order discarded.\n";
    }
}

// --- Main Menu ---
int main() {
    string choice;
    do {
        cout << "\n--- Cafeteria Ordering System ---\n";
        cout << "1. View Menu\n";
        cout << "2. Place Order\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a valid choice.\n";
            continue;
        }
        if (choice == "1") {
            displayMenuForOrder();
        } else if (choice == "2") {
            placeOrder();
        } else if (choice == "0") {
            cout << "Thank you for using the Cafeteria Ordering System!\n";
            break;
        } else if (choice == "admin") {
            inventoryMenu(); // This is now the FULL inventory management system
        } else {
            cout << "Invalid choice. Try again.\n";
        }
    } while (true);
    return 0;
}