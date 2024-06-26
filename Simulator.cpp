#include "Simulator.h"

Simulator::Simulator(size_t max_battery_steps, House::Matrix tiles): house(tiles), robot(house, max_battery_steps) {}
