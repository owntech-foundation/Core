
#ifndef CAN_H_
#define CAN_H_

#include <zephyr.h>

#define CAN_STANDBY_DEVICE DT_CHILD(DT_PATH(can_standby_switch), can_standby_gpio_pin)


void enable_can();


#endif // CAN_H_
