/* Definitions of target machine for GNU compiler, for AmigaOS.
   Copyright (C) 1997, 2003, 2005 Free Software Foundation, Inc.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 2, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING.  If not, write to the
   Free Software Foundation, 59 Temple Place - Suite 330, Boston,
   MA 02111-1307, USA.  */


#undef MD_EXEC_PREFIX
#undef MD_STARTFILE_PREFIX

/* Make CPU default to 604e. FIXME: Make this 750 later */
#undef PROCESSOR_DEFAULT
#define PROCESSOR_DEFAULT PROCESSOR_PPC604e

#undef DEFAULT_ABI
#define DEFAULT_ABI ABI_V4

#undef ASM_CPU_SPEC
#define ASM_CPU_SPEC \
"%{!mcpu*: \
  %{mpower: %{!mpower2: -mpwr}} \
  %{mpower2: -mpwrx} \
  %{mpowerpc64*: -mppc64} \
  %{!mpowerpc64*: %{mpowerpc*: -mppc}} \
  %{mno-power: %{!mpowerpc*: -mcom}} \
  %{!mno-power: %{!mpower*: %(asm_default)}}} \
%{mcpu=common: -mcom} \
%{mcpu=power: -mpwr} \
%{mcpu=power2: -mpwrx} \
%{mcpu=power3: -mppc64} \
%{mcpu=power4: -mpower4} \
%{mcpu=power5: -mpower4} \
%{mcpu=powerpc: -mppc} \
%{mcpu=rios: -mpwr} \
%{mcpu=rios1: -mpwr} \
%{mcpu=rios2: -mpwrx} \
%{mcpu=rsc: -mpwr} \
%{mcpu=rsc1: -mpwr} \
%{mcpu=rs64a: -mppc64} \
%{mcpu=401: -mppc} \
%{mcpu=403: -m403} \
%{mcpu=405: -m405} \
%{mcpu=405fp: -m405} \
%{mcpu=440: -m440} \
%{mcpu=440fp: -m440} \
%{mcpu=505: -mppc} \
%{mcpu=601: -m601} \
%{mcpu=602: -mppc} \
%{mcpu=603: -mppc} \
%{mcpu=603e: -mppc} \
%{mcpu=ec603e: -mppc} \
%{mcpu=604: -mppc} \
%{mcpu=604e: -mppc} \
%{mcpu=620: -mppc64} \
%{mcpu=630: -mppc64} \
%{mcpu=740: -mppc} \
%{mcpu=750: -mppc} \
%{mcpu=G3: -mppc} \
%{mcpu=7400: -mppc -maltivec} \
%{mcpu=7450: -mppc -maltivec} \
%{mcpu=G4: -mppc -maltivec} \
%{mcpu=801: -mppc} \
%{mcpu=821: -mppc} \
%{mcpu=823: -mppc} \
%{mcpu=860: -mppc} \
%{mcpu=970: -mpower4 -maltivec} \
%{mcpu=G5: -mpower4 -maltivec} \
%{mcpu=8540: -me500} \
%{maltivec: -maltivec}"

#undef CC1_SPEC
#define        CC1_SPEC "%{G*} %(cc1_cpu)" \
"%{g: %{!fno-eliminate-unused-debug-symbols: -feliminate-unused-debug-symbols}} \
%{g1: %{!fno-eliminate-unused-debug-symbols: -feliminate-unused-debug-symbols}} \
%{msdata: -msdata=default} \
%{mno-sdata: -msdata=none} \
%{!mbss-plt: %{!msecure-plt: %(cc1_secure_plt_default)}} \
%{profile: -p} \
%{faltivec:-maltivec -include altivec.h} %{fno-altivec:-mno-altivec} \
%<faltivec %<fno-altivec \
%{vec:-maltivec -include altivec.h} %{fno-vec:-mno-altivec} \
%<fvec %<fno-vec "

#define IS_MCRT(MCRTNAME) \
  (strcmp(amigaos_crt, MCRTNAME) == 0)

