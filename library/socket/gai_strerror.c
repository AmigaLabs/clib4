/*
 * $Id: socket_getnameinfo.c,v 1.0 2021-01-13 11:31:35 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

static struct {
    int code;
    const char *str;
} errors[] = {
            {EAI_NOERROR,       "No error"},
            {EAI_ADDRFAMILY,    "Address family for nodename not supported"},
            {EAI_AGAIN,         "Temporary failure in name resolution"},
            {EAI_BADFLAGS,      "Invalid value for ai_flags"},
            {EAI_FAIL,          "Non-recoverable failure in name resolution"},
            {EAI_FAMILY,        "ai_family not supported"},
            {EAI_MEMORY,        "Memory allocation failure"},
            {EAI_NODATA,        "No address associated with nodename"},
            {EAI_NONAME,        "Nodename nor servname provided, or not known"},
            {EAI_SERVICE,       "Servname not supported for ai_socktype"},
            {EAI_SOCKTYPE,      "ai_socktype not supported"},
            {EAI_SYSTEM,        "System error returned in errno"},
            {EAI_OVERFLOW, 		"Argument buffer too small" },
#ifdef EAI_INPROGRESS
            {EAI_INPROGRESS, 	"Processing request in progress" },
    	    {EAI_CANCELED, 		"Request canceled" },
    	    {EAI_NOTCANCELED,   "Request not canceled" },
    	    {EAI_ALLDONE, 		"All requests done" },
    	    {EAI_INTR, 		    "Interrupted by a signal" },
    	    {EAI_IDN_ENCODE, 	"Parameter string not correctly encoded" },
#endif
            {0, NULL},
};

const char *
gai_strerror(int code)
{
	size_t i;
	for (i = 0; i < sizeof(errors) / sizeof(errors[0]); ++i)
		if (errors[i].code == code)
			return errors[i].str;

	return "Unknown error";
}
