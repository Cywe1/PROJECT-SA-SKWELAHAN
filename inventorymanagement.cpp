#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <string>
#include <limits>
#include <cctype>

using namespace std;

struct Product {
    int id;
    string name;
    string category;
    int quantity;
    double price;
};

vector<Product> products;

// Helper to trim whitespace
string trim(const string& s) {
    size_t first = s.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, last - first + 1);
}

// Helper: check if string is digits only
bool isDigits(const string& s) {
    if (s.empty()) return false;
    for (char c : s)
        if (!isdigit(static_cast<unsigned char>(c)))
            return false;
    return true;
}

// Helper: check if string contains at least one letter
bool containsLetter(const string& s) {
    for (char c : s)
        if (isalpha(static_cast<unsigned char>(c)))
            return true;
    return false;
}

// Helper: check if a string is a valid floating point number
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

// Get the next available Product ID
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

// Input with back option and strict numeric check for int > 0
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

// Input with back option and strict check for double > 0
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

// For name/category input: must have at least one letter (not only numbers)
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

// Menu choice with back option
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

void menu() {
    loadProducts("products.txt");
    while (true) {
        cout << "\n--- Inventory Management System ---\n";
        cout << "1. Add New Product\n";
        cout << "2. Update Product Stock (Stock In/Out)\n";
        cout << "3. Search Product\n";
        cout << "4. Display All Products\n";
        cout << "5. Calculate Inventory Value\n";
        cout << "6. Delete a Product\n";
        cout << "0. Exit System\n";
        int choice;
        if (!getMenuChoice("Enter choice (or 'b' to go back): ", choice, 0, 6)) continue;
        switch (choice) {
            case 1: addProduct(); break;
            case 2: updateStock(); break;
            case 3: searchProduct(); break;
            case 4: displayProducts(); break;
            case 5: inventoryValue(); break;
            case 6: deleteProduct(); break;
            case 0: cout << "Exiting system. Goodbye!\n"; return;
            default: cout << "Invalid choice.\n";
        }
    }
}

int main() {
    menu();
    return 0;
}