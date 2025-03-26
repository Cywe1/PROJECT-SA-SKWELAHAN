#include <iostream>
#include <iomanip>
#include <cstring> // For C-strings
#include <limits>

using namespace std;

const int MAX_SIZE = 20; // Maximum size of the menu
int SIZE = 6; // Current number of items
char menuItems[MAX_SIZE][30] = {"Sisig with pilok", "Buko with magic sugar", "Java rice", "Tempura nga high class", "Melon Juice", "Iced Coffee"};
double prices[MAX_SIZE] = {50, 10, 50, 20, 15, 25};
int stock[MAX_SIZE] = {10, 5, 7, 20, 50, 50}; // Updated stock for new items
char categories[MAX_SIZE][15] = {"Food", "Drink", "Food", "Food", "Drink", "Drink"};

double grossIncome = 0.0; // Global variable to track the gross income

// Order history arrays
const int MAX_ORDERS = 100;
char orderItems[MAX_ORDERS][30];
int orderQuantities[MAX_ORDERS];
double orderCosts[MAX_ORDERS];
char orderPaymentMethods[MAX_ORDERS][20];
int orderCount = 0; // Total number of orders recorded

// Function to display the menu
void displayMenu() {
    cout << "\n--- Menu ---\n";
    for (int i = 0; i < SIZE; i++) {
        cout << i + 1 << ". " << menuItems[i] << " - " << fixed << setprecision(2) << prices[i]
             << " pesos (Stock: " << stock[i] << ")\n";
    }
}