/* Make most of the definitions from other compilers available */
#undef TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS()                \
  do                                            \
    {                                           \
      builtin_define_std ("PPC");		\
      builtin_define_std ("powerpc");		\
      builtin_assert ("cpu=powerpc");		\
      builtin_assert ("machine=powerpc");	\
      builtin_define_std ("AMIGA");		\
      builtin_define_std ("AMIGAOS");		\
      builtin_define_std ("AMIGAOS4");		\
      builtin_define_std ("amiga");		\
      builtin_define_std ("amigaos");		\
      builtin_define_std ("amigaos4");		\
      builtin_assert ("system=amigaos");	\
      if (!amigaos_crt)			\
        {					\
          error ("no CRT specified");		\
        }					\
      else if (IS_MCRT("clib2"))                \
        {					\
          builtin_define_std ("CLIB2");		\
        }					\
      else if (IS_MCRT("ixemul"))		\
        {					\
          builtin_define_std ("ixemul");	\
          builtin_define_std ("IXEMUL");	\
        }					\
      else if (IS_MCRT("libnix"))		\
        {					\
          builtin_define_std ("libnix");	\
          builtin_define_std ("LIBNIX");	\
        }					\
      else if (IS_MCRT("newlib"))		\
        {					\
          builtin_define_std ("NEWLIB");	\
        }					\
      TARGET_OS_SYSV_CPP_BUILTINS ();		\
    }                                           \
  while (0)

#undef CPP_SPEC
#define CPP_SPEC "%{posix: -D_POSIX_SOURCE} %(cpp_os_default)"

/*#define STANDARD_INCLUDE_DIR "/GCC/include"*/
/*#undef SYSTEM_INCLUDE_DIR *//* So that the include path order is the same in native and cross compilers */
#undef LOCAL_INCLUDE_DIR

#ifndef CROSS_DIRECTORY_STRUCTURE
#define BASE_GCC_SPEC "/GCC/"
#define BASE_SDK_SPEC "/SDK/"
#else
#define BASE_GCC_SPEC EXEC_PREFIX
#define BASE_SDK_SPEC EXEC_PREFIX "ppc-amigaos/SDK/"
#endif

#define LIB_SUBDIR_TYPE_SPEC "\
%{mbaserel:/baserel; msdata|msdata=default|msdata=sysv:/small-data}\
%{msoft-float:/soft-float}"

/* default linker specs */
#undef REAL_LIBGCC_SPEC
#define REAL_LIBGCC_SPEC "\
%{static|static-libgcc: %{!use-dynld: -lgcc -lgcc_eh} %{use-dynld: -lgcc} }%{!static:%{!static-libgcc:%{!shared:%{!shared-libgcc: %{!use-dynld: -lgcc -lgcc_eh} %{use-dynld: -lgcc}}%{shared-libgcc:-lgcc}}%{shared:%{shared-libgcc:-lgcc}%{!shared-libgcc:-lgcc}}}}"


/* make newlib the default */
#if 1
#define CPP_AMIGA_DEFAULT_SPEC "%{mcrt=default|!mcrt=*:%<mcrt=default -mcrt=newlib} %(cpp_newlib)"
#define LINK_AMIGA_DEFAULT_SPEC "%(link_newlib)"
#define STARTFILE_AMIGA_DEFAULT_SPEC "%(startfile_newlib)"
#define ENDFILE_AMIGA_DEFAULT_SPEC "%(endfile_newlib)"
#undef MULTILIB_DEFAULTS
#define MULTILIB_DEFAULTS {"mcrt=newlib"}
#else
/* make clib2 the default */
#define CPP_AMIGA_DEFAULT_SPEC "%{mcrt=default|!mcrt=*:%<mcrt=default -mcrt=clib2} %(cpp_clib2)"
#define LINK_AMIGA_DEFAULT_SPEC "%(link_clib2)"
#define STARTFILE_AMIGA_DEFAULT_SPEC "%(startfile_clib2)"
#define ENDFILE_AMIGA_DEFAULT_SPEC "%(endfile_clib2)"
#undef MULTILIB_DEFAULTS
#define MULTILIB_DEFAULTS {"mcrt=clib2"}
#endif


/* For specifying the include system paths, we generally use -idirafter so the include
 * paths are added at the end of the gcc default include paths. This is required for
 * fixincludes and libstdc++ to work properly
 */


/* clib2 */

#define CPP_CLIB2_SPEC "\
-idirafter %(base_sdk)clib2/include -idirafter %(base_sdk)local/clib2/include"

#define LIB_SUBDIR_CLIB2_SPEC "lib%(lib_subdir_type)"

#define LINK_CLIB2_SPEC "\
-L%(base_sdk)clib2/%(lib_subdir_clib2) \
-L%(base_gcc)lib/gcc/ppc-amigaos/%(version)/clib2/%(lib_subdir_clib2) \
-L%(base_sdk)local/clib2/%(lib_subdir_clib2)"

#define STARTFILE_CLIB2_SPEC "\
%{shared: %(base_sdk)clib2/%(lib_subdir_clib2)/shcrtbegin.o} %{!shared: %(base_sdk)clib2/%(lib_subdir_clib2)/crtbegin.o} %{!shared: %(base_sdk)clib2/%(lib_subdir_clib2)/crt0.o}"

