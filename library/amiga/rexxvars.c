/*
 * $Id: amiga_rexxvars.c,v 1.209 2021-01-01 10:11:59 clib4devs Exp $
*/

/* We don't want to pull in <clib/alib_protos.h> */
#define CLIB_ALIB_PROTOS_H

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

#include <rexx/rxslib.h>
#include <rexx/errors.h>

/* This is used by the stub function prototypes. The ARexx header files
   do not define it, though. */
struct Environment;

#define __NOLIBBASE__
#include <proto/rexxsyslib.h>

static struct Library *RexxSysBase;
static struct RexxSysIFace *IRexxSys;

BOOL CheckRexxMsg(struct RexxMsg *message);
LONG GetRexxVar(struct RexxMsg *message, STRPTR variable_name, STRPTR *buffer_pointer);
LONG SetRexxVar(struct RexxMsg *message, STRPTR variable_name, STRPTR value, LONG length);

/****************************************************************************/

CLIB_CONSTRUCTOR(rexxvars_init)
{
	ENTER();

	RexxSysBase = OpenLibrary(RXSNAME, 0);

	if (RexxSysBase != NULL)
	{
		IRexxSys = (struct RexxSysIFace *)GetInterface(RexxSysBase, "main", 1, 0);
		if (IRexxSys == NULL)
		{
			CloseLibrary(RexxSysBase);
			RexxSysBase = NULL;
		}
	}

	LEAVE();

	CONSTRUCTOR_SUCCEED();
}

CLIB_DESTRUCTOR(rexxvars_exit)
{
	ENTER();

	if (IRexxSys != NULL)
		DropInterface((struct Interface *)IRexxSys);

	IRexxSys = NULL;

	if (RexxSysBase != NULL)
	{
		CloseLibrary(RexxSysBase);
		RexxSysBase = NULL;
	}

	LEAVE();
}

/* This is modeled after the original assembly language code. Except for the
   fact that we compare the library base against a local, static variable
   rather then opening the library for each check. */
BOOL CheckRexxMsg(struct RexxMsg *message)
{
	BOOL result = FALSE;

	if (RexxSysBase == NULL)
		goto out;

	if (message->rm_LibBase != RexxSysBase)
		goto out;

	if (message->rm_TaskBlock == NULL)
		goto out;

	if (NOT IsRexxMsg(message))
		goto out;

	result = TRUE;

out:

	return (result);
}

#include <exec/emulation.h>

STATIC VOID
_FreeSpace(struct Environment *env, APTR mem, LONG size)
{
	STATIC CONST UWORD code[] = {0x4EAE, 0xFF88, 0x4E75}; /* jsr -120(a6) ; rts */

	EmulateTags(code,
				ET_RegisterA0, env,
				ET_RegisterA1, mem,
				ET_RegisterD0, size,
				ET_RegisterA6, RexxSysBase,
				TAG_END);
}

STATIC APTR
_GetSpace(struct Environment *env, LONG size)
{
	STATIC CONST UWORD code[] = {0x4EAE, 0xFF8E, 0x4E75}; /* jsr -114(a6) ; rts */
	APTR result;

	result = (APTR)EmulateTags(code,
							   ET_RegisterA0, env,
							   ET_RegisterD0, size,
							   ET_RegisterA6, RexxSysBase,
							   TAG_END);

	return (result);
}

STATIC LONG
_IsSymbol(STRPTR name, LONG *symbol_length_ptr)
{
	STATIC CONST UWORD code[] = {0x4EAE, 0xFF9A, 0x2481, 0x4E75}; /* jsr -102(a6) ; move.l d1,(a2) ; rts */
	LONG result;

	result = (LONG)EmulateTags(code,
							   ET_RegisterA0, name,
							   ET_RegisterA2, symbol_length_ptr,
							   ET_RegisterA6, RexxSysBase,
							   TAG_END);

	return (result);
}

STATIC VOID
_CurrentEnv(struct RexxTask *task, struct Environment **environment_ptr)
{
	STATIC CONST UWORD code[] = {0x4EAE, 0xFF94, 0x2488, 0x4E75}; /* jsr -108(a6) ; move.l a0,(a2) ; rts */

	EmulateTags(code,
				ET_RegisterA0, task,
				ET_RegisterA2, environment_ptr,
				ET_RegisterA6, RexxSysBase,
				TAG_END);
}

