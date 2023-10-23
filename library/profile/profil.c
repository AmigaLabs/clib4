/*
 * $Id: profile_profil.c,v 1.1 2023-10-20 10:08:32 clib4devs Exp $
*/

#include <proto/exec.h>
#include <exec/interrupts.h>
#include <interfaces/performancemonitor.h>
#include <resources/performancemonitor.h>
#include <unistd.h>

#include "gmon.h"

#define COUNTER 1

static struct Interrupt CounterInt;
static struct PerformanceMonitorIFace *IPM;

static struct IntData {
    struct PerformanceMonitorIFace *IPM;
    uint16 *Buffer;
    uint32 BufferSize;
    uint32 Offset;
    uint32 Scale;
    uint32 CounterStart;
} ProfileData;

uint32 GetCounterStart(void);
uint32 CounterIntFn(struct ExceptionContext *, struct ExecBase *, struct IntData *);

uint32
GetCounterStart(void) {
    uint64 tb;
    double bit0time;
    uint32 count;

    GetCPUInfoTags(GCIT_TimeBaseSpeed,	&tb, TAG_DONE);

    count = (uint32) (tb / (2 * 100 + 1));

    return (uint32) (-count);
}

uint32
CounterIntFn(struct ExceptionContext *ctx, struct ExecBase *ExecBase, struct IntData *profileData) {
    uint32 sia = (uint32) ctx->ip;

    /* Silence compiler */
    (void) ExecBase;
    (void) ctx;

    sia = ((sia - profileData->Offset) * profileData->Scale) >> 16;

    if (sia <= (profileData->BufferSize >> 1)) {
        //if (ProfileData->Buffer[sia] != 0xffff)
        profileData->Buffer[sia]++;
    }

    IPM->CounterControl(1, profileData->CounterStart, PMCI_Transition);

    return 1;
}

int
profil(unsigned short *buffer, size_t bufSize, size_t offset, unsigned int scale) {
    APTR Stack;

    if (buffer == 0) {
        /*
         * On systems with with a processor that does not support performancemonitor.resource, e.g. the AMCC PowerPC 440EP, profil() generates a DSI at process termination when buffer == 0.
         * A pointer to PerformanceMonitorIFace is never obtained, and the call to IPM->EventControlTags() when buffer == 0 attempts to dereference a NULL pointer
         * https://sourceforge.net/p/clib2/bugs/54/
         */
        if (!IPM) {
            dprintf("Cannot obtain Performance Monitor interface \n");
            return 0;
        }

        Stack = SuperState();
        IPM->EventControlTags(
                PMECT_Disable, PMEC_MasterInterrupt,
                TAG_DONE);

        IPM->SetInterruptVector(COUNTER, 0);

        IPM->Unmark(0);
        IPM->Release();
        if (Stack)
            UserState(Stack);

        return 0;
    }

    IPM = (struct PerformanceMonitorIFace *) OpenResource("performancemonitor.resource");
    if (!IPM || IPM->Obtain() != 1) {
        return 0;
    }

    Stack = SuperState();

    /* Init IntData */
    ProfileData.IPM = IPM;
    ProfileData.Buffer = buffer;
    ProfileData.BufferSize = bufSize;
    ProfileData.Offset = offset;
    ProfileData.Scale = scale;
    ProfileData.CounterStart = GetCounterStart();

    /* Set interrupt vector */
    CounterInt.is_Code = (void (*)(void)) CounterIntFn;
    CounterInt.is_Data = &ProfileData;
    IPM->SetInterruptVector(1, &CounterInt);

    /* Prepare Performance Monitor */
    IPM->MonitorControlTags(
            PMMCT_FreezeCounters,   PMMC_Unmarked,
            PMMCT_RTCBitSelect,     PMMC_BIT0,
            TAG_DONE);

    if (!IPM->CounterControl(COUNTER, ProfileData.CounterStart, PMCI_Transition)) {
        dprintf("Cannot set CounterControl\n");
    }

    IPM->EventControlTags(
            PMECT_Enable, 1,
            PMECT_Enable, PMEC_MasterInterrupt,
            TAG_DONE);

    IPM->Mark(0);
    IPM->Release();

    if (Stack)
        UserState(Stack);

    return 0;
}