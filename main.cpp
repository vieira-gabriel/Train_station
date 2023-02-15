#include "Train.h"

mutex g_maint_mtx;
mutex g_iron_mtx;
mutex g_wood_mtx;
mutex g_output;
mutex g_map_mtx;
mutex g_priority_mtx;
map<TrainType, bool> g_trainMap;

const int g_multiplier = 2;

void trainArrival(TrainType type, bool needIronLoad, bool needWoodLoad, bool needMaintenance)
{
    Train newTrain((TrainType)type, needIronLoad, needWoodLoad, needMaintenance);

    map<TrainType, bool>::iterator it = g_trainMap.find(type);
    if(it->second == true)
    {
        cout << "\e[2m" << newTrain.getType() << " Train Deadline loss\e[0m" << endl;
        return;
    }
    g_map_mtx.lock();
    it->second = true;
    g_map_mtx.unlock();

    newTrain.start();

    g_map_mtx.lock();
    it->second = false;
    g_map_mtx.unlock();
}

int main(int argc, char const *argv[])
{
    vector<thread> threads;

    int tests = 0;
    int maxTests = 15;

    int slowT = 50 * g_multiplier;
    int medT = 80 * g_multiplier;
    int fastT = 100 * g_multiplier;

    cout << "Trains to arrive: " << maxTests << ". Multiplier: " << g_multiplier << endl << endl;
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

    srand(time(0));
    // const int minRange = 1, maxRange = 3;


    g_trainMap.insert(make_pair(TrainType::SLOW,false));
    g_trainMap.insert(make_pair(TrainType::MEDIUM,false));
    g_trainMap.insert(make_pair(TrainType::FAST,false));
    
    cout << "Station ready" << endl;
    do
    {
        // int trainToGenerate = minRange + (rand()%maxRange);
        time_t current_time;
        time(&current_time);

        time(&current_time);
        if(difftime(current_time,fast_timer) >= fastT)
        {
            time(&fast_timer);
            threads.emplace_back(thread(trainArrival,TrainType::FAST,false, true, true));
            ++tests;
            this_thread::sleep_for(chrono::seconds(10));
        }

        time(&current_time);
        if(difftime(current_time,med_timer) >= medT)
        {
            time(&med_timer);
            threads.emplace_back(thread(trainArrival,TrainType::MEDIUM, true, false, false));
            ++tests;
            this_thread::sleep_for(chrono::seconds(10));
        }


        if(difftime(current_time,slow_timer) >= slowT)
        {
            time(&slow_timer);
            threads.emplace_back(thread(trainArrival,TrainType::SLOW, true, true, true));
            ++tests;
        }
            

        this_thread::sleep_for(chrono::seconds(10));
    } while (tests < maxTests);

    for (auto& th : threads)
    {
        if(th.joinable())
            th.join();
    }
    

    this_thread::sleep_for(chrono::seconds(10));

    return 0;
}