STATIC struct Node *
_FetchValue(struct Environment *env, struct NexxStr *stem, struct NexxStr *compound, struct Node *symbol_table_node, LONG *is_literal_ptr, struct NexxStr **value_ptr)
{
	STATIC CONST UWORD code[] = {0x4EAE, 0xFFB8, 0x2488, 0x2681, 0x4E75}; /* jsr -72(a6) ; move.l a0,(a2) ; move.l d1,(a3) ; rts */
	struct Node *result;

	result = (struct Node *)EmulateTags(code,
										ET_RegisterA0, env,
										ET_RegisterA1, stem,
										ET_RegisterD0, compound,
										ET_RegisterD1, symbol_table_node,
										ET_RegisterA2, is_literal_ptr,
										ET_RegisterA3, value_ptr,
										ET_RegisterA6, RexxSysBase,
										TAG_END);

	return (result);
}

STATIC struct Node *
_EnterSymbol(struct Environment *env, struct NexxStr *stem, struct NexxStr *compound)
{
	STATIC CONST UWORD code[] = {0x4EAE, 0xFFBE, 0x4E75}; /* jsr -66(a6) ; rts */
	struct Node *result;

	result = (struct Node *)EmulateTags(code,
										ET_RegisterA0, env,
										ET_RegisterA1, stem,
										ET_RegisterD0, compound,
										ET_RegisterA6, RexxSysBase,
										TAG_END);

	return (result);
}

STATIC VOID
_SetValue(struct Environment *env, struct NexxStr *value, struct Node *symbol_table_node)
{
	STATIC CONST UWORD code[] = {0x4EAE, 0xFFAC, 0x4E75}; /* jsr -84(a6) ; rts */

	EmulateTags(code,
				ET_RegisterA0, env,
				ET_RegisterA1, value,
				ET_RegisterD0, symbol_table_node,
				ET_RegisterA6, RexxSysBase,
				TAG_END);
}

STATIC ULONG
_StrcpyN(STRPTR destination, STRPTR source, LONG length)
{
	STATIC CONST UWORD code[] = {0x4EAE, 0xFEF2, 0x4E75}; /* jsr -270(a6) ; rts */
	ULONG result;

	result = (ULONG)EmulateTags(code,
								ET_RegisterA0, destination,
								ET_RegisterA1, source,
								ET_RegisterD0, length,
								ET_RegisterA6, RexxSysBase,
								TAG_END);

	return (result);
}

/****************************************************************************/

/* Releases a string structure, if it's not owned at the time. */
STATIC VOID
FreeString(struct Environment *environment, struct NexxStr *ns)
{
	/* Not currently owned? */
	if (!(ns->ns_Flags & NSF_KEEP))
		_FreeSpace(environment, ns, sizeof(*ns) + ns->ns_Length + 1);
}

/****************************************************************************/

/* Allocates and initializes a string structure. */
STATIC struct NexxStr *
MakeString(struct Environment *environment, STRPTR value, LONG length)
{
	struct NexxStr *result = NULL;
	struct NexxStr *ns;

	/* Allocate memory for the NexxStr and the NUL-terminated string itself. */
	ns = _GetSpace(environment, sizeof(*ns) + length + 1);
	if (ns == NULL)
		goto out;

	/* Fill in the NexxStr structure, copy the string and remember the hash value for it. */
	ns->ns_Ivalue = 0;
	ns->ns_Length = length;
	ns->ns_Flags = NSF_STRING;
	ns->ns_Hash = _StrcpyN((STRPTR)ns->ns_Buff, value, length);

	/* Copying the string did not NUL-terminate it. */
	ns->ns_Buff[length] = '\0';

	result = ns;

out:

	return (result);
}

/****************************************************************************/

