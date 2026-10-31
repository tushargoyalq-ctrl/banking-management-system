# Indian Banking Management System

| OOPS Lab Project - C++ |
| :--- |

A menu-driven console application in C++ that manages bank accounts, deposits, withdrawals, and transaction history using Object-Oriented Programming (OOP) principles.

For a beginner-friendly walkthrough of the code, see [oops_banking_explaine.md](oops_banking_explaine.md).

## Modules

| # | Module | What it does | Menu options |
| --- | --- | --- | --- |
| 1 | Create Account | Open new Savings, Current, or Salary accounts | 1, 2, 3, 4 |
| 2 | Display Accounts | List all active accounts in the system | 5 |
| 3 | Update Account | Modify account holder details | 6 |
| 4 | Close Account | Remove an account from the system | 7 |
| 5 | Deposit Money | Add funds to a specific account | 8 |
| 6 | Withdraw Money | Deduct funds following account-specific rules | 9 |
| 7 | Check Balance | View current balance and account details | 10, 11 |
| 8 | Transaction History | View past transaction logs (passbook style) | 12 |

## OOP Concepts Used

- **Classes & Objects**: Encapsulating account data and behavior into structured classes (`Account`, `SavingsAccount`, `CurrentAccount`, `SalaryAccount`).
- **Inheritance**: Reusing base account features across specialized account types.
- **Polymorphism**: Overriding withdrawal and rule-checking functions dynamically.
- **Encapsulation**: Protecting sensitive balance manipulations through secure methods.
