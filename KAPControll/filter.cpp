/*
 
 */

#include "filter.h"

/*
 * Simple complementary filter
 * 
 * Sources:
 * http://blog.tkjelectronics.dk/2011/06/guide-gyro-and-accelerometer-kalman-filtering-with-the-arduino/
 * http://robottini.altervista.org/tag/complementary-filter
 * http://arduino.cc/forum/index.php?topic=58048.0
 *
 */

void Complementary::setTau(float in) {
    tau = in;
}


float Complementary::filter(float speed, float dt, float angle) {
    /*
     * Complementary Filter
     * 
     * Takes in angle speed (°/s), intervall (s), angle (°)
     * Returns angle (°) 
     */
    angleSum = tau*(angleSum + speed*dt) + (1-tau)*angle;
    return angleSum;
}
