# C++ Expense Tracker

A command-line expense tracker written in C++17. It lets users record expenses, review saved records, calculate monthly expenditure, and compare spending by category. Expense records are stored locally in `expenses.txt` and loaded when the program starts.

## Features

- Add expenses with an amount, category, date, and description.
- Validate positive amounts, non-empty text fields, and calendar dates in `YYYY-MM-DD` format, including leap years.
- View all saved expenses.
- Calculate the total and count of expenses for a selected month and year.
- Calculate category totals and report every category tied for the highest spending.
- Persist records between runs using a versioned text format.
- Detect malformed records and duplicate IDs; when the data file cannot be trusted, disable saving rather than overwrite it.
- Preserve `|` and `:` characters in category and description text.

## Technology stack

- C++17
- Standard Library: `vector`, `map`, `string`, file streams, input/output streams, formatting, and numeric utilities
- GCC/MinGW
- PowerShell on Windows

No third-party libraries are required.

## C++ concepts demonstrated

- Classes, encapsulation, constructors, and `const` getters
- Dynamic collections with `std::vector`
- Key-value aggregation with `std::map`
- Iteration, conditionals, and functions
- Input validation and string parsing
- File I/O, serialization, and deserialization
- Basic error handling and data-integrity checks
- Separate declarations and implementations using `.h` and `.cpp` files

## Project structure

```text
ExpenseTracker/
├── main.cpp
├── Expense.h
├── Expense.cpp
├── ExpenseTracker.h
├── ExpenseTracker.cpp
├── README.md
├── .gitignore
└── expenses.txt       # Local runtime data; do not commit personal records
```

The executable is generated when you compile the project; it does not need to be committed.

## Architecture

- **`Expense`** (`Expense.h`, `Expense.cpp`): represents one expense with an ID, amount, category, date, and description. Its fields are private and exposed through `const` getters.
- **`ExpenseTracker`** (`ExpenseTracker.h`, `ExpenseTracker.cpp`): owns the collection, validates user input, implements the menu operations, calculates summaries, and loads/saves the data file.
- **`main.cpp`**: loads stored records, displays the menu, validates menu choices, and dispatches to the relevant `ExpenseTracker` operation.

### How records are stored in memory

The tracker keeps expenses in `std::vector<Expense>`. Each element is one `Expense` object. The vector supports adding records and iterating through them for display and calculations. IDs are assigned by the tracker and recovered from the data file on startup.

### How category totals are calculated

Category analysis builds a `std::map<std::string, double>`. For each expense, its amount is added to the entry keyed by that expense's category:

```cpp
categoryTotals[expense.getCategory()] += expense.getAmount();
```

Expenses with exactly matching category strings contribute to the same total. The program then finds the maximum total and prints all categories within its configured floating-point tie tolerance of that maximum.

## Data persistence and file format

The application reads and writes `expenses.txt` in the **current working directory**. If the file does not exist and no recovery condition is detected, the application starts with an empty list. A successful save writes a temporary file and uses a backup/rename sequence to reduce the risk of replacing the original with incomplete data. If malformed data or a duplicate ID is detected, valid records remain available for viewing, but further saving is disabled until the file is repaired.

The file starts with this version header:

```text
EXPENSE_TRACKER_V1
```

Each subsequent line stores one record in this form:

```text
ID|amount|<category-length>:<category><date-length>:<date><description-length>:<description>
```

The lengths are character counts as represented by `std::string::size()`. Length-prefixed text fields allow category and description values to contain separators such as `|` and `:` without confusing the record parser. Keep the header and record structure intact when editing the file manually.

**Privacy:** `expenses.txt` may contain personal financial information. Do not publish your real data. For a public repository, either omit the data file (recommended) or create a clearly synthetic sample file that contains no personal information. The application can create `expenses.txt` on first run.

## Build and run on Windows (PowerShell + GCC/MinGW)

Open PowerShell in the project directory—the folder containing `main.cpp`—and check that GCC is available:

```powershell
g++ --version
```

Compile using C++17 and strict warnings:

```powershell
g++ -std=c++17 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wformat=2 main.cpp Expense.cpp ExpenseTracker.cpp -o ExpenseTracker.exe
```

Run the application from the same directory so that `expenses.txt` is read/written there:

```powershell
.\ExpenseTracker.exe
```

If your compiler is not found, install/configure GCC/MinGW and ensure its `bin` directory is on `PATH`.

## Example menu

```text
===== Expense Tracker =====
1. Add Expense
2. View All Expenses
3. Monthly Expense Summary
4. Category-wise Analysis
5. Exit
Enter your choice:
```

### Example category analysis

Output depends on the records in your local `expenses.txt`. For example, if the highest totals are tied:

```text
===== Category-wise Spending =====
Category                     Total Spending
-----------------------------------------
Food and Dining                    250.50
Travel                             250.50

Highest Spending Category: Food and Dining, Travel
Highest Spending Amount: Rs. 250.50
```

This is illustrative output, not a bundled-data guarantee.

## Manual testing

After building, run through these checks manually:

1. **First launch:** with no `expenses.txt` in the working directory, start the program and choose **View All Expenses**. It should report that no expenses are recorded.
2. **Add and persist:** add a valid expense, exit, restart, and confirm it appears in **View All Expenses**.
3. **Input validation:** try zero, a negative amount, non-numeric input, an empty category/description, and an invalid date such as `2025-02-29`.
4. **Leap years:** verify `2000-02-29` is accepted and `1900-02-29` is rejected.
5. **Monthly summary:** enter a month/year with matching records and another with no matching records; verify the total and count.
6. **Category analysis:** add records in the same category and confirm totals are combined; create a tie for the highest total and verify all tied categories are printed.
7. **Reserved characters:** use `|` and `:` in a category or description, save, restart, and confirm the text is preserved.
8. **Data-file integrity:** only use a disposable copy to test malformed lines, duplicate IDs, or simulated file errors. Confirm that unsafe data is not overwritten.

These are manual test instructions; this README does not claim that an automated test suite is included.

## Current limitations

- Console-only interface; no GUI or web interface.
- Data is stored in a local text file, not a database or cloud service.
- No editing, deleting, searching, exporting, or budgeting features.
- Category matching is based on exact text; similar names with different capitalization or spacing are separate categories.
- Monetary values use `double`, not fixed-point decimal arithmetic, so this is a learning project rather than accounting software.
- File-recovery behavior can depend on operating-system file semantics; keep independent backups of important records.

## Possible future improvements

- Add automated unit and integration tests.
- Add edit/delete/search and date-range reports.
- Use integer minor currency units or a decimal representation for financial arithmetic.
- Add configurable currency and category normalization.
- Add CSV export and database storage.
- Add a build system such as CMake and a CI workflow.
