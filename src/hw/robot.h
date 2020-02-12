#pragma once

#include "maxon.h"
#include "nmt.h"

class robot
{
private:
    /* data */

    // maxon motors
    maxon upwheel_, upclaw_, pulley1_, pulley2_, downclaw1_, downclaw2_;
    nmt nmt_;

public:
    robot(nmt nmt,maxon upwheel, maxon upclaw, maxon pulley1, maxon pulley2, maxon downclaw1, maxon downclaw2);
    ~robot();
};
