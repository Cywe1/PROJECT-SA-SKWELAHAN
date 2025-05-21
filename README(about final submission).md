# Product Ordering System

A simple C++ console application for managing a product inventory and processing customer orders. This system can be used by both customers (to view and buy products) and administrators (to manage inventory).

## Features

- **Product Catalog**: View all available products in a formatted table.
- **Order Placement**: Customers can browse products by category, add products to cart, and place orders.
- **Payment Methods**: Supports Cash and E-Wallet (e.g., GCash, PayMaya) payments.
- **Admin Panel**: Accessible by typing `admin` at the main menu.
    - Add new products
    - Update product stock (Stock In/Out)
    - Search products
    - Display all products
    - Calculate total inventory value
    - Delete products

## Getting Started

### Prerequisites

- C++ compiler (e.g., g++, clang++)
- [Optional] Makefile or build system for convenience

### Compilation

```sh
g++ -o ProductOrderingSystem FinalSubmission.cpp
```

### Running

```sh
./ProductOrderingSystem
```

## Usage

### Main Menu

Upon running the program, you'll see the main menu:

```
--- Product Ordering System ---
1. View Product Catalog
2. Buy Product(s)
0. Exit
Enter choice:
```

- **View Product Catalog**: Displays all products.
- **Buy Product(s)**: Place an order as a customer.
- **Admin Panel**: Enter `admin` at the prompt to access inventory management features.

### Admin Panel

The admin panel allows you to add, update, delete, and search products, as well as calculate inventory value.

### Data Storage

- Product data is saved in `products.txt` in the application directory.
- Each product has an ID, name, category, quantity, and price.
- Data persists between sessions.

## File Structure

- `FinalSubmission.cpp` — Main application code
- `products.txt` — Product data file (generated/modified by the app)

## Example Product Data Format (products.txt)

```
1|Laptop|Electronics|10|35000.0
2|T-Shirt|Apparel|50|350.0
```

## Notes

- Input validation is performed throughout the application to ensure robust operation.
- The admin panel is not password protected (for demo purposes).
- The application uses only standard C++ libraries.

## License

This project is provided as-is for educational purposes.
