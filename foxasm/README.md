# Code layout

Move the the data from a specified address to a register (or vice versa)
```asm
mov rax, 0x200000
mov 0x200000, rax
```

Move an immediate value to a register
```asm
mov rax, #0xDEADBEEF
mov rax, #123
```

Move the data from a label to a register (or vice versa)
```asm
mov rax, $label
mov $label, rax

label:
    .db 0xDEADBEEF
```

# Types

## Numbers
```asm
#0x10
#0b1010
#123
```

## Registers
```asm
rax
rbx
...
```

## Labels
```asm
$label
```

## Addresses
```asm
0x200000
0b101001010
28924
```