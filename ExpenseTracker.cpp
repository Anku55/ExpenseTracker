

#include "ExpenseTracker.h"

#include <cerrno>

#include <cmath>

#include <cstdio>

#include <fstream>

#include <iomanip>

#include <iostream>

#include <limits>
#include <map>

#include <set>

#include <sstream>

#include <string>

namespace {

const char* DATA_FILE = "expenses.txt";

const char* TEMP_FILE = "expenses.txt.tmp";

const char* BACKUP_FILE = "expenses.txt.bak";

const char* FILE_HEADER = "EXPENSE_TRACKER_V1";

std::string trim(const std::string& text) {

    const std::string whitespace = " \t\n\r";

    const std::size_t start = text.find_first_not_of(whitespace);

    if (start == std::string::npos) {

        return "";

    }

    const std::size_t end = text.find_last_not_of(whitespace);

    return text.substr(start, end - start + 1);

}

bool isValidDate(const std::string& date) {

    if (date.size() != 10 || date[4] != '-' || date[7] != '-') {

        return false;

    }

    for (std::size_t i = 0; i < date.size(); ++i) {

        if (i == 4 || i == 7) {

            continue;

        }

        if (date[i] < '0' || date[i] > '9') {

            return false;

        }

    }

    const int year = std::stoi(date.substr(0, 4));

    const int month = std::stoi(date.substr(5, 2));

    const int day = std::stoi(date.substr(8, 2));

    if (year < 1 || month < 1 || month > 12) {

        return false;

    }

    const int daysInMonth[] = {

        31, 28, 31, 30, 31, 30,

        31, 31, 30, 31, 30, 31

    };

    int maxDay = daysInMonth[month - 1];

    const bool isLeapYear =

        (year % 400 == 0) ||

        (year % 4 == 0 && year % 100 != 0);

    if (month == 2 && isLeapYear) {

        maxDay = 29;

    }

    return day >= 1 && day <= maxDay;

}

bool parseAmount(const std::string& input, double& amount) {

    std::istringstream stream(input);

    double value;

    char extra;

    if (!(stream >> value)) {

        return false;

    }

    if (stream >> extra) {

        return false;

    }

    if (!std::isfinite(value) || value <= 0.0 ||

        value > 1000000000.0) {

        return false;

    }

    amount = value;

    return true;

}

bool parseInteger(const std::string& input, int& value) {

    std::istringstream stream(input);

    char extra;

    if (!(stream >> value) || (stream >> extra)) {

        return false;

    }

    return true;

}

bool parseStoredAmount(const std::string& input, double& amount) {

    return parseAmount(input, amount);

}

void writeLengthPrefixed(std::ostream& output,

                         const std::string& value) {

    output << value.size() << ':' << value;

}

bool readLengthPrefixed(const std::string& line,

                        std::size_t& position,

                        std::string& value) {

    const std::size_t colon = line.find(':', position);

    if (colon == std::string::npos || colon == position) {

        return false;

    }

    std::size_t length = 0;

    for (std::size_t i = position; i < colon; ++i) {

        const char ch = line[i];

        if (ch < '0' || ch > '9') {

            return false;

        }

        const std::size_t digit =

            static_cast<std::size_t>(ch - '0');

        if (length >

            (std::numeric_limits<std::size_t>::max() - digit) / 10) {

            return false;

        }

        length = length * 10 + digit;

    }

    position = colon + 1;

    if (length > line.size() - position) {

        return false;

    }

    value = line.substr(position, length);

    position += length;

    return true;

}

bool parseRecord(const std::string& line, Expense& expense) {

    const std::size_t firstSeparator = line.find('|');

    if (firstSeparator == std::string::npos) {

        return false;

    }

    const std::size_t secondSeparator =

        line.find('|', firstSeparator + 1);

    if (secondSeparator == std::string::npos) {

        return false;

    }

    int id;

    if (!parseInteger(line.substr(0, firstSeparator), id) ||

        id <= 0 ||

        id == std::numeric_limits<int>::max()) {

        return false;

    }

    double amount;

    if (!parseStoredAmount(

            line.substr(firstSeparator + 1,

                        secondSeparator - firstSeparator - 1),

            amount)) {

        return false;

    }

    std::size_t position = secondSeparator + 1;

    std::string category;

    std::string date;

    std::string description;

    if (!readLengthPrefixed(line, position, category) ||

        !readLengthPrefixed(line, position, date) ||

        !readLengthPrefixed(line, position, description)) {

        return false;

    }

    if (position != line.size() ||

        trim(category).empty() ||

        !isValidDate(date) ||

        trim(description).empty()) {

        return false;

    }

    expense = Expense(id, amount, category, date, description);

    return true;

}

// Returns false only when file status cannot be determined safely.
// When true is returned, exists tells whether the file is present.
bool inspectFile(const char* path, bool& exists) {
    errno = 0;
    std::FILE* file = std::fopen(path, "rb");

    if (file != nullptr) {
        if (std::fclose(file) != 0) {
            return false;
        }
        exists = true;
        return true;
    }

    if (errno == ENOENT) {
        exists = false;
        return true;
    }

    return false;
}

} // namespace

