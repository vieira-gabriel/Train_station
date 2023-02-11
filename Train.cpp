#include "Train.h"
#include <chrono>

mutex maint_mtx;
mutex iron_mtx;
mutex wood_mtx;

Train::Train(TrainType type, bool needIronLoad, bool needWoodLoad, bool needMaintenance):
    train_type(type),
    iron_supply(needIronLoad),
    wood_supply(needWoodLoad),
    need_maintenance(needMaintenance)
{
    state = ARRIVE;
    train_thread = new thread(&Train::trainThread, this);

    this->train_name = this->getType();
}

Train::~Train()
{
    cout << "Train " << train_name << " leaving station. Bye bye!" << endl;
}

void Train::changeState()
{
    int time_sleep = 0;
    switch (state)
    {
    case ARRIVE:
        if(train_type == SLOW)
            time_sleep = 8;
        else
            time_sleep = 5;      
        state = CHECK_MAINTENANCE;
        this_thread::sleep_for(chrono::seconds(time_sleep));
        
        cout << "Train " << train_name << " arrived in " << time_sleep << " seconds" << endl;

        this->changeState();
        break;
    
    case CHECK_MAINTENANCE:
        if(need_maintenance)
        {
            cout << "Train " << train_name << " needs maintenance" << endl;
            state = MAINTENANCE;
        }
        else
        {
            cout << "Train " << train_name << " don't needs maintenance" << endl;
            state = WAIT_SUPPLY;
        }
        this->changeState();
        break;

    case MAINTENANCE:
        cout << "Train " << train_name << " waiting for maintance" << endl;

        maint_mtx.lock();

        cout << "Train " << train_name << " in maintance" << endl;

        if(train_type == FAST)
            time_sleep = 2;
        else
            time_sleep = 1;
        this_thread::sleep_for(chrono::seconds(time_sleep));
        cout << "Train " << train_name << " finished maintance in " << time_sleep << " seconds" << endl;
        state = WAIT_SUPPLY;
        
        maint_mtx.unlock();

        this->changeState();
        break;
    
    case WAIT_SUPPLY:
        if(wood_supply)
            state = LOAD_WOOD;
        else if(iron_supply)
            state = LOAD_IRON;
        this->changeState();
        break;

    case LOAD_IRON:
        cout << "Train " << train_name << " waiting for iron supply" << endl;
        
        iron_mtx.lock();
        
        cout << "Train " << train_name << " in filling up with iron" << endl;

        if(train_type == MEDIUM)
            time_sleep = 6;
        else
            time_sleep = 3;
        
        this_thread::sleep_for(chrono::seconds(time_sleep));
        cout << "Train " << train_name << " finished iron supply in " << time_sleep << " seconds" << endl;

        state = LEAVE;
        iron_mtx.unlock();
        this->changeState();
        break;

    case LOAD_WOOD:
        cout << "Train " << train_name << " waiting for wood supply" << endl;
        
        wood_mtx.lock();
        cout << "Train " << train_name << " in filling up with wood" << endl;
        
        if(train_type == SLOW)
            time_sleep = 6;
        else
            time_sleep = 3;

        this_thread::sleep_for(chrono::seconds(time_sleep));
        cout << "Train " << train_name << " finished wood supply in " << time_sleep << " seconds" << endl;

        
        state = LEAVE;
        if(iron_supply)
        {
            wood_supply = false;
            state = WAIT_SUPPLY;
        }
        wood_mtx.unlock();
        this->changeState();
        break;

    case LEAVE:
        if(train_type == SLOW)
            this_thread::sleep_for(chrono::seconds(6));
        else
            this_thread::sleep_for(chrono::seconds(5));
    
        break;

    default:
        break;
    }
}

void Train::trainThread()
{
    this->changeState();
}

string Train::getType()
{
    switch (train_type)
    {
    case SLOW: return "Slow";
    case MEDIUM: return "Medium";
    case FAST: return "Fast";
    }
    return "NONE";
}

void Train::start()
{
    if(train_thread->joinable())
        train_thread->join();
    else
        cout << "Deadline loss" << endl;
}