#define ENDFILE_CLIB2_SPEC "\
%{shared: %(base_sdk)clib2/%(lib_subdir_clib2)/shcrtend.o} %{!shared: %(base_sdk)clib2/%(lib_subdir_clib2)/crtend.o}"

/* ixemul */

#define CPP_IXEMUL_SPEC "\
-idirafter %(base_sdk)ixemul/include -idirafter %(base_sdk)local/ixemul/include"

#define LIB_SUBDIR_IXEMUL_SPEC "lib%(lib_subdir_type)"

#define LINK_IXEMUL_SPEC "\
-L%(base_sdk)ixemul/%(lib_subdir_ixemul) \
-L%(base_gcc)lib/gcc/ppc-amigaos/%(version)/ixemul/%(lib_subdir_ixemul) \
-L%(base_sdk)local/ixemul/%(lib_subdir_ixemul)"

/* ixemul startfile should work for all library flavours */
#define STARTFILE_IXEMUL_SPEC "%(base_sdk)ixemul/%(lib_subdir_ixemul)/crtbegin.o"
#define ENDFILE_IXEMUL_SPEC "%(base_sdk)ixemul/%(lib_subdir_ixemul)/crtend.o"

/* libnix */

#define CPP_LIBNIX_SPEC "\
-idirafter %(base_sdk)libnix/include -idirafter %(base_sdk)local/libnix/include"

#define LIB_SUBDIR_LIBNIX_SPEC "lib%(lib_subdir_type)"

#define LINK_LIBNIX_SPEC "\
-L%(base_sdk)libnix/%(lib_subdir_libnix) \
-L%(base_gcc)lib/gcc/ppc-amigaos/%(version)/libnix/%(lib_subdir_libnix) \
-L%(base_sdk)local/libnix/%(lib_subdir_libnix)"

#define STARTFILE_LIBNIX_SPEC "%(base_sdk)libnix/%(lib_subdir_libnix)/crtbegin.o"
#define ENDFILE_LIBNIX_SPEC "%(base_sdk)libnix/%(lib_subdir_libnix)/crtend.o"

/* newlib */

#define CPP_NEWLIB_SPEC "\
-idirafter %(base_sdk)newlib/include -idirafter %(base_sdk)local/newlib/include"

#define LIB_SUBDIR_NEWLIB_SPEC "lib%(lib_subdir_type)"

#define LINK_NEWLIB_SPEC "\
-L%(base_sdk)newlib/%(lib_subdir_newlib) \
-L%(base_gcc)lib/gcc/ppc-amigaos/%(version)/newlib/%(lib_subdir_newlib) \
-L%(base_sdk)local/newlib/%(lib_subdir_newlib)"

/* newlib startfile should work for all library flavours */
#define STARTFILE_NEWLIB_SPEC "\
%{shared: %(base_sdk)newlib/%(lib_subdir_newlib)/shcrtbegin.o} %{!shared: %(base_sdk)newlib/%(lib_subdir_newlib)/crtbegin.o}"

#define ENDFILE_NEWLIB_SPEC "\
%{shared: %(base_sdk)newlib/%(lib_subdir_newlib)/shcrtend.o} %{!shared: %(base_sdk)newlib/%(lib_subdir_newlib)/crtend.o}"

/* End clib specific */

#undef CPP_OS_DEFAULT_SPEC
#define CPP_OS_DEFAULT_SPEC "\
%{mcrt=clib2: %(cpp_clib2); \
mcrt=ixemul: %(cpp_ixemul); \
mcrt=libnix: %(cpp_libnix); \
mcrt=newlib: %(cpp_newlib); \
mcrt=default|!mcrt=*: %{mcrt=default|!nostdinc: %(cpp_amiga_default)}; \
: %eInvalid C runtime library} \
%{!nostdinc: -idirafter %(base_sdk)include/include_h -idirafter %(base_sdk)include/netinclude -idirafter %(base_sdk)local/common/include} \
%{mbaserel: %{msdata|msdata=default|msdata=sysv: %e-mbaserel and -msdata options are incompatible}} \
%{newlib: %e-newlib is obsolete, use -mcrt=newlib instead}"

