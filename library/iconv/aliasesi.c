/*
 * $Id: aliasesi.c,v 1.0 2021-03-09 12:04:25 clib2devs Exp $
 *
 * Copyright (c) 2003-2004, Artem B. Bityuckiy
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <sys/iconvnls.h>
#include "local.h"

static const char *
canonical_form(const char *str)
{
  char *p, *p1;

  if (str == NULL || (p = p1 = strdup(str)) == NULL)
    return (const char *)NULL;

  for (; *str; str++, p++)
  {
    if (*str == '-')
      *p = '_';
    else
      *p = tolower(*str);
  }

  return (const char *)p1;
}

static char *
find_alias(const char *alias, const char *table, int len)
{
  const char *end;
  const char *p;
  int l = strlen(alias);
  const char *ptable = table;
  const char *table_end = table + len;

  if (table == NULL || alias == NULL || *table == '\0' || *alias == '\0')
    return NULL;

search_again:
  if (len < l || (p = strnstr(ptable, alias, len)) == NULL)
    return NULL;

  /* Check that substring is segregated by '\n', '\t' or ' ' */
  if (!((p == table || isspace(*(p - 1)) || *(p - 1) == '\n') && (p + l == table_end || isspace(*(p + l)) || *(p + l) == '\n')))
  {
    ptable = p + l;
    len -= table - p;
    goto search_again;
  }

  while (--p > table && *p != '\n')
    ;

  if (*(++p) == '#')
    return NULL;

  for (end = p + 1; !isspace(*end) && *end != '\n' && *end != '\0'; end++)
    ;

  return strndup(p, (size_t)(end - p));
}

char *
_iconv_resolve_encoding_name(const char *ca)
{
  char *p = (char *)ca;

  /* Alias shouldn't contain white spaces, '\n' and '\r' symbols */
  while (*p)
    if (*p == ' ' || *p == '\r' || *p++ == '\n')
      return NULL;

  if ((ca = canonical_form(ca)) == NULL)
    return NULL;

  p = find_alias(ca, _iconv_aliases, strlen(_iconv_aliases));
  if (!p)
  {
    p = strdup(ca);
  }

  free((void *)ca);
  return p;
}
