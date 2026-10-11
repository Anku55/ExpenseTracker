
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
    Expense(int id,
            double amount,
            const std::string& category,
            const std::string& date,
            const std::string& description);

    int getId() const;
    double getAmount() const;
    const std::string& getCategory() const;
    const std::string& getDate() const;
    const std::string& getDescription() const;
};

#endif
