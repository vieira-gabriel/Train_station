#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <map>
#include <vector>

using namespace std;

extern mutex g_maint_mtx, g_iron_mtx, g_wood_mtx, g_output;
extern int g_current_priority;
extern int g_multiplier;

extern map<string, int> g_resourcePriority;

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

extern map<TrainType, bool> g_trainMap;
extern map<TrainType, int> g_trainPriority;

class Train
{
private:

    States state;
    TrainType train_type;

    bool iron_supply;
    bool wood_supply;
    bool need_maintenance;
    bool started;
    bool loggedStop;
    string train_name;
    time_t start_time;
    time_t end_time;
    int thisPriority;

    void changeState();
    void checkPriority();
    bool stopByPreemprion(int value);
    bool stopByCeiling(int value);

public:
    Train(TrainType type, bool needIronLoad, bool needWoodLoad, bool needMaintenance);
    ~Train();
    States getState();
    string getType();
    void start();

};
