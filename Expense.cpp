
#include "Expense.h"

Expense::Expense(int idValue,
                 double amountValue,
                 const std::string& categoryValue,
                 const std::string& dateValue,
                 const std::string& descriptionValue)
    : id(idValue),
      amount(amountValue),
      category(categoryValue),
      date(dateValue),
      description(descriptionValue) {
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
