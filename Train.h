#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <mutex>

using namespace std;

mutex maint_mtx;
mutex iron_mtx;
mutex wood_mtx;

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

    void changeState();
    void trainThread();

public:
    Train(TrainType, bool, bool, bool);
    ~Train();
    States getState();
    string getType();

};
