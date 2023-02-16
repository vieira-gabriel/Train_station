#include "Train.h"
#include <chrono>

Train::Train(TrainType type, bool needIronLoad, bool needWoodLoad, bool needMaintenance):
    train_type(type),
    iron_supply(needIronLoad),
    wood_supply(needWoodLoad),
    need_maintenance(needMaintenance)
{
    state = ARRIVE;
    loggedStop = false;
    this->train_name = this->getType();
    thisPriority = int(type);
    
    map<TrainType, int>::iterator it = g_trainPriority.find(train_type);

    if(it == g_trainPriority.end())
    {
        #ifdef LOGON
        g_output.lock();
        cout << "Inserting " << train_name << " to priority map with value " << thisPriority << endl;
        g_output.unlock();
        #endif
        g_trainPriority.insert(make_pair(train_type,thisPriority));
    }
}

Train::~Train()
{}

void Train::changeState()
{
    #ifdef LOGON
    time_t startState = time(0);
    #endif
    int time_sleep = 0;

    #ifdef PRIORITY
    checkPriority();
    #endif

    switch (state)
    {
    case ARRIVE:
        time(&this->start_time);
        
        if(train_type == SLOW)
            time_sleep = 8 * g_multiplier;
        else
            time_sleep = 5 * g_multiplier;      
        state = CHECK_MAINTENANCE;
        this_thread::sleep_for(chrono::seconds(time_sleep));
        
        #ifdef LOGON
        g_output.lock();
        cout << "Train " << train_name << " arrived in \e[1m" << difftime(time(0),startState) << "\e[0m seconds." << endl << "   Expected \e[1m" << time_sleep << "\e[0m seconds" << endl;
        g_output.unlock();
        #endif

        this->changeState();
        break;
    
    case CHECK_MAINTENANCE:
        if(need_maintenance)
        {
            #ifdef LOGON
            g_output.lock();
            cout << "Train " << train_name << " needs maintenance" << endl;
            g_output.unlock();
            #endif
            state = MAINTENANCE;
        }
        else
        {
            #ifdef LOGON
            g_output.lock();
            cout << "Train " << train_name << " don't needs maintenance" << endl;
            g_output.unlock();
            #endif
            state = WAIT_SUPPLY;
        }
        this->changeState();
        break;

    case MAINTENANCE:
        #ifdef LOGON
        g_output.lock();
        cout << "Train " << train_name << " waiting for maintance" << endl;
        g_output.unlock();
        #endif

        g_maint_mtx.lock();

        thisPriority = g_resourcePriority.find("maintance")->second;
        g_trainPriority.find(train_type)->second = thisPriority;

        #ifdef LOGON
        g_output.lock();
        cout << "Train " << train_name << " in maintance" << endl;
        g_output.unlock();
        #endif

        if(train_type == FAST)
            time_sleep = 2 * g_multiplier;
        else
            time_sleep = 1 * g_multiplier;
        this_thread::sleep_for(chrono::seconds(time_sleep));

        #ifdef LOGON
        g_output.lock();
        cout << "Train " << train_name << " finished maintance in \e[1m" << difftime(time(0),startState) << "\e[0m seconds." << endl << "   Expected \e[1m" << time_sleep << "\e[0m seconds" << endl;
        g_output.unlock();
        #endif
        state = WAIT_SUPPLY;
        
        g_maint_mtx.unlock();
        thisPriority = int(train_type);
        g_trainPriority.find(train_type)->second = thisPriority;

        this->changeState();
        break;
    
    case WAIT_SUPPLY:
        if(wood_supply)
            state = LOAD_WOOD;
        else if(iron_supply)
            state = LOAD_IRON;
        else
            state = LEAVE;
            
        this->changeState();
        break;

    case LOAD_IRON:
        #ifdef LOGON
        g_output.lock();
        cout << "Train " << train_name << " waiting for iron supply" << endl;
        g_output.unlock();
        #endif
        
        g_iron_mtx.lock();

        thisPriority = g_resourcePriority.find("iron")->second;
        g_trainPriority.find(train_type)->second = thisPriority;

        #ifdef LOGON
        g_output.lock();
        cout << "Train " << train_name << " in filling up with iron" << endl;
        g_output.unlock();
        #endif

        if(train_type == MEDIUM)
            time_sleep = 6 * g_multiplier;
        else
            time_sleep = 3 * g_multiplier;
        
        this_thread::sleep_for(chrono::seconds(time_sleep));
        #ifdef LOGON
        g_output.lock();
        cout << "Train " << train_name << " finished iron supply in \e[1m" << difftime(time(0),startState) << "\e[0m seconds." << endl << "   Expected \e[1m" << time_sleep << "\e[0m seconds" << endl;
        g_output.unlock();
        #endif

        state = LEAVE;
        g_iron_mtx.unlock();
        thisPriority = int(train_type);
        g_trainPriority.find(train_type)->second = thisPriority;
        
        this->changeState();
        break;

    case LOAD_WOOD:
        #ifdef LOGON
        g_output.lock();
        cout << "Train " << train_name << " waiting for wood supply" << endl;
        g_output.unlock();
        #endif
        
        g_wood_mtx.lock();
        
        thisPriority = g_resourcePriority.find("wood")->second;
        g_trainPriority.find(train_type)->second = thisPriority;

        #ifdef LOGON
        g_output.lock();
        cout << "Train " << train_name << " in filling up with wood" << endl;
        g_output.unlock();
        #endif
        
        if(train_type == SLOW)
            time_sleep = 6 * g_multiplier;
        else
            time_sleep = 3 * g_multiplier;

        this_thread::sleep_for(chrono::seconds(time_sleep));
        #ifdef LOGON
        g_output.lock();
        cout << "Train " << train_name << " finished wood supply in \e[1m" << difftime(time(0),startState) << "\e[0m seconds." << endl << "   Expected \e[1m" << time_sleep << "\e[0m seconds" << endl;
        g_output.unlock();
        #endif
        
        state = LEAVE;
        if(iron_supply)
        {
            wood_supply = false;
            state = WAIT_SUPPLY;
        }
        g_wood_mtx.unlock();
        thisPriority = int(train_type);
        g_trainPriority.find(train_type)->second = thisPriority;
        
        this->changeState();
        break;

    case LEAVE:
        if(train_type == SLOW)
            this_thread::sleep_for(chrono::seconds(6));
        else
            this_thread::sleep_for(chrono::seconds(5));

        time(&this->end_time);
        
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
    g_output.lock();
    cout << "\e[1mTrain " << train_name << " arrived at the station\e[0m" << endl;
    g_output.unlock();

    this->changeState();
    
    g_output.lock();
    cout << "\e[1mTrain " << train_name << " leaving station. Bye bye! Finished in " << difftime(this->end_time,this->start_time) << " seconds\e[0m" << endl;
    g_output.unlock();
}

void Train::checkPriority()
{
    bool stop = true;

    do
    {
        this_thread::sleep_for(chrono::seconds(2));

        if(stopByPreemprion((int)train_type -1))
            continue;

        if(stopByPreemprion((int)train_type -2))
            continue;

        
        if(stopByCeiling((int)train_type + 1))
            continue;

        if(stopByCeiling((int)train_type + 2))
            continue;

        stop = false;
    } while (stop);

    if(loggedStop)
        cout << "\e[1mTrain " << train_name << " continuing\e[0m" << endl;

    loggedStop = false;
}

bool Train::stopByPreemprion(int value)
{
    if(value >= (int)TrainType::SLOW)
    {
        map<TrainType, int>::iterator it_priority = g_trainPriority.find((TrainType)value);
        map<TrainType, bool>::iterator it_train = g_trainMap.find((TrainType)value);

        if(it_train->second && it_priority->second < thisPriority && it_priority != g_trainPriority.end())
        {
            if(!loggedStop)
            {
                g_output.lock();
                cout << "\e[1mTrain " << train_name << " stoped due to higher priority\e[0m" << endl;
                #ifdef LOGON
                cout << "  \e[1mIt's priority: " << (int)train_type << ". One above: " << it_priority->second << "\e[0m" << endl;
                #endif
                g_output.unlock();
                
                loggedStop = true;
            }
            return true;
        }
    }
    return false;
}

bool Train::stopByCeiling(int value)
{
    if(value <= (int)TrainType::FAST)
    {
        map<TrainType, int>::iterator it_priority = g_trainPriority.find((TrainType)value);
        map<TrainType, bool>::iterator it_train = g_trainMap.find((TrainType)value);

        if(it_train->second && it_priority->second < thisPriority && it_priority != g_trainPriority.end())
        {
            if(!loggedStop)
            {
                g_output.lock();
                cout << "\e[1mTrain " << train_name << " stoped due to ceiling priority\e[0m" << endl;
                #ifdef LOGON
                cout << "  \e[1mIt's priority: " << (int)train_type << ". One above: " << it_priority->second << "\e[0m" << endl;
                #endif
                g_output.unlock();
                
                loggedStop = true;
            }
            return true;
        }
    }
    return false;
}
