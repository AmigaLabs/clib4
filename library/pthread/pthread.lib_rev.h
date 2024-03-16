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
#define DATE			"18.07.2022"
#endif

#define STR_HELPER(s) #s      //stringify argument
#define STR(s) STR_HELPER(s)  //indirection to expand argument macros

#define VERS			"pthread.lib " STR(VERSION) "." STR(REVISION)
#define VSTRING			"pthread.lib " STR(VERSION) "." STR(REVISION) " (" STR(DATE) ")\r\n"
#define VERSTAG			"\0$VER: pthread.lib " STR(VERSION) "." STR(REVISION) " (" STR(DATE) ")"

