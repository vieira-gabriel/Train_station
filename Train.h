#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <map>

using namespace std;

extern mutex maint_mtx, iron_mtx, wood_mtx, output;

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

extern map<TrainType, bool> trainMap;
class Train
{
private:

    States state;
    TrainType train_type;

    bool iron_supply;
    bool wood_supply;
    bool need_maintenance;
    bool started;
    string train_name;
    time_t start_time;
    time_t end_time;

    void changeState();

public:
    Train(TrainType type, bool needIronLoad, bool needWoodLoad, bool needMaintenance);
    ~Train();
    States getState();
    string getType();
    void start();

};
