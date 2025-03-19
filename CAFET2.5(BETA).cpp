#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring> // For C-strings

using namespace std;

const int SIZE = 6;
char menuItems[SIZE][30] = {"Sisig with pilok", "Buko with magic sugar", "Java rice", "Tempura nga high class", "Melon Juice", "Iced Coffee"};
double prices[SIZE] = {50, 10, 50, 20, 15, 25};
int stock[SIZE] = {10, 5, 7, 20, 50, 50}; // Updated stock for new items
char categories[SIZE][15] = {"Food", "Drink", "Food", "Food", "Drink", "Drink"};

double grossIncome = 0.0; // Global variable to track the gross income

// Function to display the menu
void displayMenu() {
    cout << "\n--- Menu ---\n";
    for (int i = 0; i < SIZE; i++) {
        cout << i + 1 << ". " << menuItems[i] << " - " << fixed << setprecision(2) << prices[i]
             << " pesos (Stock: " << stock[i] << ")\n";
    }
}

// Function to generate and save the bill
void generateBill(const int orders[], const char paymentMethod[]) {
    double total = 0;
    ofstream receiptFile("receipt.txt", ios::app);
    if (!receiptFile) {
        cout << "Error opening receipt file.\n";
        return;
    }

    receiptFile << "\n--- New Bill Summary ---\n";
    receiptFile << left << setw(25) << "Item" << setw(10) << "Qty" << setw(20) << "Total Cost" << endl;
    receiptFile << "------------------------------------------------\n";

    cout << "\n--- Bill Summary ---\n";
    cout << left << setw(25) << "Item" << setw(10) << "Qty" << setw(20) << "Total Cost" << endl;
    cout << "------------------------------------------------\n";

    bool hasOrder = false;
    for (int i = 0; i < SIZE; i++) {
        if (orders[i] > 0) {
            double cost = orders[i] * prices[i]; // Calculating cost based on quantity and price
            total += cost;
            cout << left << setw(25) << menuItems[i] << setw(10) << orders[i] << cost << " pesos" << endl;
            receiptFile << left << setw(25) << menuItems[i] << setw(10) << orders[i] << cost << " pesos" << endl;
            hasOrder = true;
        }
    }

    if (hasOrder) {
        cout << "------------------------------------------------\n";
        cout << "Grand Total: " << fixed << setprecision(2) << total << " pesos" << endl;
        cout << "Payment Method: " << paymentMethod << "\n";

        receiptFile << "------------------------------------------------\n";
        receiptFile << "Grand Total: " << fixed << setprecision(2) << total << " pesos" << endl;
        receiptFile << "Payment Method: " << paymentMethod << "\n";
        grossIncome += total;
    }
    receiptFile.close();
}

// Function to handle payment method selection
void selectPaymentMethod(char paymentMethod[], double total) {
    int paymentChoice;
    do {
        cout << "\nSelect Payment Method:\n";
        cout << "1. Cash\n";
        cout << "2. GCash/PayMaya\n";
        cout << "Enter choice: ";
        cin >> paymentChoice;

        if (paymentChoice == 1) {
            strcpy(paymentMethod, "Cash");
            break;
        } else if (paymentChoice == 2) {
            strcpy(paymentMethod, "GCash/PayMaya");
            cout << "\nTotal amount to send: " << fixed << setprecision(2) << total << " pesos\n"; // Inform user about the total cost
            cout << "09123456789  YANEX GWAPO P.\n";
            char confirmation;
            do {
                cout << "Have you sent the payment? (Y/N): ";
                cin >> confirmation;
                confirmation = toupper(confirmation);
            } while (confirmation != 'Y');
            break;
        } else {
            cout << "Invalid choice. Try again.\n";
        }
    } while (true);
}