ExpenseTracker::ExpenseTracker()

    : nextId(1), persistenceSafe(true) {

}

void ExpenseTracker::addExpense() {

    std::string input;

    double amount;

    std::cout << "\n===== Add Expense =====\n";

    std::cout << "Enter amount: ";

    if (!std::getline(std::cin, input)) {
        std::cout << "Input ended. Returning to the menu.\n";
        return;
    }

    if (!parseAmount(input, amount)) {

        std::cout

            << "Invalid amount. Enter a number greater than 0 "

            << "and at most 1000000000.\n";

        return;

    }

    std::string category;

    std::cout << "Enter category: ";

    if (!std::getline(std::cin, category)) {
        std::cout << "Input ended. Returning to the menu.\n";
        return;
    }

    category = trim(category);

    if (category.empty()) {

        std::cout << "Category cannot be empty.\n";

        return;

    }

    std::string date;

    std::cout << "Enter date (YYYY-MM-DD): ";

    if (!std::getline(std::cin, date)) {
        std::cout << "Input ended. Returning to the menu.\n";
        return;
    }

    if (!isValidDate(date)) {

        std::cout

            << "Invalid date. Enter a valid date in YYYY-MM-DD format.\n";

        return;

    }

    std::string description;

    std::cout << "Enter description: ";

    if (!std::getline(std::cin, description)) {
        std::cout << "Input ended. Returning to the menu.\n";
        return;
    }

    description = trim(description);

    if (description.empty()) {

        std::cout << "Description cannot be empty.\n";

        return;

    }

    if (!persistenceSafe) {

        std::cout

            << "Cannot add expense: the data file could not be loaded "

            << "safely. Repair or restore expenses.txt first.\n";

        return;

    }

    if (nextId <= 0 ||

        nextId == std::numeric_limits<int>::max()) {

        std::cout << "Cannot add expense: no more IDs are available.\n";

        return;

    }

    expenses.emplace_back(

        nextId, amount, category, date, description

    );

    if (!saveExpenses()) {

        expenses.pop_back();

        std::cout

            << "Expense was not added because saving failed. "

            << "Existing records have been retained where possible.\n";

        return;

    }

    std::cout << "Expense added and saved successfully. ID: "

              << nextId << '\n';

    ++nextId;

}

void ExpenseTracker::viewAllExpenses() {

    std::cout << "\n===== All Expenses =====\n";

    if (expenses.empty()) {

        std::cout << "No expenses recorded yet.\n";

        return;

    }

    std::cout << std::fixed << std::setprecision(2);

    std::cout

        << std::left

        << std::setw(6) << "ID"

        << std::setw(13) << "Date"

        << std::setw(20) << "Category"

        << std::right << std::setw(14) << "Amount"

        << "  "

        << std::left << "Description"

        << '\n';

    std::cout << std::string(85, '-') << '\n';

    for (const Expense& expense : expenses) {

        std::cout

            << std::left

            << std::setw(6) << expense.getId()

            << std::setw(13) << expense.getDate()

            << std::setw(20) << expense.getCategory()

            << std::right << std::setw(14) << expense.getAmount()

            << "  "

            << std::left << expense.getDescription()

            << '\n';

    }

}

void ExpenseTracker::monthlyExpenseSummary() {

    std::cout << "\n===== Monthly Expense Summary =====\n";

    if (expenses.empty()) {

        std::cout << "No expenses recorded yet.\n";

        return;

    }

    std::string input;

    int month;

    int year;

    std::cout << "Enter month (1-12): ";

    if (!std::getline(std::cin, input)) {

        std::cout << "Input ended. Returning to the menu.\n";

        return;

    }

    if (!parseInteger(input, month) || month < 1 || month > 12) {

        std::cout << "Invalid month. Enter a number from 1 to 12.\n";

        return;

    }

    std::cout << "Enter year (1-9999): ";

    if (!std::getline(std::cin, input)) {

        std::cout << "Input ended. Returning to the menu.\n";

        return;

    }

    if (!parseInteger(input, year) || year < 1 || year > 9999) {

        std::cout << "Invalid year. Enter a number from 1 to 9999.\n";

        return;

    }

    double total = 0.0;

    int matchingCount = 0;

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "\nExpenses for ";

    if (month < 10) {

        std::cout << '0';

    }

    std::cout << month << '/' << year << '\n';

    for (const Expense& expense : expenses) {

        const std::string& date = expense.getDate();

        const int expenseYear = std::stoi(date.substr(0, 4));

        const int expenseMonth = std::stoi(date.substr(5, 2));

        if (expenseYear == year && expenseMonth == month) {

            if (matchingCount == 0) {

                std::cout

                    << std::left

                    << std::setw(6) << "ID"

                    << std::setw(13) << "Date"

                    << std::setw(20) << "Category"

                    << std::right << std::setw(14) << "Amount"

                    << "  "

                    << std::left << "Description"

                    << '\n';

                std::cout << std::string(85, '-') << '\n';

            }

            std::cout

                << std::left

                << std::setw(6) << expense.getId()

                << std::setw(13) << expense.getDate()

                << std::setw(20) << expense.getCategory()

                << std::right << std::setw(14) << expense.getAmount()

                << "  "

                << std::left << expense.getDescription()

                << '\n';

            total += expense.getAmount();

            ++matchingCount;

        }

    }

    if (matchingCount == 0) {

        std::cout << "No expenses found for the selected month and year.\n";

    }

    std::cout << "Total monthly expenditure: Rs. "

              << total << '\n';

    std::cout << "Number of matching expenses: "

              << matchingCount << '\n';

}

