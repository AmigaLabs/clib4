/*
 * $Id: debug_kgetnum1.c,v 1.3 2006-01-08 12:04:22 clib2devs Exp $
*/

#include "debug_headers.h"

/****************************************************************************/

LONG
kgetnum(VOID)
{
	LONG sum = 0;
	LONG result;
	BOOL is_negative = FALSE;
	LONG position = 0;
	LONG c;

	while(TRUE)
	{
		c = kgetc();

		if(c == '-')
		{
			if(position == 0)
			{
				is_negative = TRUE;

				kputc(c);

				position++;
			}
		}
		else if (c == '\b')
		{
			if(position > 0)
			{
				position--;
				if(position == 0)
				{
					is_negative = FALSE;

					sum = 0;
				}
				else
				{
					sum = sum / 10;
				}

				kputc(c);
				kputc(' ');
				kputc(c);
			}
		}
		else if (c == '\r')
		{
			kputc('\n');
			break;
		}
		else if ('0' <= c && c <= '9')
		{
			LONG new_sum;

			new_sum = sum * 10 + (c - '0');
			if(new_sum >= sum)
			{
				sum = new_sum;

				kputc(c);

				position++;
			}
		}
	}

	if(is_negative)
		result = (-sum);
	else
		result = sum;

	return(result);
}
