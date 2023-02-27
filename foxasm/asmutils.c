#include <asmutils.h>

int get_R_reg_size(char* str) {
    if (str[0] == '\0') {
        return 8;
    } else {
        if (str[1] != '\0') {
           return 0;
        }

        switch (str[0]) {
            case 'D':
                return 4;
            case 'W':
                return 2;
            case 'L':
                return 1;
        }
    }

    return 0;
}

cpu_register_t get_register(char* str) {
    cpu_register_t return_reg;

    switch (str[0]) {
        case 'R': {
            switch (str[1]) {
                case '8': {
                    return_reg.code = R8;
                    return_reg.size = get_R_reg_size(str + 2);
                    break;
                }
                case '9': {
                    return_reg.code = R9;
                    return_reg.size = get_R_reg_size(str + 2);
                    break;
                }
                case '1': {
                    switch (str[2]) {
                        case '0': {
                            return_reg.code = R10;
                            return_reg.size = get_R_reg_size(str + 3);
                            break;
                        }
                        case '1': {
                            return_reg.code = R11;
                            return_reg.size = get_R_reg_size(str + 3);
                            break;
                        }
                        case '2': {
                            return_reg.code = R12;
                            return_reg.size = get_R_reg_size(str + 3);
                            break;
                        }
                        case '3': {
                            return_reg.code = R13;
                            return_reg.size = get_R_reg_size(str + 3);
                            break;
                        }
                        case '4': {
                            return_reg.code = R14;
                            return_reg.size = get_R_reg_size(str + 3);
                            break;
                        }
                        case '5': {
                            return_reg.code = R15;
                            return_reg.size = get_R_reg_size(str + 3);
                            break;
                        }
                        default: {
                            return_reg.size = 0;
                            break;
                        }
                    }
                    break;
                }
                case 'A': {
                    if (str[2] == 'X' && str[3] == '\0') {
                        return_reg.code = R0;
                        return_reg.size = 8;
                    } else {
                        return_reg.size = 0;
                    }
                    break;
                }
                case 'C': {
                    if (str[2] == 'X' && str[3] == '\0') {
                        return_reg.code = R1;
                        return_reg.size = 8;
                    } else {
                        return_reg.size = 0;
                    }
                    break;
                }
                case 'D': {
                    if (str[3] != '\0') {
                        return_reg.size = 0;
                        break;
                    }

                    if (str[2] == 'X') {
                        return_reg.code = R2;
                        return_reg.size = 8;
                    } else if (str[2] == 'I') {
                        return_reg.code = R7;
                        return_reg.size = 8;
                    } else {
                        return_reg.size = 0;
                    }
                    break;
                }
                case 'B': {
                    if (str[3] != '\0') {
                        return_reg.size = 0;
                        break;
                    }

                    if (str[2] == 'X') {
                        return_reg.code = R3;
                        return_reg.size = 8;
                    } else if (str[2] == 'P') {
                        return_reg.code = R5;
                        return_reg.size = 8;
                    } else {
                        return_reg.size = 0;
                    }
                    break;
                }
                case 'S': {
                    if (str[3] != '\0') {
                        return_reg.size = 0;
                        break;
                    }
                    
                    if (str[2] == 'P') {
                        return_reg.code = R4;
                        return_reg.size = 8;
                    } else if (str[2] == 'I') {
                        return_reg.code = R6;
                        return_reg.size = 8;
                    } else {
                        return_reg.size = 0;
                    }
                    break;
                }
                default: {
                    return_reg.size = 0;
                    break;
                }
            }
            break;
        }
        case 'E': {
            switch (str[1]) {
                case 'A': {
                    if (str[2] == 'X' && str[3] == '\0') {
                        return_reg.code = R0;
                        return_reg.size = 4;
                    } else {
                        return_reg.size = 0;
                    }
                    break;
                }
                case 'C': {
                    if (str[2] == 'X' && str[3] == '\0') {
                        return_reg.code = R1;
                        return_reg.size = 4;
                    } else {
                        return_reg.size = 0;
                    }
                    break;
                }
                case 'D': {
                    if (str[3] != '\0') {
                        return_reg.size = 0;
                        break;
                    }

                    if (str[2] == 'X') {
                        return_reg.code = R2;
                        return_reg.size = 4;
                    } else if (str[2] == 'I') {
                        return_reg.code = R7;
                        return_reg.size = 4;
                    } else {
                        return_reg.size = 0;
                    }
                    break;
                }
                case 'B': {
                    if (str[3] != '\0') {
                        return_reg.size = 0;
                        break;
                    }

                    if (str[2] == 'X') {
                        return_reg.code = R3;
                        return_reg.size = 4;
                    } else if (str[2] == 'P') {
                        return_reg.code = R5;
                        return_reg.size = 4;
                    } else {
                        return_reg.size = 0;
                    }
                    break;
                }
                case 'S': {
                    if (str[3] != '\0') {
                        return_reg.size = 0;
                        break;
                    }

                    if (str[2] == 'P') {
                        return_reg.code = R4;
                        return_reg.size = 4;
                    } else if (str[2] == 'I') {
                        return_reg.code = R6;
                        return_reg.size = 4;
                    } else {
                        return_reg.size = 0;
                    }
                    break;
                }
                default: {
                    return_reg.size = 0;
                    break;
                }
            }
            break;
        }
        case 'A': {
            if (str[2] != '\0') {
                return_reg.size = 0;
                break;
            }

            switch (str[1]) {
                case 'L': {
                    return_reg.code = R0;
                    return_reg.size = 1;
                    break;
                }
                case 'H': {
                    return_reg.code = R4;
                    return_reg.size = 1;
                    break;
                }
                case 'X': {
                    return_reg.code = R0;
                    return_reg.size = 2;
                    break;
                }
                default: {
                    return_reg.size = 0;
                    break;
                }
            }
            break;
        }
        case 'C': {
            if (str[2] != '\0') {
                return_reg.size = 0;
                break;
            }

            switch (str[1]) {
                case 'L': {
                    return_reg.code = R1;
                    return_reg.size = 1;
                    break;
                }
                case 'H': {
                    return_reg.code = R5;
                    return_reg.size = 1;
                    break;
                }
                case 'X': {
                    return_reg.code = R1;
                    return_reg.size = 2;
                    break;
                }
                default: {
                    return_reg.size = 0;
                    break;
                }
            }
            break;
        }
        case 'D': {
            if (str[2] != '\0') {
                return_reg.size = 0;
                break;
            }

            switch (str[1]) {
                case 'L': {
                    return_reg.code = R2;
                    return_reg.size = 1;
                    break;
                }
                case 'H': {
                    return_reg.code = R6;
                    return_reg.size = 1;
                    break;
                }
                case 'X': {
                    return_reg.code = R2;
                    return_reg.size = 2;
                    break;
                }
                case 'I': {
                    return_reg.code = R7;
                    return_reg.size = 2;
                    break;
                }
                default: {
                    return_reg.size = 0;
                    break;
                }
            }
            break;
        }
        case 'B': {
            if (str[2] != '\0') {
                return_reg.size = 0;
                break;
            }

            switch (str[1]) {
                case 'L': {
                    return_reg.code = R3;
                    return_reg.size = 1;
                    break;
                }
                case 'H': {
                    return_reg.code = R7;
                    return_reg.size = 1;
                    break;
                }
                case 'X': {
                    return_reg.code = R3;
                    return_reg.size = 2;
                    break;
                }
                case 'P': {
                    return_reg.code = R5;
                    return_reg.size = 2;
                    break;
                }
                default: {
                    return_reg.size = 0;
                    break;
                }
            }
            break;
        }
        case 'S': {
            if (str[2] != '\0') {
                return_reg.size = 0;
                break;
            }

            switch (str[1]) {
                case 'P': {
                    return_reg.code = R4;
                    return_reg.size = 2;
                    break;
                }
                case 'I': {
                    return_reg.code = R6;
                    return_reg.size = 2;
                    break;
                }
                default: {
                    return_reg.size = 0;
                    break;
                }
            }
            break;
        }
        default: {
            return_reg.size = 0;
            break;
        }
    }

    return return_reg;
}

