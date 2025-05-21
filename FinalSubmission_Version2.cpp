#include <iostream>      // For standard input/output stream
#include <fstream>       // For file reading/writing
#include <iomanip>       // For output formatting (setw, setprecision)
#include <vector>        // For using vector container
#include <algorithm>     // For algorithms like find, transform
#include <string>        // For string handling
#include <limits>        // For numeric limits (e.g., int max value)
#include <cctype>        // For character type functions (isdigit, isalpha, etc.)
#include <random>        // For random number generation
#include <ctime>         // For getting current time/date
#include <sstream>       // For string streams (parsing/converting strings)

using namespace std;

// CONSTANTS for column widths when displaying products
const int W_ID    = 8;   // Width for Product ID column
const int W_NAME  = 32;  // Width for Name column
const int W_CAT   = 22;  // Width for Category column
const int W_QTY   = 10;  // Width for Quantity column
const int W_PRICE = 12;  // Width for Price column

// STRUCTURE for holding product info
struct Product {
    int id;              // Product ID
    string name;         // Product name
    string category;     // Product category
    int quantity;        // Stock quantity
    double price;        // Price per item
};

// Vector to store current products
vector<Product> products;

// Returns a vector of unique category names from products list
vector<string> getCategories() {
    vector<string> cats;
    for (const auto& p : products) {
        // Only add the category if it is not already in the list
        if (find(cats.begin(), cats.end(), p.category) == cats.end())
            cats.push_back(p.category);
    }
    return cats;
}

// Removes leading and trailing whitespace from a string
string trim(const string& s) {
    size_t first = s.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, last - first + 1);
}

// Checks if a string contains ONLY digits (0-9)
bool isDigits(const string& s) {
    if (s.empty()) return false;
    for (char c : s)
        if (!isdigit(static_cast<unsigned char>(c)))
            return false;
    return true;
}

// Checks if a string contains at least one alphabet letter
bool containsLetter(const string& s) {
    for (char c : s)
        if (isalpha(static_cast<unsigned char>(c)))
            return true;
    return false;
}

// Checks if a string is a valid positive double (decimal number)
bool isValidDouble(const string& s) {
    bool decimalPoint = false;
    int count = 0;
    for (char c : s) {
        if (c == '.') {
            if (decimalPoint) return false; // Only one decimal point allowed
            decimalPoint = true;
            continue;
        }
        if (!isdigit(static_cast<unsigned char>(c))) return false;
        count++;
    }
    return count > 0;
}

// Returns the next available Product ID (one higher than the highest)
int getNextID() {
    int maxID = 0;
    for (const auto& p : products)
        if (p.id > maxID) maxID = p.id;
    return maxID + 1;
}

// Loads products from a file into the products vector
void loadProducts(const string& filename) {
    products.clear();                      // Clear current products
    ifstream fin(filename);                // Open file for reading
    if (!fin) return;                      // If file doesn't exist, do nothing
    Product p;
    string idstr;
    while (getline(fin, idstr, '|')) {     // Read ID up to '|'
        p.id = stoi(idstr);                // Convert ID string to int
        getline(fin, p.name, '|');         // Read name up to '|'
        getline(fin, p.category, '|');     // Read category up to '|'
        fin >> p.quantity;                 // Read quantity (int)
        fin.ignore(1, '|');                // Skip '|' character
        fin >> p.price;                    // Read price (double)
        fin.ignore(1, '\n');               // Skip newline
        products.push_back(p);             // Add product to vector
    }
    fin.close();                           // Close file
}

// Saves current products vector into a file 
void saveProducts(const string& filename) {
    ofstream fout(filename);               // Open file for writing
    for (const auto& p : products) {
        fout << p.id << "|" << p.name << "|" << p.category << "|"
             << p.quantity << "|" << p.price << "\n";
    }
    fout.close();                          // Close file
}

// Displays all products in a formatted table
void displayProducts() {
    cout << "\n--- Product Catalog ---\n";
    // Print table headers with set widths
    cout << left << setw(W_ID) << "ProdID"
         << setw(W_NAME) << "Name"
         << setw(W_CAT) << "Category"
         << setw(W_QTY) << "Qty"
         << setw(W_PRICE) << "Price" << endl;
    cout << string(W_ID + W_NAME + W_CAT + W_QTY + W_PRICE, '-') << endl; // Divider
    // Print each product row
    for (const auto& p : products) {
        cout << left << setw(W_ID) << p.id
             << setw(W_NAME) << p.name
             << setw(W_CAT) << p.category
             << setw(W_QTY) << p.quantity
             << setw(W_PRICE) << fixed << setprecision(2) << p.price << endl;
    }
}

