#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>

using namespace std;

class Item {
protected:
    string itemName;
    int itemID;
    int stockQuantity;
    double itemPrice;

public:
    Item(string name, int id, int quantity, double price)
        : itemName(name), itemID(id), stockQuantity(quantity), itemPrice(price) {}

    virtual void display() const {
        cout << "Item ID: " << itemID << "\nName: " << itemName 
             << "\nStock Quantity: " << stockQuantity << "\nPrice: $" << itemPrice << endl;
    }

    virtual void updateStock(int quantity) {
        stockQuantity += quantity;
    }

    string getName() const {
        return itemName;
    }

    int getStockQuantity() const {
        return stockQuantity;
    }

    int getItemID() const {
        return itemID;
    }

    double getPrice() const {
        return itemPrice;
    }

    virtual ~Item() {}
};

class PerishableItem : public Item {
private:
    string expirationDate;

public:
    PerishableItem(string name, int id, int quantity, double price, string expiration)
        : Item(name, id, quantity, price), expirationDate(expiration) {}

    void display() const override {
        Item::display();
        cout << "Expiration Date: " << expirationDate << endl;
    }

    void updateStock(int quantity) override {
        if (stockQuantity + quantity < 0) {
            cout << "Cannot remove more stock than available." << endl;
        } else {
            stockQuantity += quantity;
        }
    }
};

class NonPerishableItem : public Item {
public:
    NonPerishableItem(string name, int id, int quantity, double price)
        : Item(name, id, quantity, price) {}

    void display() const override {
        Item::display();
    }

    void updateStock(int quantity) override {
        if (stockQuantity + quantity < 0) {
            cout << "Cannot remove more stock than available." << endl;
        } else {
            stockQuantity += quantity;
        }
    }
};

class Inventory {
private:
    vector<Item*> items;

public:
    ~Inventory() {
        for (auto item : items) {
            delete item;
        }
    }

    void addItem(Item* item) {
        items.push_back(item);
    }

    void removeItem(int itemID) {
        for (auto it = items.begin(); it != items.end(); ++it) {
            if ((*it)->getItemID() == itemID) {
                delete *it;
                items.erase(it);
                cout << "Item removed from inventory." << endl;
                return;
            }
        }
        cout << "Item with ID " << itemID << " not found." << endl;
    }

    void updateItemStock(int itemID, int quantity) {
        for (auto& item : items) {
            if (item->getItemID() == itemID) {
                item->updateStock(quantity);
                cout << "Stock updated for " << item->getName() << "." << endl;
                return;
            }
        }
        cout << "Item with ID " << itemID << " not found." << endl;
    }

    void checkLowStock(int threshold) const {
        cout << "\nItems with low stock (less than " << threshold << "):" << endl;
        for (const auto& item : items) {
            if (item->getStockQuantity() < threshold) {
                cout << item->getName() << " (ID: " << item->getItemID() 
                     << ") - Stock: " << item->getStockQuantity() << endl;
            }
        }
    }

    void generateReport() const {
        cout << "\nInventory Report:" << endl;
        for (const auto& item : items) {
            item->display();
            cout << "-------------------------" << endl;
        }
    }

    void saveInventoryToFile() const {
        ofstream outFile("inventory_report.txt");
        if (outFile.is_open()) {
            outFile << "Inventory Report\n";
            outFile << "------------------\n";
            for (const auto& item : items) {
                outFile << "Item ID: " << item->getItemID() << "\n";
                outFile << "Name: " << item->getName() << "\n";
                outFile << "Stock Quantity: " << item->getStockQuantity() << "\n";
                outFile << "Price: $" << item->getPrice() << "\n";
                outFile << "------------------------\n";
            }
            outFile.close();
            cout << "Inventory saved to file." << endl;
        } else {
            cout << "Error: Could not open file for saving." << endl;
        }
    }

    void loadInventoryFromFile() {
        ifstream inFile("inventory_report.txt");
        string name, expiration;
        int id, quantity;
        double price;

        if (inFile.is_open()) {
            while (inFile >> id >> name >> quantity >> price >> expiration) {
                // assuming format: ID Name Quantity Price Expiration
                if (expiration.empty()) {
                    addItem(new NonPerishableItem(name, id, quantity, price));
                } else {
                    addItem(new PerishableItem(name, id, quantity, price, expiration));
                }
            }
            inFile.close();
            cout << "Inventory loaded from file." << endl;
        } else {
            cout << "Error: Could not open file for reading." << endl;
        }
    }
};

int main() {
    Inventory inv;
    int choice;

    inv.loadInventoryFromFile(); // Load previous inventory from file

    do {
        cout << "\n===== Inventory Tracker =====\n";
        cout << "1. Add Item\n";
        cout << "2. Remove Item\n";
        cout << "3. Update Item Stock\n";
        cout << "4. Check Low Stock\n";
        cout << "5. Generate Report\n";
        cout << "6. Save Inventory to File\n";
        cout << "7. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                string name, expiration;
                int id, quantity;
                double price;
                char type;
                cout << "Enter Item ID: ";
                cin >> id;
                cout << "Enter Item Name: ";
                cin.ignore();
                getline(cin, name);
                cout << "Enter Stock Quantity: ";
                cin >> quantity;
                cout << "Enter Item Price: ";
                cin >> price;
                cout << "Is the item perishable (y/n)? ";
                cin >> type;

                if (type == 'y' || type == 'Y') {
                    cout << "Enter Expiration Date: ";
                    cin.ignore();
                    getline(cin, expiration);
                    inv.addItem(new PerishableItem(name, id, quantity, price, expiration));
                } else {
                    inv.addItem(new NonPerishableItem(name, id, quantity, price));
                }
                break;
            }
            case 2: {
                int id;
                cout << "Enter Item ID to remove: ";
                cin >> id;
                inv.removeItem(id);
                break;
            }
            case 3: {
                int id, quantity;
                cout << "Enter Item ID to update: ";
                cin >> id;
                cout << "Enter quantity to add/remove: ";
                cin >> quantity;
                inv.updateItemStock(id, quantity);
                break;
            }
            case 4: {
                int threshold;
                cout << "Enter stock threshold: ";
                cin >> threshold;
                inv.checkLowStock(threshold);
                break;
            }
            case 5: {
                inv.generateReport();
                break;
            }
            case 6: {
                inv.saveInventoryToFile();
                break;
            }
            case 7:
                cout << "Exiting the program.\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 7);

    return 0;
}