void ExpenseTracker::categoryWiseAnalysis() {
    std::cout << "\n===== Category-wise Spending =====\n";

    if (expenses.empty()) {
        std::cout << "No expenses recorded yet.\n";
        return;
    }

    // The map groups equal category names and accumulates their amounts.
    std::map<std::string, double> categoryTotals;

    for (const Expense& expense : expenses) {
        categoryTotals[expense.getCategory()] += expense.getAmount();
    }

    std::cout << std::fixed << std::setprecision(2);
    std::cout << std::left << std::setw(25) << "Category"
              << std::right << std::setw(16) << "Total Spending" << '\n';
    std::cout << std::string(41, '-') << '\n';

    double highestSpending = 0.0;
    const double tieTolerance = 1e-9;

    for (const auto& entry : categoryTotals) {
        std::cout << std::left << std::setw(25) << entry.first
                  << std::right << std::setw(16) << entry.second << '\n';

        if (entry.second > highestSpending) {
            highestSpending = entry.second;
        }
    }

    std::cout << "\nHighest Spending Category";

    // Print every category whose total equals the maximum.
    bool firstCategory = true;
    for (const auto& entry : categoryTotals) {
        if (std::fabs(entry.second - highestSpending) <= tieTolerance) {
            if (firstCategory) {
                std::cout << ": ";
                firstCategory = false;
            } else {
                std::cout << ", ";
            }
            std::cout << entry.first;
        }
    }

    std::cout << "\nHighest Spending Amount: Rs. "
              << highestSpending << '\n';
}

void ExpenseTracker::loadExpenses() {
    expenses.clear();
    nextId = 1;
    persistenceSafe = true;

    std::ifstream input(DATA_FILE, std::ios::in | std::ios::binary);
    if (!input.is_open()) {
        bool exists = false;
        if (!inspectFile(DATA_FILE, exists)) {
            persistenceSafe = false;
            std::cerr
                << "Error: cannot determine whether expenses.txt is safely "
                << "accessible. Saving is disabled.\n";
            return;
        }

        if (!exists) {
            bool backupExists = false;
            if (!inspectFile(BACKUP_FILE, backupExists)) {
                persistenceSafe = false;
                std::cerr
                    << "Error: expenses.txt is missing and the backup "
                    << "status cannot be checked safely. Saving is disabled.\n";
                return;
            }

            if (backupExists) {
                persistenceSafe = false;
                std::cerr
                    << "Recovery required: expenses.txt is missing, but "
                    << "expenses.txt.bak exists. Restore the backup manually; "
                    << "saving is disabled to protect the data.\n";
                return;
            }

            std::cout
                << "No existing expense file. Starting with an empty list.\n";
            return;
        }

        persistenceSafe = false;
        std::cerr
            << "Error: expenses.txt exists but cannot be opened for reading. "
            << "Saving is disabled to protect existing data.\n";
        return;
    }

    std::string line;
    if (!std::getline(input, line)) {
        persistenceSafe = false;
        if (input.bad() || (!input.eof() && input.fail())) {
            std::cerr
                << "Error while reading the file header in expenses.txt. "
                << "Saving is disabled.\n";
        } else {
            std::cerr
                << "Error: expenses.txt is empty or has a missing file header. "
                << "Saving is disabled to protect existing data.\n";
        }
        return;
    }

    if (line != FILE_HEADER) {
        persistenceSafe = false;
        std::cerr
            << "Error: invalid file header in expenses.txt. "
            << "Saving is disabled to protect existing data.\n";
        return;
    }

    std::set<int> loadedIds;
    int maxId = 0;
    std::size_t lineNumber = 1;

    while (std::getline(input, line)) {
        ++lineNumber;
        Expense expense(1, 1.0, "temporary", "2000-01-01", "temporary");

        if (!parseRecord(line, expense)) {
            persistenceSafe = false;
            std::cerr << "Malformed record at line " << lineNumber
                      << ". The record was skipped; the original file will "
                      << "not be overwritten.\n";
            continue;
        }

        if (!loadedIds.insert(expense.getId()).second) {
            persistenceSafe = false;
            std::cerr << "Duplicate expense ID at line " << lineNumber
                      << ". The duplicate was skipped; the original file "
                      << "will not be overwritten.\n";
            continue;
        }

        expenses.push_back(expense);
        if (expense.getId() > maxId) {
            maxId = expense.getId();
        }
    }

    if (input.bad() || (!input.eof() && input.fail())) {
        persistenceSafe = false;
        std::cerr
            << "Error while reading expenses.txt. Valid records read so far "
            << "remain available for viewing, but saving is disabled.\n";
        return;
    }

    if (maxId >= std::numeric_limits<int>::max() - 1) {
        persistenceSafe = false;
        std::cerr
            << "Expense IDs are outside the supported range. "
            << "Saving is disabled.\n";
        return;
    }

    nextId = maxId + 1;
    std::cout << "Loaded " << expenses.size()
              << " valid expense record(s).\n";

    if (!persistenceSafe) {
        std::cerr
            << "Warning: one or more records were invalid. Valid records "
            << "are available for viewing and analysis, but adding expenses "
            << "and saving are disabled until expenses.txt is repaired.\n";
    }
}

