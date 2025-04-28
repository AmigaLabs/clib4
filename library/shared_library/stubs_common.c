#include <proto/exec.h>
#include <dos/dosextens.h>
#include <dos.h>
#include <proto/dos.h>

#include "interface.h"

asm(".section  \".text\"             \n\
       .align 8                          \n\
       .globl __Clib4Call                \n\
       .type  __Clib4Call, @function     \n\
__Clib4Call:                          \n\
        lis   %r11, IClib4@ha            \n\
        lwz   %r0, IClib4@l(%r11)        \n\
        lwzx  %r11, %r12, %r0            \n\
        mtctr %r11                       \n\
        bctr                             \n\
       .size   __Clib4Call, .-__Clib4Call");

Clib4Call(__getClib4, 80);
//Clib4Call(__getGlobalClib4, 84);
Clib4Call(__translate_amiga_to_unix_path_name, 88);
Clib4Call(__translate_unix_to_amiga_path_name, 92);
//Clib4Call(__restore_path_name, 96);
Clib4Call(__print_termination_message, 100);
Clib4Call(__get_default_file, 104);
Clib4Call(unixPathsEnabled, 108);
