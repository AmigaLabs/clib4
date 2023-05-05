#ifndef _CLIB2_H
#define _CLIB2_H

#ifndef PROTO_DOS_H
#include <proto/dos.h>
#endif /* PROTO_DOS_H */

struct Clib2Base {
    struct Library libNode;
    uint16 pad;
    BPTR SegList;
};

int libReserved(void);
uint32 libRelease(struct LibraryManagerInterface *Self);
uint32 libObtain(struct LibraryManagerInterface *Self);
struct Clib2Base *libOpen(struct LibraryManagerInterface *Self, uint32 version);
struct Clib2Base *libInit(struct Clib2Base *libBase, BPTR seglist, struct ExecIFace *const iexec);
BPTR libExpunge(struct LibraryManagerInterface *Self);

static void _start_ctors(void (*__CTOR_LIST__[])(void));
static void _start_dtors(void (*__DTOR_LIST__[])(void));
static void closeLibraries();

int library_start(char *argstr,
                  int arglen,
                  int (*start_main)(int, char **),
                  void (*__EXT_CTOR_LIST__[])(void),
                  void (*__EXT_DTOR_LIST__[])(void));

int32 _start(char *args,
             int arglen,
             struct ExecBase *sysbase);

extern int _main(char *argstr,
                 int arglen,
                 int (*start_main)(int, char **),
                 void (*__CTOR_LIST__[])(void),
                 void (*__DTOR_LIST__[])(void),
                 void (*__EXT_CTOR_LIST__[])(void),
                 void (*__EXT_DTOR_LIST__[])(void));


#endif