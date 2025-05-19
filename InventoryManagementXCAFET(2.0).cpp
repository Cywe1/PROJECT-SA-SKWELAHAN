#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <string>
#include <limits>
#include <cctype>

using namespace std;

#define W_ID    8
#define W_NAME  32     
#define W_CAT   22     
#define W_QTY   10
#define W_PRICE 12

// Inventory Product Struct & Globals
struct Product {
    int id;
    string name;
    string category;
    int quantity;
    double price;
};

vector<Product> products;

// Helper for categories
vector<string> getCategories() {
    vector<string> cats;
    for (const auto& p : products) {
        if (find(cats.begin(), cats.end(), p.category) == cats.end())
            cats.push_back(p.category);
    }
    return cats;
}

// Inventory Management Functions

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
    cout << left << setw(W_ID) << "ProdID"
         << setw(W_NAME) << "Name"
         << setw(W_CAT) << "Category"
         << setw(W_QTY) << "Qty"
         << setw(W_PRICE) << "Price" << endl;
    cout << string(W_ID + W_NAME + W_CAT + W_QTY + W_PRICE, '-') << endl;
    for (const auto& p : products) {
        cout << left << setw(W_ID) << p.id
             << setw(W_NAME) << p.name
             << setw(W_CAT) << p.category
             << setw(W_QTY) << p.quantity
             << setw(W_PRICE) << fixed << setprecision(2) << p.price << endl;
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

// Order Logic (CAFET5.0(BETA)
double grossIncome = 0.0;
const int MAX_ORDERS = 100;
string orderItems[MAX_ORDERS];
int orderQuantities[MAX_ORDERS];
double orderCosts[MAX_ORDERS];
string orderPaymentMethods[MAX_ORDERS];
int orderCount = 0;

// Helper
vector<const Product*> getMenuSelectionForOrder(bool& backSelected) {
    backSelected = false;
    loadProducts("products.txt");
    if (products.empty()) {
        cout << "\n--- Menu ---\nNo items available. Please ask admin to add items.\n";
        return {};
    }
    vector<string> categories = getCategories();
    while (true) {
        cout << "\n--- Menu Categories ---\n";
        cout << "A. Show All Items\n";
        for (size_t i = 0; i < categories.size(); ++i) {
            cout << i + 1 << ". " << categories[i] << endl;
        }
        cout << "B. Back\n";
        cout << "Select category (enter number, 'A' for all, or 'B' to go back): ";
        string catChoiceStr;
        getline(cin, catChoiceStr);

        if (catChoiceStr.length() == 1 && (catChoiceStr[0] == 'A' || catChoiceStr[0] == 'a')) {
            vector<const Product*> toDisplay;
            for (const auto& p : products) toDisplay.push_back(&p);
            return toDisplay;
        } else if (catChoiceStr.length() == 1 && (catChoiceStr[0] == 'B' || catChoiceStr[0] == 'b')) {
            backSelected = true;
            return {};
        } else if (isDigits(catChoiceStr)) {
            int catChoice = stoi(catChoiceStr);
            if (catChoice < 1 || catChoice > (int)categories.size()) {
                cout << "Invalid input. Try again.\n";
            } else {
                string selectedCat = categories[catChoice - 1];
                vector<const Product*> toDisplay;
                for (const auto& p : products)
                    if (p.category == selectedCat)
                        toDisplay.push_back(&p);
                return toDisplay;
            }
        } else {
            cout << "Invalid input. Try again.\n";
        }
    }
}

void selectPaymentMethod(string& paymentMethod, double total);

// placeOrder
void placeOrder() {
    loadProducts("products.txt");
    if (products.empty()) {
        cout << "No items available to order. Please ask admin to add items first.\n";
        return;
    }
    vector<int> cartQuantities(products.size(), 0); // FIX: no name conflict!
    double total = 0;
    string paymentMethod;
    bool orderCancelled = false;
    while (true) {
        bool backSelected = false;
        vector<const Product*> menu = getMenuSelectionForOrder(backSelected);
        if (backSelected) break;
        if (menu.empty()) continue;

        // Display menu
        cout << "\n--- Menu ---\n";
        cout << left << setw(4) << "#" 
             << setw(W_NAME) << "Name"
             << setw(W_CAT) << "Category"
             << setw(W_PRICE) << "Price"
             << setw(W_QTY) << "Stock" << endl;
        cout << string(4 + W_NAME + W_CAT + W_PRICE + W_QTY, '-') << endl;
        for (size_t i = 0; i < menu.size(); ++i) {
            cout << left << setw(4) << (i + 1)
                 << setw(W_NAME) << menu[i]->name
                 << setw(W_CAT) << menu[i]->category
                 << setw(W_PRICE) << fixed << setprecision(2) << menu[i]->price
                 << setw(W_QTY) << menu[i]->quantity << endl;
        }

        // Add items from this menu
        while (true) {
            cout << "Enter item number to add to bag (0 to finish this menu, 'b' to cancel order): ";
            string choice;
            getline(cin, choice);
            if (choice == "b" || choice == "B") {
                orderCancelled = true;
                break;
            }
            if (choice == "0") break;
            if (!isDigits(choice)) {
                cout << "Invalid input. Please enter a valid item number.\n";
                continue;
            }
            int itemNum = stoi(choice);
            if (itemNum < 1 || itemNum > (int)menu.size()) {
                cout << "Invalid choice. Try again.\n";
                continue;
            }
            int menuIdx = itemNum - 1;
            int prodIdx = -1;
            // Find index in global products vector
            for (size_t i = 0; i < products.size(); ++i) {
                if (&products[i] == menu[menuIdx]) {
                    prodIdx = i;
                    break;
                }
            }
            if (prodIdx == -1) {
                cout << "Product not found. Try again.\n";
                continue;
            }
            int quantity;
            cout << "Enter quantity: ";
            string qtyStr;
            getline(cin, qtyStr);
            if (!isDigits(qtyStr)) {
                cout << "Invalid input. Please enter a number.\n";
                continue;
            }
            quantity = stoi(qtyStr);
            if (quantity <= 0 || quantity > products[prodIdx].quantity - cartQuantities[prodIdx]) {
                cout << "Invalid quantity or not enough stock.\n";
                continue;
            }
            cartQuantities[prodIdx] += quantity;
            cout << "Added to bag: " << products[prodIdx].name << " x" << quantity << endl;
        }
        if (orderCancelled) break;

        // After finishing this menu, ask if want to add from another menu
        cout << "Do you want to add items from another category/menu? (Y/N): ";
        string addMore;
        getline(cin, addMore);
        if (addMore.length() && (addMore[0] == 'y' || addMore[0] == 'Y')) {
            continue; // loop to category/menu selection
        } else {
            break;
        }
    }
    if (orderCancelled) {
        cout << "Order cancelled.\n";
        return;
    }
    // If nothing was ordered
    bool hasOrder = false;
    for (int qty : cartQuantities) if (qty > 0) hasOrder = true;
    if (!hasOrder) {
        cout << "No items were added to your bag. Order cancelled.\n";
        return;
    }

    // Show final bag/summary
    cout << "\n--- Bag Summary ---\n";
    cout << left << setw(25) << "Item" << setw(10) << "Qty" << setw(20) << "Total Cost" << endl;
    cout << "------------------------------------------------\n";
    total = 0;
    for (size_t i = 0; i < products.size(); ++i) {
        if (cartQuantities[i] > 0) {
            double cost = cartQuantities[i] * products[i].price;
            total += cost;
            cout << left << setw(25) << products[i].name << setw(10) << cartQuantities[i] << cost << " pesos" << endl;
        }
    }
    cout << "------------------------------------------------\n";
    cout << "Grand Total: " << fixed << setprecision(2) << total << " pesos" << endl;

    // Confirm order
    cout << "Do you want to confirm your order? (Y/N): ";
    string confirmOrder;
    getline(cin, confirmOrder);
    if (!(confirmOrder.length() && (confirmOrder[0] == 'y' || confirmOrder[0] == 'Y'))) {
        cout << "Order cancelled.\n";
        return;
    }

    // Deduct quantities and save
    for (size_t i = 0; i < products.size(); ++i) {
        if (cartQuantities[i] > 0) {
            products[i].quantity -= cartQuantities[i];
        }
    }
    saveProducts("products.txt");

    // Payment method
    selectPaymentMethod(paymentMethod, total);

    // Record order
    for (size_t i = 0; i < products.size(); ++i) {
        if (cartQuantities[i] > 0 && orderCount < MAX_ORDERS) {
            orderItems[orderCount] = products[i].name;
            orderQuantities[orderCount] = cartQuantities[i];
            orderCosts[orderCount] = cartQuantities[i] * products[i].price;
            orderPaymentMethods[orderCount] = paymentMethod;
            orderCount++;
        }
    }

    cout << "Thank you for your order!\n";
}

// Payment Method Function
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

// Main Menu
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
            displayProducts();
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