// Function to generate and display the bill
void generateBill(const int orders[], const char paymentMethod[]) {
    double total = 0;

    cout << "\n--- Bill Summary ---\n";
    cout << left << setw(25) << "Item" << setw(10) << "Qty" << setw(20) << "Total Cost" << endl;
    cout << "------------------------------------------------\n";

    bool hasOrder = false;
    for (int i = 0; i < SIZE; i++) {
        if (orders[i] > 0) {
            double cost = orders[i] * prices[i]; // Calculating cost based on quantity and price
            total += cost;
            cout << left << setw(25) << menuItems[i] << setw(10) << orders[i] << cost << " pesos" << endl;
            hasOrder = true;

            // Store order in history
            strcpy(orderItems[orderCount], menuItems[i]);
            orderQuantities[orderCount] = orders[i];
            orderCosts[orderCount] = cost;
            strcpy(orderPaymentMethods[orderCount], paymentMethod);
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

// Function to handle payment method selection
void selectPaymentMethod(char paymentMethod[], double total) {
    int paymentChoice;
    int attemptCounter = 0;
    const int maxAttempts = 3; // Set maximum attempts to avoid infinite loop

    while (true) {
        if (attemptCounter >= maxAttempts) {
            cout << "Maximum attempts reached. Defaulting to Cash payment.\n";
            strcpy(paymentMethod, "Cash");
            break;
        }
        attemptCounter++;
        
        cout << "\nSelect Payment Method:\n";
        cout << "1. Cash\n";
        cout << "2. GCash/PayMaya\n";
        cout << "Enter choice: ";
        cin >> paymentChoice;

        if (cin.fail()) {
            cin.clear(); // Clear the error state of cin
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        if (paymentChoice == 1) {
            strcpy(paymentMethod, "Cash");
            break;
        } else if (paymentChoice == 2) {
            strcpy(paymentMethod, "GCash/PayMaya");
            cout << "\nTotal amount to send: " << fixed << setprecision(2) << total << " pesos\n"; // Inform user about the total cost
            cout << "09123456789  YANEX GWAPO P.\n";
            char confirmation;
            int confirmCounter = 0;
            const int maxConfirmAttempts = 3; // Set maximum confirmation attempts to avoid infinite loop
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

// Function to place an order
void placeOrder() {
    char choice;
    int itemChoice, quantity;
    char paymentMethod[20];
    double total = 0;

    int tempOrders[MAX_SIZE] = {0}; // Temporary orders array for user to confirm before finalizing
    int orders[MAX_SIZE] = {0}; // Reset orders array for each user

    int orderAttemptCounter = 0;
    const int maxOrderAttempts = 10; // Set maximum order attempts to avoid infinite loop

    do {
        if (orderAttemptCounter >= maxOrderAttempts) {
            cout << "Maximum order attempts reached. Returning to main menu.\n";
            return;
        }
        orderAttemptCounter++;

        displayMenu();
        cout << "Enter item number to order (0 to finish, 'b' to go back): ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear(); // Clear the error state of cin
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
            cout << "Invalid input. Please enter a valid item number.\n";
            continue;
        }

        if (choice == '0') break;
        if (choice == 'b') return;
        itemChoice = choice - '0';
        if (itemChoice < 1 || itemChoice > SIZE) {
            cout << "Invalid choice. Try again.\n";
            continue;
        }

        cout << "Enter quantity: ";
        cin >> quantity;

        if (cin.fail()) {
            cin.clear(); // Clear the error state of cin
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

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
    int confirmOrderCounter = 0;
    const int maxConfirmOrderAttempts = 3; // Set maximum confirmation attempts to avoid infinite loop

    do {
        if (confirmOrderCounter >= maxConfirmOrderAttempts) {
            cout << "Maximum confirmation attempts reached. Discarding order.\n";
            // Revert stock if the user does not confirm the orders
            for (int i = 0; i < SIZE; i++) {
                stock[i] += tempOrders[i];
            }
            cout << "Order discarded.\n";
            return;
        }
        confirmOrderCounter++;

        cout << "\n--- Order Summary ---\n";
        for (int i = 0; i < SIZE; i++) {
            if (tempOrders[i] > 0) {
                cout << i + 1 << ". " << menuItems[i] << " - " << tempOrders[i] << " pcs\n";
            }
        }
        cout << "Do you want to discard any item? (Y/N): ";
        cin >> confirm;

        if (cin.fail()) {
            cin.clear(); // Clear the error state of cin
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
            cout << "Invalid input. Please enter Y or N.\n";
            continue;
        }

        if (toupper(confirm) == 'Y') {
            int discardCounter = 0;
            const int maxDiscardAttempts = 3; // Set maximum discard attempts to avoid infinite loop
            do {
                if (discardCounter >= maxDiscardAttempts) {
                    cout << "Maximum discard attempts reached. Proceeding with the remaining items.\n";
                    break;
                }
                discardCounter++;

                cout << "Enter item number to discard (0 to finish): ";
                cin >> itemChoice;

                if (cin.fail()) {
                    cin.clear(); // Clear the error state of cin
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
                    cout << "Invalid input. Please enter a number.\n";
                    continue;
                }

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

        if (cin.fail()) {
            cin.clear(); // Clear the error state of cin
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
            cout << "Invalid input. Please enter Y or N.\n";
            continue;
        }
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
        cout << "2. View Gross Income\n";
        cout << "3. Add New Item\n"; // New option to add an item
        cout << "4. Remove Item\n"; // New option to remove an item
        cout << "5. View Order History\n"; // Option to view order history
        cout << "0. Exit Admin Panel\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear(); // Clear the error state of cin
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        if (choice == 1) { // Update stock
            displayMenu();
            int itemIndex, newStock;
            cout << "Enter item number to update stock (0 to go back): ";
            cin >> itemIndex;

            if (cin.fail()) {
                cin.clear(); // Clear the error state of cin
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
                cout << "Invalid input. Please enter a number.\n";
                continue;
            }

            if (itemIndex == 0) continue;
            if (itemIndex < 1 || itemIndex > SIZE) {
                cout << "Invalid item number.\n";
                continue;
            }
            cout << "Enter new stock for " << menuItems[itemIndex - 1] << ": ";
            cin >> newStock;

            if (cin.fail()) {
                cin.clear(); // Clear the error state of cin
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
                cout << "Invalid input. Please enter a number.\n";
                continue;
            }

            stock[itemIndex - 1] = newStock;
            cout << "Stock updated successfully!\n";
        } else if (choice == 2) { // View gross income
            cout << "\nGross Income: " << fixed << setprecision(2) << grossIncome << " pesos\n";
        } else if (choice == 3) { // Add new item
            if (SIZE < MAX_SIZE) {
                char newItem[30];
                double newPrice;
                int newStock;
                cout << "Enter new item name (0 to cancel): ";
                cin.ignore(); // Clear newline
                cin.getline(newItem, 30);
                if (strcmp(newItem, "0") == 0) continue;
                cout << "Enter price: ";
                cin >> newPrice;
                if (newPrice == 0) continue;
                cout << "Enter stock: ";
                cin >> newStock;
                if (newStock == 0) continue;

                strcpy(menuItems[SIZE], newItem);
                prices[SIZE] = newPrice;
                stock[SIZE] = newStock;
                SIZE++; // Increase the size of the menu
                cout << "Item added successfully!\n";
            } else {
                cout << "Menu is full. Cannot add more items.\n";
            }
        } else if (choice == 4) { // Remove item
            displayMenu();
            int itemIndex;
            cout << "Enter the item number to remove (0 to go back): ";
            cin >> itemIndex;

            if (cin.fail()) {
                cin.clear(); // Clear the error state of cin
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
                cout << "Invalid input. Please enter a number.\n";
                continue;
            }

            if (itemIndex == 0) continue;
            if (itemIndex < 1 || itemIndex > SIZE) {
                cout << "Invalid item number.\n";
                continue;
            }

            // Shift items to remove the selected item
            for (int i = itemIndex - 1; i < SIZE - 1; i++) {
                strcpy(menuItems[i], menuItems[i + 1]);
                prices[i] = prices[i + 1];
                stock[i] = stock[i + 1];
            }

            SIZE--; // Decrease the size of the menu
            cout << "Item removed successfully!\n";
        } else if (choice == 5) { // View order history
            cout << "\n--- Order History ---\n";
            for (int i = 0; i < orderCount; i++) {
                cout << left << setw(25) << orderItems[i] << setw(10) << orderQuantities[i] << orderCosts[i] << " pesos (Payment Method: " << orderPaymentMethods[i] << ")\n";
            }
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

        if (cin.fail()) {
            cin.clear(); // Clear the error state of cin
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
            cout << "Invalid input. Please enter a valid choice.\n";
            continue;
        }

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
