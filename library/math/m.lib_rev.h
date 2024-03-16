#ifndef VERSION
#define VERSION			1
#endif

#ifndef REVISION
#define REVISION		300
#endif

#ifndef SUBREVISION
#define SUBREVISION		0
#endif

#ifndef DATE
#define DATE			"14.03.2022"
#endif

#define STR_HELPER(s) #s      //stringify argument
#define STR(s) STR_HELPER(s)  //indirection to expand argument macros

#define VERS			"m.lib " STR(VERSION) "." STR(REVISION)
#define VSTRING			"m.lib " STR(VERSION) "." STR(REVISION) " (" STR(DATE) ")\r\n"
#define VERSTAG			"\0$VER: m.lib " STR(VERSION) "." STR(REVISION) " (" STR(DATE) ")"

