# Banking Management System — Explained Simply

**Subject:** OOPS Lab | **Language:** C++ | **File:** `BankingSystem.cpp`

This document explains what the project does and how the code works, in plain language. You do not need to read the code first.

---

## 1. What is this project?

A small program that works like a bank counter. Using a text menu you can:

- **open** a bank account (Savings or Current) and **close** it
- **deposit** money
- **withdraw** money (the rules depend on the account type)
- check the **balance** and see the **transaction history** (like a passbook)

Everything is stored in memory while the program runs. When you close it, the data is gone (no file or database).

### The four modules

| # | Module | What it does | Menu option |
|---|---|---|---|
| 1 | Create and manage accounts | Open, list, rename, close accounts | 1, 2, 3, 4 |
| 2 | Deposit money | Add money to an account | 5 |
| 3 | Withdraw money | Take money out, following the account's rule | 6 |
| 4 | Balance and history | See balance, see all past transactions | 7, 8 |

---

## 2. How to run it

```bash
g++ BankingSystem.cpp -o bank
./bank
```

---

## 3. The story of one customer

1. **Open an account.** Menu 1 → Savings → name `Rahul`, PIN `1234`, opening deposit Rs 5000. The bank gives **Account No. 1001**.
2. **Deposit.** Menu 5 → account 1001 → Rs 2000. Balance is now Rs 7000.
3. **Withdraw.** Menu 6 → account 1001 → PIN 1234 → Rs 6500. Allowed. Balance is now Rs 500.
4. **Try to withdraw more.** Rs 1 more is **denied**, because a Savings account must always keep Rs 500.
5. **History.** Menu 8 → shows all three transactions with the balance after each.

---

## 4. The account types and their rules

| | Savings | Current |
|---|---|---|
| Minimum opening deposit | Rs 500 | Rs 0 |
| Withdraw rule | Balance can never go below Rs 500 | Balance can go down to **-Rs 5000** (this is called an *overdraft*: you owe the bank) |

Other rules:
- Account numbers are automatic: 1001, 1002, 1003...
- A **4-digit PIN** protects withdraw, balance, history, rename and close. Anyone can deposit.
- An account with a negative balance cannot be closed until the debt is cleared.

---

## 5. The code: building blocks

```
Transaction                 (one line of the passbook)

Account   ← abstract class (the common blueprint)
 ├── SavingsAccount         (own withdraw rule)
 └── CurrentAccount         (own withdraw rule)

printHistory()              (friend function, prints the passbook)
```

### 5.1 `Transaction` — one passbook line

Remembers three things: the **type** ("Deposit" or "Withdraw"), the **amount**, and the **balance after** it. It has `set()` to fill the values and `show()` to print one line.

### 5.2 `Account` — the common blueprint

This is the main class. It holds everything every bank account needs.

| Variable | Access | Meaning |
|---|---|---|
| `accNo` | private | Account number |
| `pin` | private | 4-digit PIN |
| `history[50]`, `tCount` | private | The passbook: up to 50 `Transaction` objects, and how many are filled |
| `name` | protected | Account holder's name |
| `balance` | protected | Current money in the account |
| `nextNo` (static) | private | The next account number to hand out (starts at 1001) |
| `count` (static) | private | How many Account objects exist |

**Private vs protected:** `private` means only the `Account` class itself can touch it. `protected` means `Account` **and the classes built on it** (Savings, Current) can touch it. Outside code (like `main`) can touch neither; it must use functions. This is **encapsulation**.

Important functions:

| Function | What it does |
|---|---|
| `input()` | Asks for the holder's name and a 4-digit PIN (keeps asking until the PIN is valid) |
| `open(opening)` | Gives the account its number (`accNo = nextNo++`) and records the opening deposit |
| `deposit(amt)` | Rejects zero or negative amounts. Otherwise adds to balance and records a "Deposit" transaction |
| `addTransaction()` | Saves a passbook line. If the 50 slots are full, it throws away the oldest one and shifts the rest up |
| `checkPin(p)` | Returns true if the PIN is correct |
| `getBalance()`, `getName()`, `getAccNo()`, `setName()` | Getters and a setter, the allowed way to read or change private/protected data |
| `showCount()` (static) | Prints how many accounts exist |

**Abstract class.** Three functions are marked specially:

```cpp
virtual string type() = 0;
virtual bool withdraw(float amt) = 0;
virtual float minOpening() { return 0; }
```

- `= 0` means **pure virtual**: "every real account type must write its own version of this". A class with a pure virtual function is **abstract**, which means you cannot write `new Account()`. You can only create a Savings or a Current account. This makes sense: there is no such thing as a "generic" bank account.
- `minOpening()` is virtual with a default of 0. Savings changes it to 500.

The account number is given in `open()`, not in the constructor. If someone fails the opening-deposit check and the account is cancelled, no number is wasted.

### 5.3 `SavingsAccount` and `CurrentAccount`

Both are built on top of `Account` (`class SavingsAccount : public Account`). They **override** the virtual functions:

| Function | Savings | Current |
|---|---|---|
| `type()` | "Savings" | "Current" |
| `minOpening()` | 500 | 0 (inherited default) |
| `withdraw()` | Deny if `balance - amt < 500` | Deny if `balance - amt < -5000` |

If the rule allows it, `withdraw()` reduces the balance and records a "Withdraw" transaction.

This is the heart of the project: **the same call `withdraw(amount)` follows different rules depending on the account type.**

### 5.4 `printHistory()` — a friend function

The passbook (`history`) is private, so no outside function can read it. `printHistory()` is declared a **friend** of `Account`, which gives it special permission. It prints the holder's name, then every transaction, then the current balance.

### 5.5 Helper functions

