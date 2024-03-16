
#ifndef VERSION
#define VERSION			1
#endif

#ifndef REVISION
#define REVISION		0
#endif

#ifndef SUBREVISION
#define SUBREVISION		0
#endif

#ifndef DATE
#define DATE			"27.09.2023"
#endif

#define STR_HELPER(s) #s      //stringify argument
#define STR(s) STR_HELPER(s)  //indirection to expand argument macros

#define VERS			"clib4.library " STR(VERSION) "." STR(REVISION) "." STR(SUBREVISION)
#define VSTRING			"clib4.library " STR(VERSION) "." STR(REVISION) "." STR(SUBREVISION) " (" STR(DATE) ")\r\n"
#define VERSTAG			"\0$VER: clib4.library " STR(VERSION) "." STR(REVISION) "." STR(SUBREVISION) " (" STR(DATE) ")"