#undef LINK_SPEC
#define LINK_SPEC "\
--defsym __amigaos4__=1 \
%{!shared: %{!use-dynld: -Bstatic}} \
-q -d %{h*} %{v:-V} %{G*} \
%{Wl,*:%*} %{YP,*} %{R*} \
%{Qy:} %{!Qn:-Qy} \
%(link_thread) %(link_shlib) %(link_text) \
%{mbaserel: %{msdata|msdata=default|msdata=sysv: %e-mbaserel and -msdata options are incompatible}} \
%{mcrt=clib2: %(link_clib2); \
mcrt=ixemul: %(link_ixemul); \
mcrt=libnix: %(link_libnix); \
mcrt=newlib: %(link_newlib); \
mcrt=default|!mcrt=*: %(link_amiga_default); \
: %eInvalid C runtime library} \
-L%(base_sdk)local/common/lib%(lib_subdir_type) \
%{newlib: %e-newlib is obsolete, use -mcrt=newlib instead}"

/* FIXME: LINK_TEXT has been made empty now. Could we get rid of it? */
#if 0
#define LINK_TEXT "\
%{use-dynld: -Ttext=0x100000} %{!use-dynld: %{shared: -Ttext=0x100000} %{!shared: %{!Wl,-T*: %{!T*:-Ttext=0}}}}"
#else
#define LINK_TEXT ""
#endif

#define LINK_SHLIB "\
%{shared:-shared -dy --defsym __dynld_version__=1} %{!shared: %{static:-static}} %{use-dynld: -dy}"

#define LINK_THREAD "\
%s%{athread=native:gthr-amigaos-native.o;athread=single:gthr-amigaos-single.o;athread=pthread:gthr-amigaos-pthread.o}"

#undef STARTFILE_SPEC
#define STARTFILE_SPEC "\
%{mcrt=clib2: %(startfile_clib2); \
mcrt=ixemul: %(startfile_ixemul); \
mcrt=libnix: %(startfile_libnix); \
mcrt=newlib: %(startfile_newlib); \
mcrt=default|!mcrt=*: %(startfile_amiga_default); \
: %eInvalid C runtime library}"

#undef ENDFILE_SPEC
#define ENDFILE_SPEC "\
%{mcrt=clib2: %(endfile_clib2); \
mcrt=ixemul: %(endfile_ixemul); \
mcrt=libnix: %(endfile_libnix); \
mcrt=newlib: %(endfile_newlib); \
mcrt=default|!mcrt=*: %(endfile_amiga_default); \
: %eInvalid C runtime library}"

#undef LIB_SPEC
#define LIB_SPEC "\
--start-group -lc --end-group"

#undef TARGET_DEFAULT
#define TARGET_DEFAULT 0

#undef SUBTARGET_EXTRA_SPECS
#define SUBTARGET_EXTRA_SPECS \
  {"base_gcc", BASE_GCC_SPEC}, \
  {"base_sdk", BASE_SDK_SPEC}, \
  {"cpp_os_default", CPP_OS_DEFAULT_SPEC}, \
  {"lib_subdir_type", LIB_SUBDIR_TYPE_SPEC}, \
  /* default C runtime */ \
  {"cpp_amiga_default", CPP_AMIGA_DEFAULT_SPEC}, \
  {"link_amiga_default", LINK_AMIGA_DEFAULT_SPEC}, \
  {"startfile_amiga_default", STARTFILE_AMIGA_DEFAULT_SPEC}, \
  {"endfile_amiga_default", ENDFILE_AMIGA_DEFAULT_SPEC}, \
  /* clib2 */ \
  {"cpp_clib2", CPP_CLIB2_SPEC}, \
  {"lib_subdir_clib2", LIB_SUBDIR_CLIB2_SPEC}, \
  {"link_clib2", LINK_CLIB2_SPEC}, \
  {"startfile_clib2", STARTFILE_CLIB2_SPEC}, \
  {"endfile_clib2", ENDFILE_CLIB2_SPEC}, \
  /* ixemul */ \
  {"cpp_ixemul", CPP_IXEMUL_SPEC}, \
  {"lib_subdir_ixemul", LIB_SUBDIR_IXEMUL_SPEC}, \
  {"link_ixemul", LINK_IXEMUL_SPEC}, \
  {"startfile_ixemul", STARTFILE_IXEMUL_SPEC}, \
  {"endfile_ixemul", ENDFILE_IXEMUL_SPEC}, \
  /* libnix */ \
  {"cpp_libnix", CPP_LIBNIX_SPEC}, \
  {"lib_subdir_libnix", LIB_SUBDIR_LIBNIX_SPEC}, \
  {"link_libnix", LINK_LIBNIX_SPEC}, \
  {"startfile_libnix", STARTFILE_LIBNIX_SPEC}, \
  {"endfile_libnix", ENDFILE_LIBNIX_SPEC}, \
  /* newlib */ \
  {"cpp_newlib", CPP_NEWLIB_SPEC}, \
  {"lib_subdir_newlib", LIB_SUBDIR_NEWLIB_SPEC}, \
  {"link_newlib", LINK_NEWLIB_SPEC}, \
  {"startfile_newlib", STARTFILE_NEWLIB_SPEC}, \
  {"endfile_newlib", ENDFILE_NEWLIB_SPEC}, \
  /* used in link spec  */ \
  {"link_text", LINK_TEXT}, \
  {"link_shlib", LINK_SHLIB}, \
  {"link_thread", LINK_THREAD},

