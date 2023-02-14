#include "Train.h"
#include <chrono>

Train::Train(TrainType type, bool needIronLoad, bool needWoodLoad, bool needMaintenance):
    train_type(type),
    iron_supply(needIronLoad),
    wood_supply(needWoodLoad),
    need_maintenance(needMaintenance)
{
    state = ARRIVE;
    this->train_name = this->getType();
}

Train::~Train()
{}

void Train::changeState()
{
    int time_sleep = 0;
    switch (state)
    {
    case ARRIVE:
        output.lock();
        cout << "Train " << train_name << " enterring station" << endl;
        output.unlock();
        
        if(train_type == SLOW)
            time_sleep = 80;
        else
            time_sleep = 50;      
        state = CHECK_MAINTENANCE;
        this_thread::sleep_for(chrono::seconds(time_sleep));
        
        output.lock();
        cout << "Train " << train_name << " arrived in " << time_sleep << " seconds" << endl;
        output.unlock();

        this->changeState();
        break;
    
    case CHECK_MAINTENANCE:
        if(need_maintenance)
        {
            output.lock();
            cout << "Train " << train_name << " needs maintenance" << endl;
            output.unlock();
            state = MAINTENANCE;
        }
        else
        {
            output.lock();
            cout << "Train " << train_name << " don't needs maintenance" << endl;
            output.unlock();
            state = WAIT_SUPPLY;
        }
        this->changeState();
        break;

    case MAINTENANCE:
        output.lock();
        cout << "Train " << train_name << " waiting for maintance" << endl;
        output.unlock();

        maint_mtx.lock();

        output.lock();
        cout << "Train " << train_name << " in maintance" << endl;
        output.unlock();

        if(train_type == FAST)
            time_sleep = 20;
        else
            time_sleep = 10;
        this_thread::sleep_for(chrono::seconds(time_sleep));

        output.lock();
        cout << "Train " << train_name << " finished maintance in " << time_sleep << " seconds" << endl;
        output.unlock();
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
        output.lock();
        cout << "Train " << train_name << " waiting for iron supply" << endl;
        output.unlock();
        
        iron_mtx.lock();
        
        output.lock();
        cout << "Train " << train_name << " in filling up with iron" << endl;
        output.unlock();

        if(train_type == MEDIUM)
            time_sleep = 60;
        else
            time_sleep = 30;
        
        this_thread::sleep_for(chrono::seconds(time_sleep));
        output.lock();
        cout << "Train " << train_name << " finished iron supply in " << time_sleep << " seconds" << endl;
        output.unlock();

        state = LEAVE;
        iron_mtx.unlock();
        this->changeState();
        break;

    case LOAD_WOOD:
        output.lock();
        cout << "Train " << train_name << " waiting for wood supply" << endl;
        output.unlock();
        
        wood_mtx.lock();
        output.lock();
        cout << "Train " << train_name << " in filling up with wood" << endl;
        output.unlock();
        
        if(train_type == SLOW)
            time_sleep = 60;
        else
            time_sleep = 30;

        this_thread::sleep_for(chrono::seconds(time_sleep));
        output.lock();
        cout << "Train " << train_name << " finished wood supply in " << time_sleep << " seconds" << endl;
        output.unlock();
        
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
    time(&this->start_time);
    this->changeState();
    time(&this->end_time);
    double time_diff = double(this->end_time - this->start_time);
    output.lock();
    cout << "Train " << train_name << " leaving station. Bye bye! Finished in " << time_diff << " seconds" << endl;
    output.unlock();
}