// Finds products whose name contains the search key (case-insensitive)
// Returns vector of matching product indices
vector<int> findProductsBySubstring(const string& key) {
    string tkey = key;
    transform(tkey.begin(), tkey.end(), tkey.begin(), ::tolower); // Lowercase search key
    vector<int> result;
    for (size_t i = 0; i < products.size(); ++i) {
        string pname = products[i].name;
        transform(pname.begin(), pname.end(), pname.begin(), ::tolower); // Lowercase product name
        if (pname.find(tkey) != string::npos)
            result.push_back(i); // Add index if key found in name
    }
    return result;
}

// Lets user search for a product by (partial) name and display details
void searchProduct() {
    cout << "\nEnter product name or part of it (or 'b' to go back): ";
    string search;
    getline(cin, search);                      // Get user search input
    if (search == "b" || search == "B") return;

    vector<int> matches = findProductsBySubstring(search); // Find matches

    if (matches.empty()) {
        cout << "No matching products found.\n";
        return;
    }

    cout << "\nMatching products:\n";
    for (size_t i = 0; i < matches.size(); ++i) {
        cout << i + 1 << ". " << products[matches[i]].name << " (ID: " << products[matches[i]].id << ")\n";
    }

    if (matches.size() == 1) {
        int idx = matches[0];
        // Show product details if only one match
        cout << "\nProduct details:\n";
        cout << "ID: " << products[idx].id << endl;
        cout << "Name: " << products[idx].name << endl;
        cout << "Category: " << products[idx].category << endl;
        cout << "Quantity: " << products[idx].quantity << endl;
        cout << "Price: " << products[idx].price << endl;
        return;
    }

    // If multiple matches, prompt to select one
    cout << "Enter number to see product details or 'b' to cancel: ";
    string choice;
    getline(cin, choice);
    if (choice == "b" || choice == "B") return;
    if (!isDigits(choice)) {
        cout << "Invalid input.\n";
        return;
    }
    int pick = stoi(choice);
    if (pick < 1 || pick > (int)matches.size()) {
        cout << "Invalid selection.\n";
        return;
    }
    int idx = matches[pick - 1];
    cout << "\nProduct details:\n";
    cout << "ID: " << products[idx].id << endl;
    cout << "Name: " << products[idx].name << endl;
    cout << "Category: " << products[idx].category << endl;
    cout << "Quantity: " << products[idx].quantity << endl;
    cout << "Price: " << products[idx].price << endl;
}

// Finds index of a product by exact ID or exact name
int findProduct(const string& key) {
    string tkey = trim(key);
    for (size_t i = 0; i < products.size(); ++i) {
        if (to_string(products[i].id) == tkey || trim(products[i].name) == tkey)
            return i; // Return index if found
    }
    return -1; // Not found
}

// Prompts user for a positive integer, stores in 'out', returns true on success
bool getStrictIntPositive(const string& prompt, int& out) {
    string s;
    while (true) {
        cout << prompt;
        getline(cin, s);
        if (s == "b" || s == "B") return false; // Back option
        if (isDigits(s)) {
            // Manual conversion to int to check overflow
            long long val = 0;
            bool overflow = false;
            for (char c : s) {
                val = val * 10 + (c - '0');
                if (val > std::numeric_limits<int>::max()) {
                    overflow = true;
                    break;
                }
            }
            if (!overflow && val > 0) {
                out = static_cast<int>(val);
                return true;
            }
            cout << "Value must be greater than zero and within valid range. Try again.\n";
        } else {
            cout << "Invalid input. Enter a number greater than zero or 'b' to go back.\n";
        }
    }
}

// Prompts user for a positive double, stores in 'out', returns true on success
bool getStrictDoublePositive(const string& prompt, double& out) {
    string s;
    while (true) {
        cout << prompt;
        getline(cin, s);
        if (s == "b" || s == "B") return false; // Back option
        if (isValidDouble(s)) {
            std::stringstream ss(s);
            double val;
            ss >> val;
            if (!ss.fail() && ss.eof() && val > 0) {
                out = val;
                return true;
            }
            cout << "Value must be greater than zero. Try again.\n";
        } else {
            cout << "Invalid input. Enter a valid price greater than zero or 'b' to go back.\n";
        }
    }
}

