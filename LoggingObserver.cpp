#include "LoggingObserver.h"
#include <fstream>
#include <algorithm>

void Subject::attach(Observer* observer) {
    if (std::find(observers.begin(), observers.end(), observer) == observers.end()) {
    observers.push_back(observer);
    }
}

void Subject::detach(Observer* observer) {
    observers.erase(
        std::remove(observers.begin(), observers.end(), observer), observers.end()
    );
}

void Subject::notify(const ILoggable* loggable) {
    for (Observer* observer : observers) {
        observer->update(loggable);
    }
}

void LogObserver::update(const ILoggable* loggable) {
    if (loggable == nullptr) return; // safety check

    std::ofstream logFile("gamelog.txt", std::ios::app);

    logFile << loggable->stringToLog() << std::endl;
}

