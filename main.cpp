#include "lab1.h"

void main(int argc, char* argv[])
{
  srand(time(NULL));
  const int numberOfStations = atoi(argv[1]);
  const int packetLength = atoi(argv[2]);
  const int rate = atoi(argv[3]);
  const int arrivalRate = atoi(argv[4]);
  Persistence mode;
  float p = 0;
  if (argc > 5) {
    p = atof(argv[5]);
    mode = PPERSISTENT;
  } else {
    mode = NONPERSISTENT; 
  }
  const int numberOfTicks = 50000000;
  const float tick_duration = 1/(pow(10,8));
  const float bit_time = 1 / float(rate);
  const float transmissionDelay = (((float)packetLength / (float)rate));

  // to switch between non-persistent and p-persistent, change the for loop
  //for(int numberOfStations = 20; numberOfStations <= 100; numberOfStations++){
  for (int arrivalRate = 1; arrivalRate <= 10; arrivalRate++) {
    float totalThroughput = 0;
    float totalDelay = 0;
	for (int average = 0; average < 5; average++) {
		std::map<int, Simulator> allStationsArray;
		Bus centralBus;
		centralBus.setTickDuration(tick_duration);

		for (int i = 1; i <= numberOfStations; i++) {
			allStationsArray.insert(std::map<int, Simulator>::value_type(i, Simulator(&centralBus, i, numberOfStations, transmissionDelay, arrivalRate, tick_duration, mode, p, bit_time, numberOfTicks)));
		}

		for (int i = 0; i < numberOfTicks; i++) {
			centralBus.decrementWaits(i);
			for (int j = 1; j <= numberOfStations; j++) {
				allStationsArray.at(j).run(i);
			}
		}

		float throughput = (float(centralBus.successfulPacketCount * packetLength) / (float(numberOfTicks) * tick_duration)) / float(1000000);
		float delay = (float)centralBus.totalDelay / (float)centralBus.successfulPacketCount;
		totalThroughput += throughput;
		totalDelay += delay;
	}
    std::cout << "Arrival rate" <<  arrivalRate << std::endl;
    std::cout << "Average throughput: " << totalThroughput / 5 << std::endl;
    std::cout << "Average delay: " << totalDelay / 5 << std::endl;
  }
  std::cin.get();
}
