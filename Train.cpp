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
    // #ifdef PRIORITY
    // if(g_current_priority > (int)train_type)
    // {
    //     g_output.lock();
    //     cout << "\e[1mTrain " << train_name << " Have more priority\e[0m" << endl;
    //     g_output.unlock();

    //     if(g_current_priority != 4)
    //         g_priority_mtx.unlock();

    //     g_output.lock();
    //     cout << "\e[1mChange current priority from " << g_current_priority << " to " << (int)train_type << "\e[0m" << endl;
    //     g_current_priority = (int)train_type;
    //     g_output.unlock();
    //     g_priority_mtx.lock();
    // }
    // #endif
    

    time_t startState = time(0);
    int time_sleep = 0;

    #ifdef PRIORITY
    checkPriority();
    #endif

    switch (state)
    {
    case ARRIVE:
        g_output.lock();
        cout << "\e[1mTrain " << train_name << " enterring station\e[0m" << endl;
        g_output.unlock();
        
        if(train_type == SLOW)
            time_sleep = 8 * g_multiplier;
        else
            time_sleep = 5 * g_multiplier;      
        state = CHECK_MAINTENANCE;
        this_thread::sleep_for(chrono::seconds(time_sleep));
        
        g_output.lock();
        cout << "Train " << train_name << " arrived in \e[1m" << difftime(time(0),startState) << "\e[0m seconds." << endl << "   Expected \e[1m" << time_sleep << "\e[0m seconds" << endl;
        g_output.unlock();

        this->changeState();
        break;
    
    case CHECK_MAINTENANCE:
        if(need_maintenance)
        {
            g_output.lock();
            cout << "Train " << train_name << " needs maintenance" << endl;
            g_output.unlock();
            state = MAINTENANCE;
        }
        else
        {
            g_output.lock();
            cout << "Train " << train_name << " don't needs maintenance" << endl;
            g_output.unlock();
            state = WAIT_SUPPLY;
        }
        this->changeState();
        break;

    case MAINTENANCE:
        g_output.lock();
        cout << "Train " << train_name << " waiting for maintance" << endl;
        g_output.unlock();

        g_maint_mtx.lock();

        g_output.lock();
        cout << "Train " << train_name << " in maintance" << endl;
        g_output.unlock();

        if(train_type == FAST)
            time_sleep = 2 * g_multiplier;
        else
            time_sleep = 1 * g_multiplier;
        this_thread::sleep_for(chrono::seconds(time_sleep));

        g_output.lock();
        cout << "Train " << train_name << " finished maintance in \e[1m" << difftime(time(0),startState) << "\e[0m seconds." << endl << "   Expected \e[1m" << time_sleep << "\e[0m seconds" << endl;
        g_output.unlock();
        state = WAIT_SUPPLY;
        
        g_maint_mtx.unlock();

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
        g_output.lock();
        cout << "Train " << train_name << " waiting for iron supply" << endl;
        g_output.unlock();
        
        g_iron_mtx.lock();
        
        g_output.lock();
        cout << "Train " << train_name << " in filling up with iron" << endl;
        g_output.unlock();

        if(train_type == MEDIUM)
            time_sleep = 6 * g_multiplier;
        else
            time_sleep = 3 * g_multiplier;
        
        this_thread::sleep_for(chrono::seconds(time_sleep));
        g_output.lock();
        cout << "Train " << train_name << " finished iron supply in \e[1m" << difftime(time(0),startState) << "\e[0m seconds." << endl << "   Expected \e[1m" << time_sleep << "\e[0m seconds" << endl;
        g_output.unlock();

        state = LEAVE;
        g_iron_mtx.unlock();
        
        this->changeState();
        break;

    case LOAD_WOOD:
        g_output.lock();
        cout << "Train " << train_name << " waiting for wood supply" << endl;
        g_output.unlock();
        
        g_wood_mtx.lock();
        g_output.lock();
        cout << "Train " << train_name << " in filling up with wood" << endl;
        g_output.unlock();
        
        if(train_type == SLOW)
            time_sleep = 6 * g_multiplier;
        else
            time_sleep = 3 * g_multiplier;

        this_thread::sleep_for(chrono::seconds(time_sleep));
        g_output.lock();
        cout << "Train " << train_name << " finished wood supply in \e[1m" << difftime(time(0),startState) << "\e[0m seconds." << endl << "   Expected \e[1m" << time_sleep << "\e[0m seconds" << endl;
        g_output.unlock();
        
        state = LEAVE;
        if(iron_supply)
        {
            wood_supply = false;
            state = WAIT_SUPPLY;
        }
        g_wood_mtx.unlock();
        
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
    
    g_output.lock();
    cout << "\e[1mTrain " << train_name << " leaving station. Bye bye! Finished in " << time_diff << " seconds\e[0m" << endl;
    g_output.unlock();
    
    #ifdef PRIORITY
    g_priority_mtx.unlock();
    #endif
}

void Train::checkPriority()
{
    bool stop = true;
    bool logged = false;

    do
    {
        this_thread::sleep_for(chrono::seconds(2));

        int other_priority_1 = (int)train_type -1;
        int other_priority_2 = (int)train_type -2;
        if(other_priority_1 > 0)
        {
            map<TrainType, bool>::iterator it = g_trainMap.find((TrainType)other_priority_1);

            if(it->second == true)
            {
                if(!logged)
                {
                    g_output.lock();
                    cout << "\e[1mTrain " << train_name << " stoped due to higher priority\e[0m" << endl;
                    g_output.unlock();
                    
                    logged = true;
                }
                
                continue;
            }
        }
        if(other_priority_2 > 0)
        {
            map<TrainType, bool>::iterator it = g_trainMap.find((TrainType)other_priority_2);

            if(it->second == true)
            {
                if(!logged)
                {
                    g_output.lock();
                    cout << "\e[1mTrain " << train_name << " stoped due to higher priority\e[0m" << endl;
                    g_output.unlock();
                    
                    logged = true;
                }
                
                continue;
            }
        }
        stop = false;
    } while (stop);
    
}
