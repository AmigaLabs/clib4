#ifndef VERSION
#define VERSION			1
#endif

#ifndef REVISION
#define REVISION		216
#endif

#ifndef SUBREVISION
#define SUBREVISION		0
#endif

#ifndef DATE
#define DATE			"08.02.2021"
#endif

#define STR_HELPER(s) #s      //stringify argument
#define STR(s) STR_HELPER(s)  //indirection to expand argument macros

#define VERS			"amiga.lib " STR(VERSION) "." STR(REVISION)
#define VSTRING			"amiga.lib " STR(VERSION) "." STR(REVISION) " (" STR(DATE) ")\r\n"
#define VERSTAG			"\0$VER: amiga.lib " STR(VERSION) "." STR(REVISION) " (" STR(DATE) ")"
