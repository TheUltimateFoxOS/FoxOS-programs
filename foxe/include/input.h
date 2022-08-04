#pragma once

#include <stdint.h>

void arrow_keys_sighandler(uint8_t signum);
bool listen_input();