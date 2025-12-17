#include <iostream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <fstream>
using namespace std;

// ================== CONFIG ==================
const string RS = "Rs.";

string formatMoney(double amount) {
    ostringstream ss;
    ss << RS << fixed << setprecision(2) << amount;
    return ss.str();
}

// ================== ITEM BASE CLASS ==================
class Item {
protected:
    int id;
    string name;
    double price;
    int quantity;
    double discountPercent;

public:
    Item(int id=0, string name="", double price=0, int qty=0, double disc=0)
        : id(id), name(name), price(price), quantity(qty), discountPercent(disc) {}

    virtual string getCategory() const = 0;

    int getId() const { return id; }
    string getName() const { return name; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }
    double getDiscount() const { return discountPercent; }

    double getFinalPrice() const {
        return price - (price * discountPercent / 100.0);
    }

    void reduceQuantity(int q) { quantity -= q; }

    void edit(string newName, double newPrice, int newQty) {
        name = newName;
        price = newPrice;
        quantity = newQty;
    }

    void display() const {
        cout << setw(5) << id
             << setw(20) << name
             << setw(20) << getCategory()
             << setw(12) << formatMoney(price)
             << setw(10) << discountPercent << "%"
             << setw(12) << formatMoney(getFinalPrice())
             << setw(10) << quantity << endl;
    }
};

// ================== ITEM CATEGORIES ==================
class Dairy : public Item {
public:
    Dairy(int id, string name, double price, int qty)
        : Item(id, name, price, qty, 10) {}
    string getCategory() const override { return "Dairy"; }
};

class FrozenVeggies : public Item {
public:
    FrozenVeggies(int id, string name, double price, int qty)
        : Item(id, name, price, qty, 15) {}
    string getCategory() const override { return "Frozen Veggies"; }
};

class Fruits : public Item {
public:
    Fruits(int id, string name, double price, int qty)
        : Item(id, name, price, qty, 5) {}
    string getCategory() const override { return "Fruits"; }
};

class CookingMaterial : public Item {
public:
    CookingMaterial(int id, string name, double price, int qty)
        : Item(id, name, price, qty, 20) {}
    string getCategory() const override { return "Cooking Material"; }
};