#undef DEFAULT_VTABLE_THUNKS
#ifndef USE_GNULIBC_1
#define DEFAULT_VTABLE_THUNKS 1
#endif

#undef JUMP_TABLES_IN_TEXT_SECTION
#define JUMP_TABLES_IN_TEXT_SECTION 0

/* Used as cookie for linear vararg passing */
#define CALL_LINEARVARARGS      0x10000000

#define SUB3TARGET_OVERRIDE_OPTIONS  \
do                                   \
{                                    \
  if (TARGET_ALTIVEC)                \
  {                                  \
    rs6000_altivec_abi = 1;          \
    TARGET_ALTIVEC_VRSAVE = 1;       \
  }                                  \
} while(0)

#undef SUBTARGET_EXPAND_BUILTIN
#define SUBTARGET_EXPAND_BUILTIN(EXP, TARGET, SUBTARGET, MODE, IGNORE, SUCCESS) \
  amigaos_expand_builtin (EXP, TARGET, SUBTARGET, MODE, IGNORE, SUCCESS)

#undef SUBTARGET_INIT_BUILTINS
#define SUBTARGET_INIT_BUILTINS \
  amigaos_init_builtins ()

/* AmigaOS specific attribute */
/* { name, min_len, max_len, decl_req, type_req, fn_type_req,
       affects_type_identity, handler, exclude } */
#define SUBTARGET_ATTRIBUTE_TABLE \
  { "linearvarargs", 0, 0, false, true,  true, false, amigaos_handle_linearvarargs_attribute, NULL}, \
  { "checktags", 0, 0, false, true, true, false, amigaos_handle_checktags_attribute, NULL}, \
  { "baserel_restore", 0, 0, false, true, true, false, amigaos_handle_baserel_restore_attribute, NULL }, \
  { "force_no_baserel", 0, 0, true, false, false, false, amigaos_handle_force_no_baserel_attribute, NULL }, \
  { "check68kfuncptr", 0, 0, false, true, true, false, amigaos_handle_check68kfuncptr_attribute, NULL }

/* Overrides */

/* We don't want to use section anchors in baserel mode so we have to override
 * the decision functions
 */
#undef TARGET_USE_ANCHORS_FOR_SYMBOL_P
#define TARGET_USE_ANCHORS_FOR_SYMBOL_P amigaos_use_anchors_for_symbol_p

/*
#undef INIT_CUMULATIVE_ARGS
#define INIT_CUMULATIVE_ARGS(CUM, FNTYPE, LIBNAME, INDIRECT, N_NAMED_ARGS) \
  amigaos_init_cumulative_args (&CUM, FNTYPE, LIBNAME, FALSE, N_NAMED_ARGS)

#undef INIT_CUMULATIVE_INCOMING_ARGS
#define INIT_CUMULATIVE_INCOMING_ARGS(CUM, FNTYPE, LIBNAME) \
  amigaos_init_cumulative_args (&CUM, FNTYPE, LIBNAME, TRUE, 1000)

#undef FUNCTION_ARG_ADVANCE
#define FUNCTION_ARG_ADVANCE(CUM, MODE, TYPE, NAMED)    \
  amigaos_function_arg_advance (&CUM, MODE, TYPE, NAMED)

#undef FUNCTION_ARG
#define FUNCTION_ARG(CUM, MODE, TYPE, NAMED) \
  amigaos_function_arg (&CUM, MODE, TYPE, NAMED)
*/
#undef EXPAND_BUILTIN_VA_START
#define EXPAND_BUILTIN_VA_START(VALIST, NEXTARG) \
  amigaos_expand_builtin_va_start (VALIST, NEXTARG)

/*
//#undef SLOW_UNALIGNED_ACCESS
//#define SLOW_UNALIGNED_ACCESS(MODE, ALIGN)				\
//  (STRICT_ALIGNMENT							\
//   || (((MODE) == SFmode) && (ALIGN) < 32)				\
//   || (((MODE) == DFmode || (MODE) == TFmode || (MODE) == DImode)	\
//       && (ALIGN) < 64))
*/

/* This target uses the amigaos.opt file.  */
#define TARGET_USES_AMIGAOS_OPT 1
