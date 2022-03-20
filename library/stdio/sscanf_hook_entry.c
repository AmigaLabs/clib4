/*
 * $Id: stdio_sscanf_hook_entry.c,v 1.6 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int64_t
__sscanf_hook_entry(
	struct iob *string_iob,
	struct file_action_message *fam)
{
	int64_t result = EOF;
	int64_t num_bytes;

	assert(fam != NULL && string_iob != NULL);

	if (fam->fam_Action != file_action_read)
	{
		fam->fam_Error = EBADF;
		goto out;
	}

	assert(string_iob->iob_StringPosition >= 0);
	assert(string_iob->iob_StringLength >= 0);

	if (string_iob->iob_StringPosition < string_iob->iob_StringLength)
	{
		int64_t num_bytes_left = string_iob->iob_StringLength - string_iob->iob_StringPosition;

		num_bytes = fam->fam_Size;
		if (num_bytes > num_bytes_left)
			num_bytes = num_bytes_left;

		assert(fam->fam_Data != NULL);
		assert(num_bytes >= 0);

		memmove(fam->fam_Data, &string_iob->iob_String[string_iob->iob_StringPosition], (size_t)num_bytes);
		string_iob->iob_StringPosition += num_bytes;
	}
	else
	{
		num_bytes = 0;
	}

	result = num_bytes;

out:

	return (result);
}
