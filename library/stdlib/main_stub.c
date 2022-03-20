/*
 * $Id: stdlib_main_stub.c,v 1.2 2006-01-08 12:04:25 clib2devs Exp $
*/

#if defined(__GNUC__)

/****************************************************************************/

/* The following is automatically called by the main() function through code
   inserted by GCC. In theory, this could be removed by updating the machine
   definition, but for now we'll just keep this stub around. It is intended
   to call the constructor functions, but we do this in our own _main()
   anyway. */

/****************************************************************************/

void __main(void);

/****************************************************************************/

void
__main(void)
{
}

/****************************************************************************/

#endif /* __GNUC__ */
