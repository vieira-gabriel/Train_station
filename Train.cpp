#include "Train.h"
#include <chrono>

Train::Train(TrainType type, bool needIronLoad, bool needWoodLoad, bool needMaintenance):
    train_type(type),
    iron_supply(needIronLoad),
    wood_supply(needWoodLoad),
    need_maintenance(needMaintenance)
{
    state = ARRIVE;
    thread train_thread(Train::trainThread);

    train_thread.join();
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
            this_thread::sleep_for(chrono::seconds(1));
        state = WAIT_SUPPLY;
        maint_mtx.unlock();

        cout << "Train " << train_name << " finished maintance in " <<  << endl;
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
        iron_mtx.lock();
        if(train_type == MEDIUM)
            this_thread::sleep_for(chrono::seconds(6));
        else
            this_thread::sleep_for(chrono::seconds(3));
        
        state = LEAVE;
        iron_mtx.unlock();
        this->changeState();
        break;

    case LOAD_WOOD:
        wood_mtx.lock();
        if(train_type == SLOW)
            this_thread::sleep_for(chrono::seconds(6));
        else
            this_thread::sleep_for(chrono::seconds(3));
        
        state = LEAVE;
        if(iron_supply)
        {
            wood_supply == false;
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
    delete this;
}

string Train::getType()
{
    switch (train_type)
    {
    case SLOW: return "Slow";
    case MEDIUM: return "Medium";
    case FAST: return "Fast";
    }
}