bool ExpenseTracker::saveExpenses() {
    if (!persistenceSafe) {
        std::cerr
            << "Save refused: the existing data file is not safe to replace.\n";
        return false;
    }

    bool backupExists = false;
    if (!inspectFile(BACKUP_FILE, backupExists)) {
        std::cerr
            << "Save refused: cannot safely inspect expenses.txt.bak.\n";
        return false;
    }
    if (backupExists) {
        std::cerr
            << "Save refused: expenses.txt.bak already exists. Inspect or "
            << "restore the backup before retrying.\n";
        return false;
    }

    // A stale temporary file is disposable; the canonical file is not.
    if (std::remove(TEMP_FILE) != 0 && errno != ENOENT) {
        std::cerr << "Error: cannot remove stale temporary expense file.\n";
        return false;
    }

    std::ofstream output(TEMP_FILE, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!output.is_open()) {
        std::cerr << "Error: cannot create temporary expense file.\n";
        return false;
    }

    output << FILE_HEADER << '\n';
    output << std::setprecision(std::numeric_limits<double>::max_digits10);

    for (const Expense& expense : expenses) {
        output << expense.getId() << '|'
               << expense.getAmount() << '|';
        writeLengthPrefixed(output, expense.getCategory());
        writeLengthPrefixed(output, expense.getDate());
        writeLengthPrefixed(output, expense.getDescription());
        output << '\n';

        if (!output) {
            break;
        }
    }

    output.flush();
    const bool flushSucceeded = static_cast<bool>(output);
    output.close();
    const bool closeSucceeded = !output.fail();

    if (!flushSucceeded || !closeSucceeded) {
        std::remove(TEMP_FILE);
        std::cerr
            << "Error: writing the temporary file failed. "
            << "The original data file was not replaced.\n";
        return false;
    }

    bool originalExists = false;
    if (!inspectFile(DATA_FILE, originalExists)) {
        std::remove(TEMP_FILE);
        std::cerr
            << "Error: cannot safely inspect expenses.txt. "
            << "The original file was not replaced.\n";
        return false;
    }

    if (originalExists && std::rename(DATA_FILE, BACKUP_FILE) != 0) {
        std::remove(TEMP_FILE);
        std::cerr
            << "Error: could not move the original file to its backup. "
            << "The original file was not replaced.\n";
        return false;
    }

    if (std::rename(TEMP_FILE, DATA_FILE) != 0) {
        std::cerr << "Error: could not install the new expense file.\n";

        if (originalExists) {
            if (std::rename(BACKUP_FILE, DATA_FILE) != 0) {
                persistenceSafe = false;
                std::cerr
                    << "CRITICAL: automatic restoration failed. Keep "
                    << "expenses.txt.bak and restore it manually. Further "
                    << "writes are disabled for this session.\n";
            } else {
                std::cerr << "The original data file was restored.\n";
            }
        }

        return false;
    }

    if (originalExists && std::remove(BACKUP_FILE) != 0) {
        persistenceSafe = false;
        std::cerr
            << "Warning: the new data file was saved, but the backup could "
            << "not be removed. Inspect expenses.txt.bak before continuing. "
            << "Further writes are disabled for this session.\n";
    }

    return true;
}
