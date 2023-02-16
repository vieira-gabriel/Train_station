#include "Train.h"

mutex g_maint_mtx;
mutex g_iron_mtx;
mutex g_wood_mtx;
mutex g_output;
mutex g_map_mtx;

map<TrainType, bool> g_trainMap;
map<string, int> g_resourcePriority;
map<TrainType, int> g_trainPriority;
map<TrainType, int> g_trainAttended;

int g_multiplier;

void trainArrival(TrainType type, bool needIronLoad, bool needWoodLoad, bool needMaintenance)
{
    Train newTrain((TrainType)type, needIronLoad, needWoodLoad, needMaintenance);

    map<TrainType, bool>::iterator it = g_trainMap.find(type);
    if(it->second == true)
    {
        #ifdef LOGON
        cout << "\e[2m Train Deadline loss\e[0m" << endl;
        #endif
        return;
    }
    map<TrainType, int>::iterator it_attended = g_trainAttended.find(type);

    g_map_mtx.lock();
    it->second = true;
    it_attended->second += 1;
    g_map_mtx.unlock();

    newTrain.start();

    newTrain.~Train();

    g_map_mtx.lock();
    it->second = false;
    g_map_mtx.unlock();
}

int main(int argc, char const *argv[])
{
    vector<thread> threads;

    g_multiplier = stoi(string(argv[1]));

    int tests = 0;
    int maxTests = stoi(string(argv[2]));

    int slowT = 50 * g_multiplier;
    int medT = 90 * g_multiplier;
    int fastT = 200 * g_multiplier;

    cout << "Trains to arrive: " << maxTests * 3 << ". Multiplier: " << g_multiplier << endl << endl;
    #ifdef PRIORITY
    cout << "Priority " << TrainType::SLOW ;
    #endif
    cout <<  " Slow train deadline: " << slowT << endl;
    #ifdef PRIORITY
    cout << "Priority " << TrainType::MEDIUM;
    #endif
    cout <<  " Medium train deadline: " << medT << endl;
    #ifdef PRIORITY
    cout << "Priority " << TrainType::FAST;
    #endif
    cout <<  " Fast train deadline: " << fastT << endl << endl;

    time_t slow_timer = time(0) - slowT;
    time_t med_timer = time(0) - medT;
    time_t fast_timer = time(0) - fastT;

    g_trainMap.insert(make_pair(TrainType::SLOW,false));
    g_trainMap.insert(make_pair(TrainType::MEDIUM,false));
    g_trainMap.insert(make_pair(TrainType::FAST,false));

    g_resourcePriority.insert(make_pair("wood",1));
    g_resourcePriority.insert(make_pair("iron",2));
    g_resourcePriority.insert(make_pair("maintenance",2));
    
    g_trainAttended.insert(make_pair(TrainType::SLOW,0));
    g_trainAttended.insert(make_pair(TrainType::MEDIUM,0));
    g_trainAttended.insert(make_pair(TrainType::FAST,0));
    
    cout << "Station ready" << endl;
    do
    {
        int fast_int = g_trainAttended.find(TrainType::FAST)->second;
        int med_int = g_trainAttended.find(TrainType::MEDIUM)->second;
        int slow_int = g_trainAttended.find(TrainType::SLOW)->second;
        // int trainToGenerate = minRange + (rand()%maxRange);
        time_t current_time;
        time(&current_time);

        time(&current_time);
        #ifdef LIMITATION
        if(difftime(current_time,fast_timer) >= fastT && fast_int < maxTests)
        #else
        if(difftime(current_time,fast_timer) >= fastT)
        #endif
        {
            time(&fast_timer);
            threads.emplace_back(thread(trainArrival,TrainType::FAST, true, false, true));
            this_thread::sleep_for(chrono::seconds(2));
        }

        time(&current_time);
        #ifdef LIMITATION
        if(difftime(current_time,med_timer) >= medT && med_int < maxTests)
        #else
        if(difftime(current_time,med_timer) >= medT)
        #endif
        {
            time(&med_timer);
            threads.emplace_back(thread(trainArrival,TrainType::MEDIUM, true, true, true));
            this_thread::sleep_for(chrono::seconds(2));
        }


        #ifdef LIMITATION
        if(difftime(current_time,slow_timer) >= slowT && slow_int < maxTests)
        #else
        if(difftime(current_time,slow_timer) >= slowT)
        #endif
        {
            time(&slow_timer);
            threads.emplace_back(thread(trainArrival,TrainType::SLOW, false, true, false));
        }

        this_thread::sleep_for(chrono::seconds(1));
        tests = fast_int + med_int + slow_int;
    } while (tests < maxTests * 3);

    for (auto& th : threads)
    {
        if(th.joinable())
            th.join();
    }

    return 0;
}
