
#include <iostream>
#include <iomanip>
#include "Expense.h"

int main() {
    Expense expense(
        1,
        250.50,
        "Food",
        "2026-10-11",
        "Lunch with friends"
    );

    std::cout << "Expense Tracker - Milestone 1\n";
    std::cout << "------------------------------\n";

    std::cout << "ID: " << expense.getId() << '\n';
    std::cout << "Date: " << expense.getDate() << '\n';
    std::cout << "Category: " << expense.getCategory() << '\n';

    std::cout << "Amount: Rs. "
              << std::fixed << std::setprecision(2)
              << expense.getAmount() << '\n';

    std::cout << "Description: "
              << expense.getDescription() << '\n';

    return 0;
}
