//
//  DeadEnds
//
//  utils.c
//
//  Created by Thomas Wetmore on 13 November 2022.
//  Last changed on 6 November 2022.
//

#include <sys/time.h>
#include "utils.h"

// Get current time in milliseconds modulo 10 seconds.
//--------------------------------------------------------------------------------------------------
double getmilliseconds(void) {
    struct timeval time;
    void* tz;
    (void) gettimeofday(&time, &tz);
    int seconds = time.tv_sec % 10;  // Seconds now an int between 0 and 9.
    int milliseconds = (int) (time.tv_usec/1000);
    return seconds + milliseconds / 1000.;
}