/* Classifies a symbol and returns the stem and compound parts. */
STATIC LONG
TypeString(STRPTR variable_name, struct Environment *environment, struct NexxStr **compound_ptr, struct NexxStr **stem_ptr)
{
	struct NexxStr *compound;
	struct NexxStr *stem = NULL;
	LONG error = ERR10_003; /* no memory available */
	LONG stem_length;
	LONG symbol_length;
	STRPTR dot;

	(*compound_ptr) = NULL;
	(*stem_ptr) = NULL;

	/* The 'compound' part is the entire variable name, including all dots and
	   what's in between them. */
	compound = MakeString(environment, variable_name, strlen(variable_name));
	if (compound == NULL)
		goto out;

	/* Find the first dot in the variable name. Everything in front of it
	   and including the dot constitutes the 'stem' part. If there is no dot
	   in the name, then the 'compound' and 'stem' parts are identical. */
	dot = memchr(compound->ns_Buff, '.', compound->ns_Length);
	if (dot != NULL)
		stem_length = ((char *)dot - (char *)compound->ns_Buff) + 1;
	else
		stem_length = compound->ns_Length;

	/* Make a copy of the 'stem' part. */
	stem = MakeString(environment, variable_name, stem_length);
	if (stem == NULL)
		goto out;

	/* Figure out if this is a symbol after all. */
	_IsSymbol((STRPTR)stem->ns_Buff, &symbol_length);

	/* The entire name must match the stem part. */
	if (symbol_length != stem->ns_Length)
	{
		error = ERR10_040; /* invalid variable name */
		goto out;
	}

	(*compound_ptr) = compound;
	(*stem_ptr) = stem;

	error = 0;

out:

	if (error != 0)
	{
		if (compound != NULL)
			FreeString(environment, compound);

		if (stem != NULL)
			FreeString(environment, stem);
	}

	return (error);
}

/****************************************************************************/

/* Retrieves the value of a variable from the current storage environment. */
LONG GetRexxVar(struct RexxMsg *context, STRPTR variable_name, STRPTR *return_value)
{
	struct Environment *environment;
	struct NexxStr *ns;
	struct NexxStr *compound_string;
	struct NexxStr *stem_string;
	LONG is_literal;
	LONG error;

	(*return_value) = NULL;

	if (!CheckRexxMsg(context))
	{
		error = ERR10_010; /* invalid message packet */
		goto out;
	}

	/* Find the current storage environment. */
	_CurrentEnv(context->rm_TaskBlock, &environment);

	/* Create the stem and component parts. */
	error = TypeString(variable_name, environment, &compound_string, &stem_string);
	if (error != 0)
		goto out;

	/* Look up the value. NOTE: _FetchValue() will free the two 'struct NexxStr *' provided. */
	_FetchValue(environment, stem_string, compound_string, NULL, &is_literal, &ns);

	/* If this is not a literal, return a pointer to the string. */
	if (!is_literal)
		(*return_value) = (STRPTR)ns->ns_Buff;

	error = 0;

out:

	return (error);
}

/****************************************************************************/

/* Assigns a value to a variable in the current storage environment. */
LONG SetRexxVar(struct RexxMsg *context, STRPTR variable_name, STRPTR value, LONG length)
{
	struct Environment *environment;
	struct NexxStr *compound_string;
	struct NexxStr *stem_string;
	struct Node *symbol_table_node;
	struct NexxStr *value_string;
	LONG error;

	/* Make sure the value string is not too long */
	if (length > 65535)
	{
		error = ERR10_009; /* symbol or string too long */
		goto out;
	}

	if (!CheckRexxMsg(context))
	{
		error = ERR10_010; /* invalid message packet */
		goto out;
	}

	/* Find the current storage environment. */
	_CurrentEnv(context->rm_TaskBlock, &environment);

	/* Create the stem and compound parts */
	error = TypeString(variable_name, environment, &compound_string, &stem_string);
	if (error != 0)
		goto out;

	/* Locate or create the symbol node. NOTE: _EnterSymbol() will put the two 'struct NexxStr *' into
	   the symbol table. It is not nececessary to free them. */
	symbol_table_node = _EnterSymbol(environment, stem_string, compound_string);
	if (symbol_table_node == NULL)
	{
		error = ERR10_003; /* no memory available */
		goto out;
	}

	/* Create the value string. */
	value_string = MakeString(environment, value, length);
	if (value_string == NULL)
	{
		error = ERR10_003; /* no memory available */
		goto out;
	}

	/* Install the value string. */
	_SetValue(environment, value_string, symbol_table_node);

	error = 0;

out:

	return (error);
}
