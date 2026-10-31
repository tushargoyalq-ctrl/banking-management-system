#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib>
using namespace std;

const int MAXT = 50; // transactions remembered per account

// ---------- Safe number input (re-asks if the user types letters) ----------
int readInt(const char *msg) {
    int x;
    cout << msg;
    while (!(cin >> x)) {
        if (cin.eof())
            exit(0);
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid input! " << msg;
    }
    return x;
}

float readFloat(const char *msg) {
    float x;
    cout << msg;
    while (!(cin >> x)) {
        if (cin.eof())
            exit(0);
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid input! " << msg;
    }
    return x;
}

// ---------- One line of the passbook ----------
class Transaction {
private:
    string type;
    float amount, balanceAfter;

public:
    Transaction() {
        type = "";
        amount = balanceAfter = 0;
    }

    void set(string t, float a, float b) {
        type = t;
        amount = a;
        balanceAfter = b;
    }

    void show(int no) {
        cout << left << setw(5) << no << setw(12) << type << setw(12) << amount << balanceAfter << endl;
    }
};

// ---------- Account (abstract base class) ----------
class Account {
private:
    int accNo;
    int pin;
    Transaction history[MAXT];
    int tCount;
    static int nextNo;
    static int count;

protected:
    string name;
    float balance;

    void addTransaction(string type, float amount) {
        if (tCount == MAXT) { // history full: drop the oldest entry
            for (int i = 0; i < MAXT - 1; i++)
                history[i] = history[i + 1];
            tCount--;
        }
        history[tCount++].set(type, amount, balance);
    }

public:
    Account() {
        accNo = 0;
        pin = 0;
        balance = 0;
        tCount = 0;
        name = "";
        count++;
    }

    void input() {
        cout << "Enter Account Holder Name: ";
        cin >> name;
        do {
            pin = readInt("Set a 4-digit PIN: ");
            if (pin < 1000 || pin > 9999)
                cout << "PIN must be 4 digits!\n";
        } while (pin < 1000 || pin > 9999);
    }

    // gives the account its number and records the opening deposit
    void open(float opening) {
        accNo = nextNo++;
        if (opening > 0)
            deposit(opening);
    }

    void deposit(float amt) {
        if (amt <= 0) {
            cout << "Invalid amount!\n";
            return;
        }
        balance += amt;
        addTransaction("Deposit", amt);
        cout << "Deposited Rs " << amt << ". New balance: Rs " << balance << endl;
    }

    int getAccNo() { return accNo; }
    string getName() { return name; }
    float getBalance() { return balance; }
    bool checkPin(int p) { return p == pin; }
    void setName(string n) { name = n; }

    static void showCount() {
        cout << "Total account objects: " << count << endl;
    }

    virtual string type() = 0;                // pure virtual: makes Account abstract
    virtual bool withdraw(float amt) = 0;     // each account type has its own rule
    virtual float minOpening() { return 0; }

    virtual void display() {
        cout << left << setw(10) << accNo << setw(15) << name << type() << endl;
    }

    friend void printHistory(Account &a);

    virtual ~Account() { count--; }
};

int Account::nextNo = 1001;
int Account::count = 0;

// ---------- Savings: must always keep Rs 500 ----------
class SavingsAccount : public Account {
public:
    string type() override { return "Savings"; }

    float minOpening() override { return 500; }

    bool withdraw(float amt) override {
        if (amt <= 0) {
            cout << "Invalid amount!\n";
            return false;
        }
        if (balance - amt < 500) {
            cout << "Denied! A savings account must keep a minimum balance of Rs 500.\n";
            return false;
        }
        balance -= amt;
        addTransaction("Withdraw", amt);
        return true;
    }
};

// ---------- Current: may go Rs 5000 into overdraft ----------
class CurrentAccount : public Account {
public:
    string type() override { return "Current"; }

    bool withdraw(float amt) override {
        if (amt <= 0) {
            cout << "Invalid amount!\n";
            return false;
        }
        if (balance - amt < -5000) {
            cout << "Denied! Overdraft limit of Rs 5000 would be exceeded.\n";
            return false;
        }
        balance -= amt;
        addTransaction("Withdraw", amt);
        return true;
    }
};

// friend: can read the private transaction history
void printHistory(Account &a) {
    cout << "\n--- Transaction History: " << a.name << " (A/C " << a.accNo << ") ---\n";
    if (a.tCount == 0) {
        cout << "No transactions yet!\n";
        return;
    }
    cout << left << setw(5) << "No." << setw(12) << "Type" << setw(12) << "Amount" << "Balance\n";
    for (int i = 0; i < a.tCount; i++)
        a.history[i].show(i + 1);
    cout << "Current Balance: Rs " << a.balance << endl;
}

