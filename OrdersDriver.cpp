#include "Orders.h"
#include <iostream>

using std::cout;
using std::endl;


int main() {

    // 1) Create orders of every kind and add them to OrdersList
    // (Requirement: create every kind, place them in OrdersList) :contentReference[oaicite:6]{index=6} :contentReference[oaicite:7]{index=7}
    OrdersList list;

    // Valid-ish examples (logic can still be placeholder for A1)
    list.add(new Deploy(5, "Alaska"));
    list.add(new Advance(3, "Alaska", "Northwest Territory"));
    list.add(new Bomb("Greenland"));
    list.add(new Blockade("Ontario"));
    list.add(new Airlift(7, "Brazil", "Ural"));
    list.add(new Negotiate("Player2"));

    // Add an intentionally "invalid" example too (to show invalid executes do nothing)
    // Requirement: invalid orders may exist; execute shouldn't result in action :contentReference[oaicite:8]{index=8}
    list.add(new Bomb(""));                 // invalid if your validate checks empty target
    list.add(new Deploy(-10, "Alaska"));    // invalid if your validate checks armies >= 0

    cout << "Initial OrdersList (before move/remove)\n";
    cout << list << endl;

    // 2) Demonstrate move()
    // Requirement: OrdersList implements move() and driver demonstrates it :contentReference[oaicite:9]{index=9}
    cout << "Demonstrate move()\n";
    cout << "Moving index 0 -> index 3...\n";
    bool moved = list.move(0, 3);
    cout << "move result: " << (moved ? "true" : "false") << "\n";
    cout << list << endl;

    // 3) Demonstrate remove()
    // Requirement: OrdersList implements remove() and driver demonstrates it :contentReference[oaicite:10]{index=10}
    cout << "Demonstrate remove()\n";
    cout << "Removing order at index 2...\n";
    bool removed = list.remove(2);
    cout << "remove result: " << (removed ? "true" : "false") << "\n";
    cout << list << endl;

    // 4) Demonstrate validate() + execute() + effect printing
    // Requirement: execute validates first, executes if valid :contentReference[oaicite:11]{index=11}
    // Requirement: operator<< prints description, and if executed prints effect string :contentReference[oaicite:12]{index=12}
    cout << "Execute all orders (shows effect after execution)\n";
    for (int i = 0; i < list.size(); ++i) {
        const Order* o = list.get(i);
        if (!o) continue;

        cout << "Before execute: " << *o << "\n";

        // Need non-const to execute; if your get() returns const Order*,
        // you can add a non-const accessor OR cast here for the driver.
        // Better: implement OrdersList::get(int) -> Order* overload.
        Order* mutableOrder = const_cast<Order*>(o);
        bool ok = mutableOrder->validate();
        cout << "validate(): " << (ok ? "true" : "false") << "\n";
        mutableOrder->execute();

        cout << "After execute:  " << *mutableOrder << "\n\n";
    }

    cout << "Final OrdersList (after execution)\n";
    cout << list << endl;

    cout << "Driver finished\n";
    return 0;
}
