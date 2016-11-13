#include "lab1.h"

// argument order is numStations, packetLength, rate, arrivalRate and then optional p for ppersistent
void main(int argc, char* argv[])
{
  srand(time(NULL));
  const int numberOfStations = atoi(argv[1]);
  const int packetLength = atoi(argv[2]);
  const int rate = atoi(argv[3]);
  const int arrivalRate = atoi(argv[4]);
  Persistence mode;
  int p = 0;
  if (argc > 5)
  {
    p = atoi(argv[5]);
    mode = PPERSISTENT;
  }
  else
  {
    mode = NONPERSISTENT; 
  }
  const int numberOfTicks = 50000;
  const float tick_duration = 0.001;
  
  for (int numberOfStations = 20; numberOfStations <= 100; numberOfStations+=20){

    std::map<int, Simulator> allStationsArray;
    Bus centralBus;
    centralBus.setTickDuration(tick_duration);
    const int transmissionDelay = (packetLength / rate) * tick_duration;
    for (int i = 1; i <= numberOfStations; i++){
      allStationsArray.insert(std::map<int, Simulator>::value_type(i, Simulator(&centralBus, i, numberOfStations, transmissionDelay, arrivalRate, tick_duration, mode, p)));
    }
  
    for (int i = 0; i < numberOfTicks; i++){
      centralBus.decrementWaits(i);
      for (int j = 1; j <= numberOfStations; j++){
        allStationsArray.at(j).run(i);
      }
    }
    /*
    std::cout << "Arrival rate is : " << a << std::endl;
    std::cout << "Size of Bus: " << centralBus.bus.size() << std::endl;
    std::cout << "Successful Packets: " << centralBus.successfulPacketCount << std::endl;
    std::cout << "Number of Collisions: " << centralBus.numberOfCollisions << std::endl;
    std::cout << "Throughput: " << (centralBus.successfulPacketCount * packetLength) / (numberOfTicks*tick_duration) << std::endl;
    std::cout << "Delay: " << centralBus.totalDelay / centralBus.successfulPacketCount << std::endl; */
    std::cout << "Num of computers, Throughput, Delay" << std::endl;
    std::cout << numberOfStations << "," << (centralBus.successfulPacketCount * packetLength) / (numberOfTicks*tick_duration) << "," << centralBus.totalDelay / centralBus.successfulPacketCount << std::endl;

  }
  std::cin.get();
}