int findAccount(Account *accs[], int n, int no) {
    for (int i = 0; i < n; i++)
        if (accs[i]->getAccNo() == no)
            return i;
    return -1;
}

// asks account number and PIN; returns the index or -1
int login(Account *accs[], int n) {
    int no = readInt("Enter Account No: ");
    int i = findAccount(accs, n, no);
    if (i == -1) {
        cout << "Account not found!\n";
        return -1;
    }
    if (!accs[i]->checkPin(readInt("Enter PIN: "))) {
        cout << "Wrong PIN!\n";
        return -1;
    }
    return i;
}

int main() {
    const int MAX = 100;
    Account *accs[MAX];
    int n = 0, choice;

    cout << fixed << setprecision(2);

    do {
        cout << "\n===== BANKING MANAGEMENT SYSTEM =====\n";
        cout << "1. Create Account\n";
        cout << "2. Display All Accounts\n";
        cout << "3. Update Account Holder Name\n";
        cout << "4. Close Account\n";
        cout << "5. Deposit Money\n";
        cout << "6. Withdraw Money\n";
        cout << "7. Check Balance\n";
        cout << "8. Transaction History\n";
        cout << "9. Show Total Accounts\n";
        cout << "0. Exit\n";
        choice = readInt("Enter choice: ");

        switch (choice) {
            case 1: {
                if (n == MAX) {
                    cout << "Bank is full!\n";
                    break;
                }
                int t = readInt("Account type (1. Savings  2. Current): ");
                if (t != 1 && t != 2) {
                    cout << "Invalid account type!\n";
                    break;
                }

                Account *a;
                if (t == 1)
                    a = new SavingsAccount();
                else
                    a = new CurrentAccount();
                a->input();

                float opening = readFloat("Enter Opening Deposit: ");
                if (opening < a->minOpening()) {
                    cout << "Opening deposit too low! Minimum is Rs " << a->minOpening() << endl;
                    delete a;
                    break;
                }
                a->open(opening);
                accs[n++] = a;
                cout << "Account created! Account No: " << a->getAccNo() << endl;
                break;
            }

            case 2:
                if (n == 0)
                    cout << "No accounts found!\n";
                else
                    cout << left << setw(10) << "Acc No" << setw(15) << "Name" << "Type\n";
                for (int i = 0; i < n; i++)
                    accs[i]->display();
                break;

            case 3: {
                int i = login(accs, n);
                if (i == -1)
                    break;
                string nm;
                cout << "Enter new name: ";
                cin >> nm;
                accs[i]->setName(nm);
                cout << "Name updated!\n";
                break;
            }

            case 4: {
                int i = login(accs, n);
                if (i == -1)
                    break;
                if (accs[i]->getBalance() < 0) {
                    cout << "Overdraft pending! Deposit money to clear it before closing.\n";
                    break;
                }
                cout << "Account closed. Rs " << accs[i]->getBalance() << " handed over to " << accs[i]->getName() << ".\n";
                delete accs[i];
                for (int j = i; j < n - 1; j++)
                    accs[j] = accs[j + 1];
                n--;
                break;
            }

            case 5: {
                int i = findAccount(accs, n, readInt("Enter Account No: "));
                if (i == -1)
                    cout << "Account not found!\n";
                else
                    accs[i]->deposit(readFloat("Enter amount to deposit: "));
                break;
            }

            case 6: {
                int i = login(accs, n);
                if (i == -1)
                    break;
                float amt = readFloat("Enter amount to withdraw: ");
                if (accs[i]->withdraw(amt))
                    cout << "Please collect Rs " << amt << ". New balance: Rs " << accs[i]->getBalance() << endl;
                break;
            }

            case 7: {
                int i = login(accs, n);
                if (i == -1)
                    break;
                cout << "\nAccount Holder: " << accs[i]->getName() << endl;
                cout << "Account Type  : " << accs[i]->type() << endl;
                cout << "Balance       : Rs " << accs[i]->getBalance() << endl;
                break;
            }

            case 8: {
                int i = login(accs, n);
                if (i != -1)
                    printHistory(*accs[i]);
                break;
            }

            case 9:
                Account::showCount();
                break;

            case 0:
                cout << "Exiting program...\n";
                break;

            default:
                cout << "Invalid choice!\n";
        }

    } while (choice != 0);

    for (int i = 0; i < n; i++)
        delete accs[i];

    return 0;
}