// Function to place an order
void placeOrder() {
    char choice;
    int itemChoice, quantity;
    char paymentMethod[20];
    double total = 0;

    int tempOrders[SIZE] = {0}; // Temporary orders array for user to confirm before finalizing
    int orders[SIZE] = {0}; // Reset orders array for each user

    do {
        displayMenu();
        cout << "Enter item number to order (0 to finish, 'b' to go back): ";
        cin >> choice;

        if (choice == '0') break;
        if (choice == 'b') return;
        itemChoice = choice - '0';
        if (itemChoice < 1 || itemChoice > SIZE) {
            cout << "Invalid choice. Try again.\n";
            continue;
        }

        cout << "Enter quantity: ";
        cin >> quantity;

        int index = itemChoice - 1;
        if (quantity <= 0 || quantity > stock[index]) {
            cout << "Invalid quantity or not enough stock.\n";
            continue;
        }

        stock[index] -= quantity;
        tempOrders[index] += quantity;
        total += prices[index] * quantity; // Adding price to total
        cout << "Order added successfully!\n";
    } while (true);

    // Confirm orders before proceeding to payment
    char confirm;
    do {
        cout << "\n--- Order Summary ---\n";
        for (int i = 0; i < SIZE; i++) {
            if (tempOrders[i] > 0) {
                cout << i + 1 << ". " << menuItems[i] << " - " << tempOrders[i] << " pcs\n";
            }
        }
        cout << "Do you want to discard any item? (Y/N): ";
        cin >> confirm;

        if (toupper(confirm) == 'Y') {
            do {
                cout << "Enter item number to discard (0 to finish): ";
                cin >> itemChoice;

                if (itemChoice == 0) break;
                if (itemChoice < 1 || itemChoice > SIZE || tempOrders[itemChoice - 1] == 0) {
                    cout << "Invalid choice. Try again.\n";
                    continue;
                }

                int index = itemChoice - 1;
                stock[index] += tempOrders[index]; // Revert stock
                total -= prices[index] * tempOrders[index]; // Revert total
                tempOrders[index] = 0;
                cout << "Item discarded successfully!\n";
            } while (true);

            // Check if all items are discarded
            bool allDiscarded = true;
            for (int i = 0; i < SIZE; i++) {
                if (tempOrders[i] > 0) {
                    allDiscarded = false;
                    break;
                }
            }

            // If all items are discarded, go back to menu
            if (allDiscarded) {
                cout << "All items discarded. Returning to menu.\n";
                return;
            }
        }

        // Ask for confirmation again after discarding
        cout << "Do you want to confirm your orders? (Y/N): ";
        cin >> confirm;
    } while (toupper(confirm) != 'Y' && toupper(confirm) != 'N');

    if (toupper(confirm) == 'Y') {
        for (int i = 0; i < SIZE; i++) {
            orders[i] += tempOrders[i];
        }
        selectPaymentMethod(paymentMethod, total);
        generateBill(orders, paymentMethod);
    } else {
        // Revert stock if the user does not confirm the orders
        for (int i = 0; i < SIZE; i++) {
            stock[i] += tempOrders[i];
        }
        cout << "Order discarded.\n";
    }
}

// Function to display the admin panel
void adminPanel() {
    int choice;
    do {
        cout << "\n--- Admin Panel ---\n";
        cout << "1. Update Stock\n";
        cout << "2. View Order History\n";
        cout << "3. View Gross Income\n";
        cout << "0. Exit Admin Panel\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            int itemIndex, newStock;
            displayMenu();
            cout << "Enter item number to update stock: ";
            cin >> itemIndex;
            if (itemIndex < 1 || itemIndex > SIZE) {
                cout << "Invalid item number.\n";
                continue;
            }
            cout << "Enter new stock for " << menuItems[itemIndex - 1] << ": ";
            cin >> newStock;
            stock[itemIndex - 1] = newStock;
            cout << "Stock updated successfully!\n";
        } else if (choice == 2) {
            ifstream receiptFile("receipt.txt");
            if (!receiptFile) {
                cout << "Error opening receipt file.\n";
                continue;
            }
            cout << "\n--- Order History ---\n";
            string line;
            while (getline(receiptFile, line)) {
                cout << line << endl;
            }
            receiptFile.close();
        } else if (choice == 3) {
            cout << "\nGross Income: " << fixed << setprecision(2) << grossIncome << " pesos\n";
        } else if (choice == 0) {
            break;
        } else {
            cout << "Invalid choice. Try again.\n";
        }
    } while (true);
}

// Main function
int main() {
    char choice[20];

    do {
        cout << "\n--- Cafeteria Ordering System ---\n";
        cout << "1. View Menu\n";
        cout << "2. Place Order\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (strcmp(choice, "1") == 0) {
            displayMenu();
        } else if (strcmp(choice, "2") == 0) {
            placeOrder();
        } else if (strcmp(choice, "0") == 0) {
            cout << "Thank you for using the Cafeteria Ordering System!\n";
            break;
        } else if (strcmp(choice, "admin") == 0) { // Hidden admin panel access
            adminPanel();
        } else {
            cout << "Invalid choice. Try again.\n";
        }
    } while (true);

    return 0;
}
