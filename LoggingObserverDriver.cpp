#include "LoggingObserver.h"
#include "CommandProcessing.h"
#include "GameEngine.h"
#include "Orders.h"

int main() {

    LogObserver logger;

    CommandProcessor cp;
    GameEngine engine;
    OrdersList orderList;

    cp.attach(&logger);
    engine.attach(&logger);
    orderList.attach(&logger);

    // Command logging
    Command cmd("loadmap europe.map");
    cmd.attach(&logger);

    cp.saveCommand(&cmd);
    cmd.saveEffect("Map loaded successfully");

    // GameEngine logging
    engine.setState(MapLoaded);

    // Order logging
    Order* order = new Deploy();
    order->attach(&logger);

    orderList.add(order);
    order->execute();

    return 0;
}