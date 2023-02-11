#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <mutex>

using namespace std;

enum States
{
    ARRIVE,
    CHECK_MAINTENANCE,
    MAINTENANCE,
    WAIT_SUPPLY,
    LOAD_IRON,
    LOAD_WOOD,
    LEAVE
};

enum TrainType
{
    SLOW=1,
    MEDIUM,
    FAST
};

class Train
{
private:

    States state;
    TrainType train_type;

    bool iron_supply;
    bool wood_supply;
    bool need_maintenance;
    string train_name;
    thread *train_thread;

    void changeState();
    void trainThread();

public:
    Train(TrainType type, bool needIronLoad, bool needWoodLoad, bool needMaintenance);
    ~Train();
    States getState();
    string getType();
    void start();

};
