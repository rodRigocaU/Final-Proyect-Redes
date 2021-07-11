#include "Network/Algorithm/RDTEstimator.hpp"
#include <algorithm>
#include <math.h>

/*
  Link: https://en.wikipedia.org/wiki/Moving_average
  alpha [0.8, 0.9]
*/

rdt::RTTEstimator::RTTEstimator() {
  lastEstimated = 0.0;
  alpha = 0.85;
}

double rdt::RTTEstimator::EWMA(const double& sample, const double& lastEWMA){
  return alpha * lastEWMA + (1 - alpha) * sample;
}

int32_t rdt::RTTEstimator::estimate(const double& currentRTT){
  lastEstimated = EWMA(currentRTT, lastEstimated);
  return std::max(static_cast<int32_t>(std::round(lastEstimated) * 2), TIMEOUT_LOWERBOUND);
}