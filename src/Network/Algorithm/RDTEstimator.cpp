#include "Network/Algorithm/RDTEstimator.hpp"
#include <math.h>

/*
  Link: https://en.wikipedia.org/wiki/Moving_average
  alpha <0,1>
*/

rdt::RTTEstimator::RTTEstimator() {
  weightedSum = weightedCount = 0.0;
  alpha = 0.25;
}

double rdt::RTTEstimator::EWMA(const double& serieValue, const double& lastEWMA){
  return serieValue + (1 - alpha) * lastEWMA;
}

uint32_t rdt::RTTEstimator::estimate(const double& currentRTT){
  weightedSum = EWMA(currentRTT, weightedSum);                    
  weightedCount = EWMA(1.0, weightedCount);                       
  return static_cast<uint32_t>(std::round(weightedSum / weightedCount));
}