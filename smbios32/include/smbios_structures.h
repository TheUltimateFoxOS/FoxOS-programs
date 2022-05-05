#pragma once

#include <smbios.h>
#include <stdio.h>

extern smbios_structure_handler_t bios_information_handler;
extern smbios_structure_handler_t system_information_handler;
extern smbios_structure_handler_t system_chassis_handler;
extern smbios_structure_handler_t processor_information_handler;