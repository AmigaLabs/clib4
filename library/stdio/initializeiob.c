/*
 * $Id: stdio_initializeiob.c,v 1.6 2006-09-22 09:02:51 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

void 
__initialize_iob(
	struct iob *iob,
	file_action_iob_t action_function,
	STRPTR custom_buffer,
	STRPTR buffer,
	int64_t buffer_size,
	int file_descriptor,
	int slot_number,
	ULONG flags,
	struct SignalSemaphore *lock)
{
	assert(iob != NULL && action_function != NULL);

	memset(iob, 0, sizeof(*iob));

	iob->iob_CustomBuffer = custom_buffer;
	iob->iob_Buffer = (UBYTE *)buffer;
	iob->iob_BufferSize = buffer_size;
	iob->iob_Descriptor = file_descriptor;
	iob->iob_SlotNumber = slot_number;
	iob->iob_Flags = flags &~__SORD;
	iob->iob_Action = action_function;
	iob->iob_Lock = lock;
	iob->iob_Flags2 = 0;
	memset (&iob->iob_mbState, 0, sizeof (_mbstate_t));
}
