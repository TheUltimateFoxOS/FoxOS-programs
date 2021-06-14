#pragma once
#pragma once
#include <extern.h>

#define SYS_ENV 5

#define ENV_ARGV 0
#define ENV_ENVP 1

EXPOSEC char** env(int mode);