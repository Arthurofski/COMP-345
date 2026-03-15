// Loggable Interface
#ifndef LOGGINGOBSERVER_H
#define LOGGINGOBSERVER_H

#include <string>
#include <vector>

class ILoggable {
public:
    virtual std::string stringToLog() const = 0;
    virtual ~ILoggable() = default;
};


// Observer Interface

class Observer {
    public:
        virtual void update( const ILoggable* loggable) = 0;
        virtual ~Observer() = default;
};


// Subject 

class Subject {
    private:
        std::vector<Observer*> observers;
    public:
        void attach(Observer* observer);
        void detach(Observer* observer);
        void notify(const ILoggable* loggable);
        virtual ~Subject() = default;
};

class LogObserver : public Observer {
    public:
        void update(const ILoggable* loggable);
};

#endif