| Function | What it does |
|---|---|
| `readInt()`, `readFloat()` | Read a number safely. If the user types letters, they print "Invalid input!" and ask again, so the program never gets stuck |
| `findAccount(accs, n, no)` | Loops through the accounts and returns the position of the account number, or -1 |
| `login(accs, n)` | Asks for account number and PIN. Returns the account's position if both are right, or -1 with a message ("Account not found!" / "Wrong PIN!") |

---

## 6. The `main()` function

### Storage

```cpp
Account* accs[100];     // array of POINTERS to accounts
```

Why `Account*` (pointers)? One array has to hold **both** Savings and Current accounts. A pointer to the base class (`Account*`) can point to either, and when we call `withdraw()` through it, C++ automatically runs the right version. This is **polymorphism**.

### The menu loop

A `do { ... } while (choice != 0);` loop shows the menu again and again until the user enters 0. A `switch` runs the code for each option.

### What each option does

| Option | What happens |
|---|---|
| 1 Create | Asks type (1 or 2). Creates `new SavingsAccount()` or `new CurrentAccount()`. Calls `input()`. Asks the opening deposit; if it is below `minOpening()`, the account is deleted and cancelled. Otherwise `open()` and store the pointer in `accs[]` |
| 2 Display All | Prints account number, name and type of every account (not balances, for privacy) |
| 3 Update Name | `login()` first, then `setName()` |
| 4 Close | `login()`. Refused if balance is negative. Otherwise prints the money handed back, `delete`s the account and shifts the later ones left |
| 5 Deposit | Only needs the account number (no PIN), then `deposit()` |
| 6 Withdraw | `login()`, ask the amount, call `withdraw()`. Prints the result |
| 7 Balance | `login()`, then prints name, type and balance |
| 8 History | `login()`, then `printHistory()` |
| 9 Total | `Account::showCount()` |

When the user exits, a final loop `delete`s every account so no memory is leaked.

---

## 7. OOPS concepts used, in simple words

| Concept | Simple meaning | Where in this project |
|---|---|---|
| **Class and object** | Blueprint and the real thing made from it | `Account` is a blueprint; account 1001 is an object |
| **Abstraction** | Hide details, and declare "what" without saying "how" | `Account` is abstract: `withdraw()` is declared, but each type decides how |
| **Encapsulation** | Keep data hidden; allow access only through functions | PIN, balance and history are private/protected, used through functions |
| **Inheritance** | A new class reuses an existing class | `SavingsAccount` and `CurrentAccount` inherit from `Account` |
| **Polymorphism** | Same call, different behaviour depending on the object | `accs[i]->withdraw(amt)` runs Savings or Current rules |
| **Function overriding** | A child class replaces the parent's version of a function | `override` keyword in the two child classes |
| **Pure virtual function** | A function with no body in the parent that children must provide | `virtual bool withdraw(float) = 0;` |
| **Constructor / Destructor** | Run automatically when an object is created / destroyed | `Account()` sets defaults; `~Account()` reduces `count` |
| **Virtual destructor** | Makes sure the correct destructor runs when deleting through a base pointer | `virtual ~Account()` |
| **Static member** | One value shared by all objects | `nextNo` (account numbers) and `count` |
| **Friend function** | An outside function allowed to read private data | `printHistory()` |
| **Composition** | An object that contains other objects | Each `Account` contains 50 `Transaction` objects |
| **Dynamic memory** | Create objects while the program runs with `new`, free with `delete` | Accounts are created and deleted at runtime |

---

## 8. Rules built into the program (validation)

- Typing letters where a number is expected is rejected and re-asked.
- PIN must be exactly 4 digits.
- Opening deposit must be at least Rs 500 for Savings.
- Deposit and withdraw amounts must be positive.
- Wrong PIN or unknown account number is rejected.
- Savings cannot drop below Rs 500; Current cannot go beyond -Rs 5000.
- An account in overdraft cannot be closed.

---

## 9. Limitations (be honest about these in the viva)

- No saving: all data is lost when the program closes.
- Maximum 100 accounts; each remembers only its latest 50 transactions.
- Names are one word only (it uses `cin >>`).
- The PIN is stored as plain text and there is no lockout after wrong tries. Real banks never do this.
- No transfers between accounts and no interest.

**Easy improvements to mention:** transfer between accounts, interest for savings, saving to a file, locking after 3 wrong PINs.

---

## 10. Likely viva questions

**Q: Where did you use inheritance?**
`SavingsAccount` and `CurrentAccount` inherit from `Account`.

**Q: Where is polymorphism?**
`withdraw()` is virtual. We call it through an `Account*` pointer and C++ runs the Savings or Current version based on the real object.

**Q: What is an abstract class? Which one is it here?**
A class with at least one pure virtual function; it cannot be instantiated. `Account` is abstract because `withdraw()` and `type()` are `= 0`.

**Q: Why store accounts as `Account*`?**
So one array can hold both Savings and Current accounts.

**Q: What is the difference between private and protected?**
Private: only the same class. Protected: the same class and its child classes. We made `balance` protected so Savings and Current can change it when withdrawing, while outside code cannot.

**Q: What does `static` do for `nextNo`?**
There is only one `nextNo`, shared by all accounts. Every new account takes the next number, so numbers never repeat.

**Q: Why is the destructor virtual?**
When we `delete` a `SavingsAccount` through an `Account*`, a virtual destructor makes sure the correct destructor runs.

**Q: What is a friend function and why use it here?**
A function outside the class with permission to read its private data. `printHistory()` needs the private `history` array.

**Q: What is an overdraft?**
Taking out more money than you have. A Current account here may go down to -Rs 5000.

**Q: What happens when the history reaches 50 transactions?**
The oldest entry is dropped and the rest shift up, so the passbook always shows the latest 50.
