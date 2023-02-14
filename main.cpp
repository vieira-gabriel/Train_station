#include "Train.h"
#include <vector>

mutex maint_mtx;
mutex iron_mtx;
mutex wood_mtx;
mutex output;
mutex map_mtx;
map<TrainType, bool> trainMap;

void trainArrival(TrainType type, bool needIronLoad, bool needWoodLoad, bool needMaintenance)
{
    Train newTrain((TrainType)type, needIronLoad, needWoodLoad, needMaintenance);

    map<TrainType, bool>::iterator it = trainMap.find(type);
    if(it->second == true)
    {
        output.lock();
        cout << newTrain.getType() << " Train Deadline loss" << endl;
        output.unlock();
        return;
    }
    map_mtx.lock();
    it->second = true;
    map_mtx.unlock();

    newTrain.start();

    map_mtx.lock();
    it->second = false;
    map_mtx.unlock();
}

int main(int argc, char const *argv[])
{
    vector<thread> threads;

    trainMap.insert(make_pair(TrainType::SLOW,false));
    trainMap.insert(make_pair(TrainType::MEDIUM,false));
    trainMap.insert(make_pair(TrainType::FAST,false));

    cout << "Station ready" << endl;

    threads.emplace_back(thread(trainArrival,TrainType::SLOW, true, true, true));
    threads.emplace_back(thread(trainArrival,TrainType::MEDIUM, true, false, false));
    threads.emplace_back(thread(trainArrival,TrainType::FAST,false, true, true));
    threads.emplace_back(thread(trainArrival,TrainType::SLOW, true, true, true));

    for (auto& th : threads)
    {
        if(th.joinable())
            th.join();
    }

    this_thread::sleep_for(chrono::seconds(10));

    return 0;
}
