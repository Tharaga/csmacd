#include "bus.h"
#include <map>

Bus::Bus()
{
  isCollision=false;
};

Bus::~Bus()
{
};


void Bus::setTickDuration(float inputTickDuration){
  tick_duration = inputTickDuration;
}

// call this every tick
void Bus::decrementWaits(int curTick){
  bool allPacketsCollided = false;
  if (isCollision){
    for (std::map<int, Packet>::iterator it = bus.begin(); it != bus.end();){
      it->second.collided = true;
      ++it;
    }
    numberOfCollisions++;
    isCollision = false;
  }

  for (std::map<int, Packet>::iterator it = bus.begin(); it != bus.end();){
    if (it->second.collided) 
    {
      allPacketsCollided = true;
      break;
    }
    ++it;
  }

  if (bus.size() > 0)
  {
    bool allNodesSense = true;
    for (std::map<int, Packet>::iterator it = bus.begin(); it != bus.end();){
      for (int i = 0; i < it->second.timeToFirstBit.size(); i++){
        if (it->second.timeToFirstBit.at(i) > 0){
          it->second.timeToFirstBit[i] -= tick_duration;
          allNodesSense = false;
        }
        if (it->second.timeToLastBit.at(i) > 0)
        {
          it->second.timeToLastBit[i] -= tick_duration;
        }
        if (it->second.timeToLastBit.at(0) <= 0 ){
          it->second.transmittedTime = curTick;
        }
      }
      if (it->second.timeToLastBit.at(0) <= 0 && (!it->second.isReceived && !it->second.collided)){
        it->second.isReceived = true;
        totalDelay += (it->second.transmittedTime - it->second.generationTime);
        successfulPacketCount++;
      }
      if ((it->second.isReceived || it->second.collided) && allNodesSense){
        bus.erase(it++);
      }
      else{
        ++it;
      }
    }
  }
}