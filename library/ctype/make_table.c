/*
 * $Id: make_ctype_table.c,v 1.3 2006-01-08 12:04:23 clib2devs Exp $
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/****************************************************************************/

#define __CTYPE_CONTROL		0x01
#define __CTYPE_DIGIT		0x02
#define __CTYPE_HEX_DIGIT	0x04
#define __CTYPE_PUNCTUATION	0x08
#define __CTYPE_PRINTABLE	0x10
#define __CTYPE_WHITE_SPACE	0x20
#define __CTYPE_LOWER_CASE	0x40
#define __CTYPE_UPPER_CASE	0x80

/****************************************************************************/

unsigned char __ctype_table[128];

/****************************************************************************/

struct { char * name; int flag; } tab[] =
{

	{ "__CTYPE_CONTROL",		__CTYPE_CONTROL },
	{ "__CTYPE_DIGIT",			__CTYPE_DIGIT },
	{ "__CTYPE_HEX_DIGIT",		__CTYPE_HEX_DIGIT },
	{ "__CTYPE_PUNCTUATION",	__CTYPE_PUNCTUATION },
	{ "__CTYPE_PRINTABLE",		__CTYPE_PRINTABLE },
	{ "__CTYPE_WHITE_SPACE",	__CTYPE_WHITE_SPACE },
	{ "__CTYPE_LOWER_CASE",		__CTYPE_LOWER_CASE },
	{ "__CTYPE_UPPER_CASE",		__CTYPE_UPPER_CASE },
	{ NULL,						0 }

};

/****************************************************************************/

#define _isalnum(c)		((__ctype_table[c] & (__CTYPE_DIGIT|__CTYPE_LOWER_CASE|__CTYPE_UPPER_CASE)) != 0)
#define _isalpha(c)		((__ctype_table[c] & (__CTYPE_LOWER_CASE|__CTYPE_UPPER_CASE)) != 0)
#define _iscntrl(c)		((__ctype_table[c] & __CTYPE_CONTROL) != 0)
#define _isdigit(c)		((__ctype_table[c] & __CTYPE_DIGIT) != 0)
#define _isxdigit(c)	((__ctype_table[c] & __CTYPE_HEX_DIGIT) != 0)
#define _isgraph(c)		((__ctype_table[c] & (__CTYPE_DIGIT|__CTYPE_PUNCTUATION|__CTYPE_LOWER_CASE|__CTYPE_UPPER_CASE)) != 0)
#define _ispunct(c)		((__ctype_table[c] & __CTYPE_PUNCTUATION) != 0)
#define _isprint(c)		((__ctype_table[c] & __CTYPE_PRINTABLE) != 0)
#define _islower(c)		((__ctype_table[c] & __CTYPE_LOWER_CASE) != 0)
#define _isupper(c)		((__ctype_table[c] & __CTYPE_UPPER_CASE) != 0)
#define _isspace(c)		((__ctype_table[c] & __CTYPE_WHITE_SPACE) != 0)

/****************************************************************************/

void
dump_table(void)
{
	int i;

	printf("const unsigned char __ctype_table[128] =\n{\n");

	for(i = 0 ; i < 128 ; i++)
	{
		printf("\t/* %3d",i);

		if(' ' <= i && i <= '~')
			printf(", '%c'",i);
		else
			printf("     ");

		printf(" */ ");

		if(__ctype_table[i] != 0)
		{
			int j,k;

			for(j = k = 0 ; tab[j].name != NULL ; j++)
			{
				if(__ctype_table[i] & tab[j].flag)
				{
					if(k++ > 0)
						printf("|");

					printf("%s",tab[j].name);
				}
			}
		}
		else
		{
			printf("0");
		}

		if(i < 127)
			printf(",");

		printf("\n");
	}

	printf("};\n");
}

/****************************************************************************/

int
main(void)
{
	int i,a,b;

	/* Rebuild the contents of the character type table. */
	memset(__ctype_table,0,sizeof(__ctype_table));

	for(i = 0 ; i < 128 ; i++)
	{
		if(iscntrl(i))
			__ctype_table[i] |= __CTYPE_CONTROL;

		if(isdigit(i))
			__ctype_table[i] |= __CTYPE_DIGIT;

		if(isxdigit(i))
			__ctype_table[i] |= __CTYPE_HEX_DIGIT;

		if(ispunct(i))
			__ctype_table[i] |= __CTYPE_PUNCTUATION;

		if(isspace(i))
			__ctype_table[i] |= __CTYPE_WHITE_SPACE;

		if(islower(i))
			__ctype_table[i] |= __CTYPE_LOWER_CASE;

		if(isupper(i))
			__ctype_table[i] |= __CTYPE_UPPER_CASE;

		if(isprint(i))
			__ctype_table[i] |= __CTYPE_PRINTABLE;
	}

	/* Verify that the table contents match the functional
	   definitions. */
	for(i = 0 ; i < 128 ; i++)
	{
		a = ( isalnum(i) != 0);
		b = (_isalnum(i) != 0);

		if(a != b)
		{
			printf("isalnum(%d) %d != %d\n",i,a,b);
			abort();
		}
	}

	for(i = 0 ; i < 128 ; i++)
	{
		a = ( isalpha(i) != 0);
		b = (_isalpha(i) != 0);

		if(a != b)
		{
			printf("isalpha(%d) %d != %d\n",i,a,b);
			abort();
		}
	}

	for(i = 0 ; i < 128 ; i++)
	{
		a = ( iscntrl(i) != 0);
		b = (_iscntrl(i) != 0);

		if(a != b)
		{
			printf("iscntrl(%d) %d != %d\n",i,a,b);
			abort();
		}
	}

	for(i = 0 ; i < 128 ; i++)
	{
		a = ( isdigit(i) != 0);
		b = (_isdigit(i) != 0);

		if(a != b)
		{
			printf("isdigit(%d) %d != %d\n",i,a,b);
			abort();
		}
	}

	for(i = 0 ; i < 128 ; i++)
	{
		a = ( isgraph(i) != 0);
		b = (_isgraph(i) != 0);

		if(a != b)
		{
			printf("isgraph(%d) %d != %d\n",i,a,b);
			abort();
		}
	}

	for(i = 0 ; i < 128 ; i++)
	{
		a = ( ispunct(i) != 0);
		b = (_ispunct(i) != 0);

		if(a != b)
		{
			printf("ispunct(%d) %d != %d\n",i,a,b);
			abort();
		}
	}

	for(i = 0 ; i < 128 ; i++)
	{
		a = ( isprint(i) != 0);
		b = (_isprint(i) != 0);

		if(a != b)
		{
			printf("isprint(%d) %d != %d\n",i,a,b);
			abort();
		}
	}

	for(i = 0 ; i < 128 ; i++)
	{
		a = ( islower(i) != 0);
		b = (_islower(i) != 0);

		if(a != b)
		{
			printf("islower(%d) %d != %d\n",i,a,b);
			abort();
		}
	}

	for(i = 0 ; i < 128 ; i++)
	{
		a = ( isupper(i) != 0);
		b = (_isupper(i) != 0);

		if(a != b)
		{
			printf("isupper(%d) %d != %d\n",i,a,b);
			abort();
		}
	}

	for(i = 0 ; i < 128 ; i++)
	{
		a = ( isspace(i) != 0);
		b = (_isspace(i) != 0);

		if(a != b)
		{
			printf("isspace(%d) %d != %d\n",i,a,b);
			abort();
		}
	}

	/* If that went well, dump the table as is. */
	dump_table();

	return(0);
}
