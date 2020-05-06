#pragma once

// controller states
enum class controller_state
{
    disable,
    enable,
    error
};

class controller
{
private:
    /* data */

    // state

public:
    controller(/* args */);
    ~controller();

    // enable controller
    controller_state Enable();

    // disable controller
    controller_state Disable();
};