uint8_t get_two_register_magic(cpu_register_t* dest, cpu_register_t* src) {
    return (8 * ((uint8_t) src->code)) + ((uint8_t) dest->code);
}

bool is_digits(char* str) {
    if (str[0] == '-') {
        str++;
    }
    while (*str) {
        if (*str < '0' || *str > '9') {
            return false;
        }
        str++;
    }
    return true;
}

bool is_hex(char* str) {
    while (*str) {
        if ((*str < '0' || *str > '9') && (*str < 'A' || *str > 'F')) {
            return false;
        }
        str++;
    }
    return true;
}

bool is_binary(char* str) {
    while (*str) {
        if (*str != '0' && *str != '1') {
            return false;
        }
        str++;
    }
    return true;
}

bool is_letters(char* str) {
    while (*str) {
        if ((*str < 'A' || *str > 'Z')) {
            return false;
        }
        str++;
    }
    return true;
}

bool is_alphanumeric(char* str) {
    while (*str) {
        if ((*str < 'A' || *str > 'Z') && (*str < '0' || *str > '9')) {
            return false;
        }
        str++;
    }
    return true;
}

void str_to_upper(char* str) {
    while (*str) {
        if (*str >= 'a' && *str <= 'z') {
            *str -= 32;
        }
        str++;
    }
}

uint64_t hex_str_to_num(char* str) {
    uint64_t result = 0;
    while (*str) {
        result *= 16;
        if (*str >= '0' && *str <= '9') {
            result += *str - '0';
        } else if (*str >= 'A' && *str <= 'F') {
            result += *str - 'A' + 10;
        }
        str++;
    }
    return result;
}

uint64_t binary_str_to_num(char* str) {
    uint64_t result = 0;
    while (*str) {
        result *= 2;
        result += *str - '0';
        str++;
    }
    return result;
}

uint64_t str_to_num(char* str) {
    uint64_t result = 0;
    while (*str) {
        result *= 10;
        result += *str - '0';
        str++;
    }
    return result;
}

char* find_string_end(char* str, char string_delimiter) {
    bool escaped = false;
    while (*str) {
        if (*str == string_delimiter && !escaped) {
            return str;
        }
        if (*str == '\\') {
            escaped = !escaped;
        } else {
            escaped = false;
        }
        str++;
    }

    return 0;
}
