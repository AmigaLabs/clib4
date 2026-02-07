#include <proto/exec.h>
#include <proto/dos.h>
#include <string>
#include <cstring>

struct Library *SysBase, *DOSBase;
#ifndef CLIB4
struct Library *NewlibBase;
struct NewlibIFace *INewlib;
#else
struct Library *Clib4Base;
struct Clib4IFace *IClib4;
#endif
struct ExecIFace *IExec;
struct DOSIFace	*IDOS;

extern "C" {
	int _start( char *argstring, int arglen, struct ExecBase *sysbase) {
		SysBase = (struct Library *)sysbase;
		IExec = (struct ExecIFace *)sysbase->MainInterface;

#ifndef CLIB4
		NewlibBase = IExec->OpenLibrary("newlib.library", 53);
		INewlib = (struct NewlibIFace*) IExec->GetInterface(NewlibBase, "main", 1, NULL);
#else
		Clib4Base = IExec->OpenLibrary("clib4.library", 2);
		IClib4 = (struct Clib4IFace*) IExec->GetInterface(Clib4Base, "main", 1, NULL);
#endif
		DOSBase = IExec->OpenLibrary("dos.library", 53);
		IDOS = (struct DOSIFace*) IExec->GetInterface(DOSBase, "main", 1, NULL);

		IDOS->Printf("Testing operator new...\n");

		// Test 1: Simple new/delete
		int* ptr = new int(42);
		IDOS->Printf("new int: 0x%08lx value=%ld\n", ptr, *ptr);
		delete ptr;

		// Test 2: std::string (this is what crashes)
		std::string str("Hello World");
		IDOS->Printf("std::string: %s size=%ld\n", str.c_str(), str.size());

		// Test 3: std::string with constructor from buffer
		const char* html = "<html><body>test</body></html>";
		std::string html_str(html, strlen(html));
		IDOS->Printf("html string: size=%ld\n", html_str.size());

		IDOS->Printf("All tests passed!\n");

		IExec->DropInterface((struct Interface*)IDOS);
		IExec->CloseLibrary(DOSBase);

#ifndef CLIB4
        IExec->DropInterface((struct Interface*)INewlib);
        IExec->CloseLibrary(NewlibBase);
#else
        IExec->DropInterface((struct Interface*)IClib4);
        IExec->CloseLibrary(Clib4Base);
#endif

		return 0;
	}
};
