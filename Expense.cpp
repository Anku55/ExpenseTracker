
#include "Expense.h"

Expense::Expense(int id,
                 double amount,
                 const std::string& category,
                 const std::string& date,
                 const std::string& description)
    : id(id),
      amount(amount),
      category(category),
      date(date),
      description(description) {
}

int Expense::getId() const {
    return id;
}

double Expense::getAmount() const {
    return amount;
}

const std::string& Expense::getCategory() const {
    return category;
}

const std::string& Expense::getDate() const {
    return date;
}

const std::string& Expense::getDescription() const {
    return description;
}
