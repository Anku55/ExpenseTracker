
#include "ExpenseTracker.h"

#include <iostream>
#include <sstream>
#include <string>

namespace {

bool parseMenuChoice(const std::string& input, int& choice) {
    std::istringstream stream(input);
    char extra;

    if (!(stream >> choice)) {
        return false;
    }

    // Allow surrounding whitespace, but reject extra non-whitespace text.
    if (stream >> extra) {
        return false;
    }

    return true;
}

} // namespace

int main() {
    ExpenseTracker tracker;
    tracker.loadExpenses();

    while (true) {
        std::cout << "\n===== Expense Tracker =====\n"
                  << "1. Add Expense\n"
                  << "2. View All Expenses\n"
                  << "3. Monthly Expense Summary\n"
                  << "4. Category-wise Analysis\n"
                  << "5. Exit\n"
                  << "Enter your choice: ";

        std::string input;

        if (!std::getline(std::cin, input)) {
            std::cout << "\nInput ended. Exiting Expense Tracker.\n";
            break;
        }

        int choice = 0;

        if (!parseMenuChoice(input, choice)) {
            std::cout
                << "Invalid choice. Enter a number from 1 to 5.\n";
            continue;
        }

        if (choice < 1 || choice > 5) {
            std::cout
                << "Invalid choice. Enter a number from 1 to 5.\n";
            continue;
        }

        switch (choice) {
            case 1:
                tracker.addExpense();
                break;

            case 2:
                tracker.viewAllExpenses();
                break;

            case 3:
                tracker.monthlyExpenseSummary();
                break;

            case 4:
                tracker.categoryWiseAnalysis();
                break;

            case 5:
                std::cout << "Exiting Expense Tracker. Goodbye!\n";
                return 0;
        }
    }

    return 0;
}
