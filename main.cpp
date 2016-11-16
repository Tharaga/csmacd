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
  float p = 0;
  if (argc > 5)
  {
    p = atof(argv[5]);
    mode = PPERSISTENT;
  }
  else
  {
    mode = NONPERSISTENT; 
  }
  const int numberOfTicks = 100000;
  const float tick_duration = 0.001;

  for (int numberOfStations = 20; numberOfStations <= 100; numberOfStations += 20){
    float totalThroughput = 0;
    float totalDelay = 0;
  for (int x = 0; x < 5; x++)
  {


    std::map<int, Simulator> allStationsArray;
    Bus centralBus;
    centralBus.setTickDuration(tick_duration);
    const float transmissionDelay = ((float)packetLength / (float)rate) * tick_duration;
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
    std::cout << "Size of Bus: " << centralBus.bus.size() << std::endl;
    std::cout << "Successful Packets: " << centralBus.successfulPacketCount << std::endl;
    std::cout << "Number of Collisions: " << centralBus.numberOfCollisions << std::endl;
    */
    float throughput = float(centralBus.successfulPacketCount * packetLength) / float(numberOfTicks*tick_duration);
    float delay = (float)centralBus.totalDelay / (float)centralBus.successfulPacketCount;
    totalThroughput += throughput;
    totalDelay += delay;
  /*  std::cout << "Throughput: " << throughput << std::endl;
    std::cout << "Delay: " << delay << std::endl;
    std::cout << "Num of computers, Throughput, Delay" << std::endl;
    std::cout << numberOfStations << "," << throughput << "," << delay << std::endl;*/
    
    }
    std::cout << "Number of stations" <<  numberOfStations << std::endl;
    std::cout << "Average throughput: " << totalThroughput / 5 << std::endl;
    std::cout << "Average delay: " << totalDelay / 5 << std::endl;
  }
  std::cin.get();
}