// Prompts user for a string with at least one letter, stores in 'out'
bool getStringWithLetter(const string& prompt, string& out) {
    string s;
    while (true) {
        cout << prompt;
        getline(cin, s);
        if (s == "b" || s == "B") return false; // Back option
        if (containsLetter(s)) {
            out = s;
            return true;
        } else {
            cout << "Input must contain at least one letter and cannot be only numbers. Try again or 'b' to go back.\n";
        }
    }
}

// Prompts user for menu choice within [minv, maxv], returns true on success
bool getMenuChoice(const string& prompt, int& out, int minv, int maxv) {
    string s;
    while (true) {
        cout << prompt;
        getline(cin, s);
        if (s == "b" || s == "B") return false; // Back option
        if (isDigits(s)) {
            long long val = 0;
            bool overflow = false;
            for (char c : s) {
                val = val * 10 + (c - '0');
                if (val > std::numeric_limits<int>::max()) {
                    overflow = true;
                    break;
                }
            }
            if (!overflow && val >= minv && val <= maxv) {
                out = static_cast<int>(val);
                return true;
            }
        }
        cout << "Invalid input. Enter a number between " << minv << " and " << maxv << ", or 'b' to go back.\n";
    }
}

// Adds a new product to the inventory
void addProduct() {
    Product p;
    p.id = getNextID(); // Assign new ID
    cout << "\nAssigned Product ID: " << p.id << endl;

    // Prompt for product details
    if (!getStringWithLetter("Enter Product Name (must contain a letter, or 'b' to go back): ", p.name)) return;
    if (!getStringWithLetter("Enter Product Category (e.g., Electronics, Apparel, etc., or 'b' to go back): ", p.category)) return;
    if (!getStrictIntPositive("Enter Quantity (number > 0, or 'b' to go back): ", p.quantity)) return;
    if (!getStrictDoublePositive("Enter Price (number > 0, or 'b' to go back): ", p.price)) return;

    products.push_back(p);         // Add to products vector
    saveProducts("products.txt");  // Save to file
    cout << "Product added successfully!\n";
}

// Updates stock for an existing product
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
    // Menu: stock in or stock out
    if (!getMenuChoice("1. Stock In\n2. Stock Out\nEnter choice (or 'b' to go back): ", opt, 1, 2)) return;

    int qty;
    if (!getStrictIntPositive("Enter quantity (number > 0, or 'b' to go back): ", qty)) return;

    if (opt == 1) {
        products[idx].quantity += qty; // Stock in
        cout << "Stock increased.\n";
    } else if (opt == 2) {
        if (qty > products[idx].quantity) {
            cout << "Not enough stock.\n";
            return;
        }
        products[idx].quantity -= qty; // Stock out
        cout << "Stock decreased.\n";
    }
    saveProducts("products.txt"); // Save changes
}

// Calculates and displays total value of all inventory
void inventoryValue() {
    double total = 0;
    for (const auto& p : products)
        total += p.quantity * p.price; // Sum up (qty * price) for each
    cout << "\nTotal Inventory Value: " << fixed << setprecision(2) << total << endl;
}

// Deletes a product by ID or name
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
    products.erase(products.begin() + idx); // Remove from vector
    saveProducts("products.txt");           // Save to file
    cout << "Product deleted successfully.\n";
}

