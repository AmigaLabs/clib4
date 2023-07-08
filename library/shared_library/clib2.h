#ifndef _CLIB2_H
#define _CLIB2_H

#include <proto/dos.h>
#include <proto/exec.h>

#include "map.h"
#include "uuid.h"
#include "ipc_headers.h"

#define RESOURCE_NAME "clib2.resource"

struct Clib2Resource {
    struct Library          resource;       /* must be first */
    uint32                  size;           /* for struct validation only */
    struct SignalSemaphore  semaphore;      /* for list arbitration */
    struct hashmap         *children;       /* list of parent nodes */
    struct hashmap         *uxSocketsMap;
    struct _clib2          *fallbackClib;
    /* SysVIPC fields */
    int locked;
    struct
    {
        struct IPCIdKeyMap keymap;
        uint32 totshm, shmmax;
    } shmcx;
    struct
    {
        struct IPCIdKeyMap keymap;
        uint32 qsizemax;
    } msgcx;
    struct
    {
        struct IPCIdKeyMap keymap;
    } semcx;
};

struct Clib2Node {
    uint32           pid;  /* the process PID */
    uint32           pPid; /* the process Parent PID */
    char             uuid[UUID4_LEN + 1];
    /* SysVIPC fields */
    struct UndoInfo *undo;
    int32            errNo;
};

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

int32 _start(STRPTR args,
             int32 arglen,
             struct ExecBase *sysbase);

extern int _main(char *argstr,
                 int arglen,
                 int (*start_main)(int, char **),
                 void (*__EXT_CTOR_LIST__[])(void),
                 void (*__EXT_DTOR_LIST__[])(void));


#endif