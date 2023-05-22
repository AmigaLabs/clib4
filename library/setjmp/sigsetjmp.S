/* non altivec (old) version of setjmp for PowerPC.
   Copyright (C) 1995-2023 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#define __NO_VMX__
#undef __sigsetjmp_symbol
#undef __sigjmp_save_symbol
#undef JB_SIZE
#define __sigsetjmp_symbol __sigsetjmp
#define __sigjmp_save_symbol __sigjmp_save
#include "sigsetjmp-common.S"