// Inventory management menu for admin actions
void inventoryMenu() {
    loadProducts("products.txt"); // Load latest products
    while (true) {
        cout << "\n--- Inventory Management Panel ---\n";
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

// Helper for product category selection during order placement
vector<const Product*> getCatalogSelectionForOrder(bool& backSelected) {
    backSelected = false;
    loadProducts("products.txt"); // Ensure latest data
    if (products.empty()) {
        cout << "\n--- Catalog ---\nNo products available. Please ask admin to add products.\n";
        return {};
    }
    vector<string> categories = getCategories();
    while (true) {
        cout << "\n--- Product Categories ---\n";
        cout << "A. Show All Products\n";
        cout << "R. Suggest Me a Random Product\n";
        for (size_t i = 0; i < categories.size(); ++i) {
            cout << i + 1 << ". " << categories[i] << endl;
        }
        cout << "B. Back\n";
        cout << "Select category (enter number, 'A' for all, 'R' for random, or 'B' to go back): ";
        string catChoiceStr;
        getline(cin, catChoiceStr);

        if (catChoiceStr.length() == 1 && (catChoiceStr[0] == 'A' || catChoiceStr[0] == 'a')) {
            // Show all products
            vector<const Product*> toDisplay;
            for (const auto& p : products) toDisplay.push_back(&p);
            return toDisplay;
        } else if (catChoiceStr.length() == 1 && (catChoiceStr[0] == 'R' || catChoiceStr[0] == 'r')) {
            // Suggest random product
            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<> dis(0, products.size() - 1);
            int randIdx = dis(gen);
            cout << "\nRandom Product Suggestion:\n";
            cout << "Name: " << products[randIdx].name << endl;
            cout << "Category: " << products[randIdx].category << endl;
            cout << "Price: " << fixed << setprecision(2) << products[randIdx].price << endl;
            cout << "Stock: " << products[randIdx].quantity << endl;
            cout << "Do you want to add this product to your order? (Y/N): ";
            string yn;
            getline(cin, yn);
            if (yn.length() && (yn[0] == 'y' || yn[0] == 'Y')) {
                vector<const Product*> suggestion;
                suggestion.push_back(&products[randIdx]);
                return suggestion;
            } else {
                continue;
            }
        } else if (catChoiceStr.length() == 1 && (catChoiceStr[0] == 'B' || catChoiceStr[0] == 'b')) {
            // Back to previous menu
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

//Prompts user for payment method and handles E-Wallet confirmation
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
        cout << "2. E-Wallet (e.g., GCash, PayMaya)\n";
        cout << "Enter choice: ";
        string pc;
        getline(cin, pc);

        // Back option
        if (pc.length() == 1 && (pc[0] == 'B' || pc[0] == 'b')) {
            paymentMethod = "";
            return;
        }

        if (!isDigits(pc)) {
            cout << "Invalid input. Please enter a number or 'B' to go back.\n";
            continue;
        }
        paymentChoice = stoi(pc);
        if (paymentChoice == 1) {
            paymentMethod = "Cash";
            break;
        } else if (paymentChoice == 2) {
            paymentMethod = "E-Wallet";
            // Show payment info
            cout << "\nTotal amount to send: " << fixed << setprecision(2) << total << " pesos\n";
            cout << "Send to store account number: \n";
            cout << "09123456789\n";
            cout << "Yanex G.\n";
            char confirmation;
            int confirmCounter = 0;
            const int maxConfirmAttempts = 3;
            while (true) {
                if (confirmCounter >= maxConfirmAttempts) {
                    cout << "Maximum confirmation attempts reached. Assuming payment is sent.\n";
                    confirmation = 'Y';
                } else {
                    cout << "Have you sent the payment? (Y/N/B for back): ";
                    string confStr;
                    getline(cin, confStr);
                    if (confStr.empty()) continue;
                    char confChar = toupper(confStr[0]);
                    if (confChar == 'B') {
                        paymentMethod = "";
                        return;
                    }
                    confirmation = confChar;
                    confirmCounter++;
                }
                if (confirmation == 'Y') {
                    break;
                } else if (confirmation == 'N') {
                    cout << "Do you want to reselect the payment method? (Y/N): ";
                    string choice;
                    getline(cin, choice);
                    if (!choice.empty() && toupper(choice[0]) == 'Y') {
                        paymentMethod = "";
                        return;
                    }
                } else {
                    cout << "Invalid choice. Please enter Y, N, or B.\n";
                }
            }
            break;
        } else {
            cout << "Invalid choice. Try again.\n";
        }
    }
}

// Main function for placing an order
void placeOrder() {
    loadProducts("products.txt"); // Load latest products
    if (products.empty()) {
        cout << "No products available to order. Please ask admin to add products first.\n";
        return;
    }
    vector<int> cartQuantities(products.size(), 0); // Track each product's ordered qty
    double total = 0;
    string paymentMethod;
    bool orderCancelled = false;

    while (true) {
        bool backSelected = false;
        vector<const Product*> catalog = getCatalogSelectionForOrder(backSelected);
        if (backSelected) break;
        if (catalog.empty()) continue;

        // Show catalog
        cout << "\n--- Product Catalog ---\n";
        cout << left << setw(4) << "#" << setw(W_NAME) << "Name" << setw(W_CAT) << "Category"
             << setw(W_PRICE) << "Price" << setw(W_QTY) << "Stock" << endl;
        cout << string(4 + W_NAME + W_CAT + W_PRICE + W_QTY, '-') << endl;
        for (size_t i = 0; i < catalog.size(); ++i) {
            cout << left << setw(4) << (i + 1)
                 << setw(W_NAME) << catalog[i]->name
                 << setw(W_CAT) << catalog[i]->category
                 << setw(W_PRICE) << fixed << setprecision(2) << catalog[i]->price
                 << setw(W_QTY) << catalog[i]->quantity << endl;
        }

        // Add to cart loop
        while (true) {
            cout << "Enter item number to add to cart (0 to finish this catalog, 'b' to cancel order): ";
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
            if (itemNum < 1 || itemNum > (int)catalog.size()) {
                cout << "Invalid choice. Try again.\n";
                continue;
            }
            int catalogIdx = itemNum - 1;
            int prodIdx = -1;
            for (size_t i = 0; i < products.size(); ++i) {
                if (&products[i] == catalog[catalogIdx]) {
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
            cout << "Added to cart: " << products[prodIdx].name << " x" << quantity << endl;
        }
        if (orderCancelled) break;

        cout << "Do you want to add products from another category? (Y/N): ";
        string addMore;
        getline(cin, addMore);
        if (addMore.length() && (addMore[0] == 'y' || addMore[0] == 'Y')) {
            continue;
        } else {
            break;
        }
    }
    if (orderCancelled) {
        cout << "Order cancelled.\n";
        return;
    }
    bool hasOrder = false;
    for (int qty : cartQuantities) if (qty > 0) hasOrder = true;
    if (!hasOrder) {
        cout << "No products were added to your cart. Order cancelled.\n";
        return;
    }

    // Print order summary
    cout << "\n--- Order Summary ---\n";
    cout << left << setw(25) << "Product" << setw(10) << "Qty" << setw(20) << "Total Cost" << endl;
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

    cout << "Do you want to confirm your order? (Y/N): ";
    string confirmOrder;
    getline(cin, confirmOrder);
    if (!(confirmOrder.length() && (confirmOrder[0] == 'y' || confirmOrder[0] == 'Y'))) {
        cout << "Order cancelled.\n";
        return;
    }

    // Deduct ordered quantities from stock
    for (size_t i = 0; i < products.size(); ++i) {
        if (cartQuantities[i] > 0) {
            products[i].quantity -= cartQuantities[i];
        }
    }
    saveProducts("products.txt"); // Save new stock

    // Payment method selection (loop for back option)
    do {
        selectPaymentMethod(paymentMethod, total);
        if (paymentMethod.empty()) {
            cout << "Returning to payment method selection as per your request.\n";
        }
    } while (paymentMethod.empty());

    cout << "Thank you for your order!\n";

    // Print receipt for the customer
    cout << "\nYanex Store Receipt\n";
    time_t now = time(0);
    tm *ltm = localtime(&now);
    cout << "Date: "
         << 1900 + ltm->tm_year << "-"
         << setw(2) << setfill('0') << 1 + ltm->tm_mon << "-"
         << setw(2) << setfill('0') << ltm->tm_mday << " "
         << setw(2) << setfill('0') << ltm->tm_hour << ":"
         << setw(2) << setfill('0') << ltm->tm_min << ":"
         << setw(2) << setfill('0') << ltm->tm_sec << endl;
    // Reset fill character to space for table output
    cout << setfill(' ');
    cout << "-----------------------------\n";
    for (size_t i = 0; i < products.size(); ++i) {
        if (cartQuantities[i] > 0) {
            double cost = cartQuantities[i] * products[i].price;
            cout << "Item: " << products[i].name << endl;
            cout << "Qty: " << cartQuantities[i] << endl;
            cout << "Subtotal: " << fixed << setprecision(2) << cost << " Pesos" << endl;
            cout << endl;
        }
    }
    cout << "-----------------------------\n";
    cout << "Total: " << fixed << setprecision(2) << total << " pesos\n";
    cout << "Payment Method: " << paymentMethod << endl;
    cout << "\nThank you for shopping with us!\n";
}

// --- Main menu for the product ordering system ---
int main() {
    string choice;
    do {
        cout << "\n--- Product Ordering System ---\n";
        cout << "1. View Product Catalog\n";
        cout << "2. Buy Product(s)\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        getline(cin, choice);
        if (choice == "admin") {
            inventoryMenu();
            continue;
        }
        if (!isDigits(choice)) {
            cout << "Invalid choice. Try again.\n";
            continue;
        }
        int menuChoice = stoi(choice);

        switch (menuChoice) {
            case 1:
                displayProducts();
                break;
            case 2:
                placeOrder();
                break;
            case 0:
                cout << "Thank you for using the Product Ordering System!\n";
                return 0;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    } while (true);
    return 0;
}