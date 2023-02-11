#include "Train.h"

int main(int argc, char const *argv[])
{
    Train slowTrain(TrainType::SLOW, true, true, true);
    Train mediumTrain(TrainType::MEDIUM, true, false, false);
    Train fastTrain(TrainType::FAST,false, true, true);

    cout << "Station ready" << endl;

    slowTrain.start();
    mediumTrain.start();
    fastTrain.start();

    return 0;
}