// ================== GROCERY STORE ==================
class GroceryStore {
private:
    vector<Item*> items;
   const string FILENAME = "inventory.csv";

public:
    void loadItems() {
    ifstream fin(FILENAME);
    if (!fin.is_open()) return;

    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string idStr, name, category, priceStr, qtyStr, discStr;

        getline(ss, idStr, ',');
        getline(ss, name, ',');
        getline(ss, category, ',');
        getline(ss, priceStr, ',');
        getline(ss, qtyStr, ',');
        getline(ss, discStr, ',');

        // Skip lines that are missing required data
        if (idStr.empty() || name.empty() || category.empty() ||
            priceStr.empty() || qtyStr.empty()) {
            continue;
        }

        try {
            int id = stoi(idStr);
            double price = stod(priceStr);
            int qty = stoi(qtyStr);

            if (category == "Dairy")
                items.push_back(new Dairy(id, name, price, qty));
            else if (category == "Frozen Veggies")
                items.push_back(new FrozenVeggies(id, name, price, qty));
            else if (category == "Fruits")
                items.push_back(new Fruits(id, name, price, qty));
            else if (category == "Cooking Material")
                items.push_back(new CookingMaterial(id, name, price, qty));
        }
        catch (exception &e) {
            // Skip bad lines silently
            continue;
        }
    }
    fin.close();
}

    void saveItems() {
        ofstream fout(FILENAME);
        for (auto i : items) {
            fout << i->getId() << ","
                 << i->getName() << ","
                 << i->getCategory() << ","
                 << i->getPrice() << ","
                 << i->getQuantity() << ","
                 << i->getDiscount() << "\n";
        }
        fout.close();
    }

    void addItem() {
        int choice, id, qty;
        string name;
        double price;

        cout << "\nSelect Category:\n";
        cout << "1. Dairy\n2. Frozen Veggies\n3. Fruits\n4. Cooking Material\nEnter choice: ";
        cin >> choice;

        cout << "Enter Item ID: ";
        cin >> id;
        cin.ignore();
        cout << "Enter Name: ";
        getline(cin, name);
        cout << "Enter Price: ";
        cin >> price;
        cout << "Enter Stock Quantity: ";
        cin >> qty;

        switch (choice) {
            case 1: items.push_back(new Dairy(id, name, price, qty)); break;
            case 2: items.push_back(new FrozenVeggies(id, name, price, qty)); break;
            case 3: items.push_back(new Fruits(id, name, price, qty)); break;
            case 4: items.push_back(new CookingMaterial(id, name, price, qty)); break;
            default: cout << "Invalid category!\n"; return;
        }

        cout << "Item added!\n";
        saveItems();
    }

    void showItems() {
        if (items.empty()) { 
            cout << "No items in store.\n"; 
            return; 
        }

        cout << "\n================ ITEMS IN STORE ================\n";
        cout << setw(5) << "ID"
             << setw(20) << "NAME"
             << setw(20) << "CATEGORY"
             << setw(12) << "PRICE"
             << setw(10) << "DISC"
             << setw(12) << "FINAL"
             << setw(10) << "QTY" << endl;

        for (auto *i : items) i->display();
    }

    int findItem(int id) {
        for (int i = 0; i < (int)items.size(); i++)
            if (items[i]->getId() == id) return i;
        return -1;
    }

    void editItem() {
        int id, qty;
        double price;
        string name;

        cout << "Enter Item ID to Edit: ";
        cin >> id;

        int idx = findItem(id);
        if (idx == -1) { cout << "Item not found!\n"; return; }

        cin.ignore();
        cout << "New Name: "; getline(cin, name);
        cout << "New Price: "; cin >> price;
        cout << "New Quantity: "; cin >> qty;

        items[idx]->edit(name, price, qty);
        cout << "Item updated!\n";
        saveItems();
    }

    void deleteItem() {
        int id;
        cout << "Enter Item ID to delete: ";
        cin >> id;

        int idx = findItem(id);
        if (idx == -1) { cout << "Item not found!\n"; return; }

        delete items[idx];
        items.erase(items.begin() + idx);

        cout << "Item deleted.\n";
        saveItems();
    }

    void billing() {
        double total = 0;
        char more;
        vector<pair<Item*, int>> purchased;

        cout << "\n===== START BILLING =====\n";

        do {
            int id, qty;
            cout << "Enter Item ID: ";
            cin >> id;

            int idx = findItem(id);
            if (idx == -1) { 
                cout << "Item does NOT exist!\n"; 
                cout << "Add another item? (y/n): "; 
                cin >> more; 
                continue; 
            }

            cout << "Enter Quantity: ";
            cin >> qty;

            if (qty > items[idx]->getQuantity()) {
                cout << "Only " << items[idx]->getQuantity() << " in stock!\n";
                cout << "Add another item? (y/n): "; 
                cin >> more; 
                continue;
            }

            double cost = qty * items[idx]->getFinalPrice();
            total += cost;
            items[idx]->reduceQuantity(qty);
            purchased.push_back({items[idx], qty});

            cout << "Added " << qty << " x " << items[idx]->getName()
                 << " (" << formatMoney(items[idx]->getFinalPrice()) << " each)\n";

            cout << "Add another item? (y/n): "; 
            cin >> more;
        } while (more == 'y' || more == 'Y');

        int mem; 
        double memberDisc = 0;
        cout << "\nMembership Options:\n1. None (0%)\n2. Silver (5%)\n3. Gold (10%)\n4. Platinum (15%)\nChoose: ";
        cin >> mem;

        if (mem == 2) memberDisc = 5; 
        else if (mem == 3) memberDisc = 10; 
        else if (mem == 4) memberDisc = 15;

        double memberAmount = total * memberDisc / 100.0;
        double afterMember = total - memberAmount;
        double cgst = afterMember * 0.05;
        double sgst = afterMember * 0.05;
        double finalBill = afterMember + cgst + sgst;

        cout << "\n=========== FINAL INVOICE ===========\n";
        cout << left << setw(30) << "Item" << setw(10) << "Qty" << setw(12) << "Price" << setw(12) << "Total\n";

        for (auto p : purchased) {
            cout << left << setw(30) << p.first->getName()
                 << setw(10) << p.second
                 << setw(12) << formatMoney(p.first->getFinalPrice())
                 << setw(12) << formatMoney(p.first->getFinalPrice()*p.second) << endl;
        }

        cout << left << setw(30) << "Subtotal:" 
             << right << setw(12) << formatMoney(total) << endl;

        cout << left << setw(30) << ("Membership Discount (" + to_string((int)memberDisc) + "%):")
             << right << setw(12) << "-" + formatMoney(memberAmount).substr(RS.size()) << endl;

        cout << left << setw(30) << "CGST (5%):" 
             << right << setw(12) << formatMoney(cgst) << endl;

        cout << left << setw(30) << "SGST (5%):" 
             << right << setw(12) << formatMoney(sgst) << endl;

        cout << left << setw(30) << "TOTAL PAYABLE:" 
             << right << setw(12) << formatMoney(finalBill) << endl;

        cout << "======================================\n";

        saveItems();
    }

    ~GroceryStore() {
        for (auto p : items) delete p;
    }
};

// ================== MAIN ==================
int main() {
    GroceryStore store;
    store.loadItems();

    int choice;
    do {
        cout << "\n====== GROCERY MANAGEMENT SYSTEM ======\n";
        cout << "1. Add Item\n2. View Items\n3. Edit Item\n4. Delete Item\n5. Billing\n6. Exit\nChoose: ";
        cin >> choice;

        switch (choice) {
            case 1: store.addItem(); break;
            case 2: store.showItems(); break;
            case 3: store.editItem(); break;
            case 4: store.deleteItem(); break;
            case 5: store.billing(); break;
            case 6: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice!\n";
        }
    } while (choice != 6);

    return 0;
}
