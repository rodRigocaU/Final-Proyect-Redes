#ifndef RDT_ESTIMATOR_HPP_
#define RDT_ESTIMATOR_HPP_

#include <iostream>

#define DEFAULT_RTT_VALUE   200
#define MIN_RTT_VALUE       15

namespace rdt{
  
  class RTTEstimator{
  private:
    double lastEstimated, alpha;
    double EWMA(const double& serieValue, const double& lastEWMA);
  public:
    RTTEstimator();
    int32_t estimate(const double& currentRTT = DEFAULT_RTT_VALUE);
  };

}


#endif//RDT_ESTIMATOR_HPP_