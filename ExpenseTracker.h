
#ifndef EXPENSE_TRACKER_H
#define EXPENSE_TRACKER_H

#include <vector>
#include "Expense.h"

class ExpenseTracker {
private:
    std::vector<Expense> expenses;
    int nextId;
    bool persistenceSafe;

public:
    ExpenseTracker();

    void addExpense();
    void viewAllExpenses();
    void monthlyExpenseSummary();
    void categoryWiseAnalysis();

    void loadExpenses();
    bool saveExpenses();
};

#endif
