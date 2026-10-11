#ifndef EXPENSE_H
#define EXPENSE_H

#include <string>

class Expense {
private:
    int id;
    double amount;
    std::string category;
    std::string date;
    std::string description;

public:
    Expense(int idValue,
            double amountValue,
            const std::string& categoryValue,
            const std::string& dateValue,
            const std::string& descriptionValue);

    int getId() const;
    double getAmount() const;
    const std::string& getCategory() const;
    const std::string& getDate() const;
    const std::string& getDescription() const;
};

#endif