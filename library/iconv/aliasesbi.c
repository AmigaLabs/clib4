/*
 * $Id: aliasesbi.c,v 1.0 2021-03-09 12:04:25 apalmate Exp $
 *
 * :ts=4
 *
 * Portable ISO 'C' (1994) runtime library for the Amiga computer
 * Copyright (c) 2002-2015 by Olaf Barthel <obarthel (at) gmx.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Neither the name of Olaf Barthel nor the names of contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "encnames.h"

const char *
    _iconv_aliases =
        {
#if defined(_ICONV_FROM_ENCODING_AMIGA_1251) || defined(_ICONV_TO_ENCODING_AMIGA_1251)
            ICONV_ENCODING_AMIGA_1251 "\n"
#endif
#if defined(_ICONV_FROM_ENCODING_BIG5) || defined(_ICONV_TO_ENCODING_BIG5)
            ICONV_ENCODING_BIG5 " csbig5 big_five bigfive cn_big5 cp950\n"
#endif
#if defined(_ICONV_FROM_ENCODING_CP437) || defined(_ICONV_TO_ENCODING_CP437)
            ICONV_ENCODING_CP437 " ibm437 437\n"
#endif
#if defined(_ICONV_FROM_ENCODING_CP775) || defined(_ICONV_TO_ENCODING_CP775)
            ICONV_ENCODING_CP775 " ibm775 cspc775baltic\n"
#endif
#if defined(_ICONV_FROM_ENCODING_CP850) || defined(_ICONV_TO_ENCODING_CP850)
            ICONV_ENCODING_CP850 " ibm850 850 cspc850multilingual\n"
#endif
#if defined(_ICONV_FROM_ENCODING_CP852) || defined(_ICONV_TO_ENCODING_CP852)
            ICONV_ENCODING_CP852 " ibm852 852 cspcp852\n"
#endif
#if defined(_ICONV_FROM_ENCODING_CP855) || defined(_ICONV_TO_ENCODING_CP855)
            ICONV_ENCODING_CP855 " ibm855 855 csibm855\n"
#endif
#if defined(_ICONV_FROM_ENCODING_CP866) || defined(_ICONV_TO_ENCODING_CP866)
            ICONV_ENCODING_CP866 " 866 IBM866 CSIBM866\n"
#endif
#if defined(_ICONV_FROM_ENCODING_EUC_JP) || defined(_ICONV_TO_ENCODING_EUC_JP)
            ICONV_ENCODING_EUC_JP " eucjp\n"
#endif
#if defined(_ICONV_FROM_ENCODING_EUC_KR) || defined(_ICONV_TO_ENCODING_EUC_KR)
            ICONV_ENCODING_EUC_KR " euckr\n"
#endif
#if defined(_ICONV_FROM_ENCODING_EUC_TW) || defined(_ICONV_TO_ENCODING_EUC_TW)
            ICONV_ENCODING_EUC_TW " euctw\n"
#endif
#if defined(_ICONV_FROM_ENCODING_ISO_8859_1) || defined(_ICONV_TO_ENCODING_ISO_8859_1)
            ICONV_ENCODING_ISO_8859_1 " iso8859_1 iso88591 iso_8859_1:1987 iso_ir_100 latin1 l1 ibm819 cp819 csisolatin1\n"
#endif
#if defined(_ICONV_FROM_ENCODING_ISO_8859_10) || defined(_ICONV_TO_ENCODING_ISO_8859_10)
            ICONV_ENCODING_ISO_8859_10 " iso_8859_10:1992 iso_ir_157 iso885910 latin6 l6 csisolatin6 iso8859_10\n"
#endif
#if defined(_ICONV_FROM_ENCODING_ISO_8859_11) || defined(_ICONV_TO_ENCODING_ISO_8859_11)
            ICONV_ENCODING_ISO_8859_11 " iso8859_11 iso885911\n"
#endif
#if defined(_ICONV_FROM_ENCODING_ISO_8859_13) || defined(_ICONV_TO_ENCODING_ISO_8859_13)
            ICONV_ENCODING_ISO_8859_13 " iso_8859_13:1998 iso8859_13 iso885913 latin7 l7\n"
#endif
#if defined(_ICONV_FROM_ENCODING_ISO_8859_14) || defined(_ICONV_TO_ENCODING_ISO_8859_14)
            ICONV_ENCODING_ISO_8859_14 " iso_8859_14:1998 iso885914 iso8859_14 iso_ir_199 latin8 l8 iso_celtic\n"
#endif
#if defined(_ICONV_FROM_ENCODING_ISO_8859_15) || defined(_ICONV_TO_ENCODING_ISO_8859_15)
            ICONV_ENCODING_ISO_8859_15 " iso885915 iso_8859_15:1998 iso8859_15 latin9 l9\n"
#endif
#if defined(_ICONV_FROM_ENCODING_ISO_8859_16) || defined(_ICONV_TO_ENCODING_ISO_8859_16)
            ICONV_ENCODING_ISO_8859_16 " iso8859_16 iso885916 iso_8859_16:2001 iso_ir_226 latin10  l10\n"
#endif
#if defined(_ICONV_FROM_ENCODING_ISO_8859_2) || defined(_ICONV_TO_ENCODING_ISO_8859_2)
            ICONV_ENCODING_ISO_8859_2 " iso8859_2 iso88592 iso_8859_2:1987 iso_ir_101 latin2 l2 csisolatin2\n"
#endif
#if defined(_ICONV_FROM_ENCODING_ISO_8859_3) || defined(_ICONV_TO_ENCODING_ISO_8859_3)
            ICONV_ENCODING_ISO_8859_3 " iso8859_3 iso_8859_3:1988 iso_ir_109 iso8859_3 latin3 l3 csisolatin3 iso88593\n"
#endif
#if defined(_ICONV_FROM_ENCODING_ISO_8859_4) || defined(_ICONV_TO_ENCODING_ISO_8859_4)
            ICONV_ENCODING_ISO_8859_4 " iso8859_4 iso88594 iso_8859_4:1988 iso_ir_110 latin4 l4 csisolatin4\n"
#endif
#if defined(_ICONV_FROM_ENCODING_ISO_8859_5) || defined(_ICONV_TO_ENCODING_ISO_8859_5)
            ICONV_ENCODING_ISO_8859_5 " iso8859_5 iso88595 iso_8859_5:1988 iso_ir_144 cyrillic csisolatincyrillic\n"
#endif
#if defined(_ICONV_FROM_ENCODING_ISO_8859_6) || defined(_ICONV_TO_ENCODING_ISO_8859_6)
            ICONV_ENCODING_ISO_8859_6 " iso_8859_6:1987 iso_ir_127 iso8859_6 ecma_114 asmo_708 arabic csisolatinarabic iso88596\n"
#endif
#if defined(_ICONV_FROM_ENCODING_ISO_8859_7) || defined(_ICONV_TO_ENCODING_ISO_8859_7)
            ICONV_ENCODING_ISO_8859_7 " iso_8859_7:1987 iso_ir_126 iso8859_7 elot_928 ecma_118 greek greek8 csisolatingreek iso88597\n"
#endif
#if defined(_ICONV_FROM_ENCODING_ISO_8859_8) || defined(_ICONV_TO_ENCODING_ISO_8859_8)
            ICONV_ENCODING_ISO_8859_8 " iso_8859_8:1988 iso_ir_138 iso8859_8 hebrew csisolatinhebrew iso88598\n"
#endif
#if defined(_ICONV_FROM_ENCODING_ISO_8859_9) || defined(_ICONV_TO_ENCODING_ISO_8859_9)
            ICONV_ENCODING_ISO_8859_9 " iso_8859_9:1989 iso_ir_148 iso8859_9 latin5 l5 csisolatin5 iso88599\n"
#endif
#if defined(_ICONV_FROM_ENCODING_ISO_IR_111) || defined(_ICONV_TO_ENCODING_ISO_IR_111)
            ICONV_ENCODING_ISO_IR_111 " ecma_cyrillic koi8_e koi8e csiso111ecmacyrillic\n"
#endif
#if defined(_ICONV_FROM_ENCODING_KOI8_R) || defined(_ICONV_TO_ENCODING_KOI8_R)
            ICONV_ENCODING_KOI8_R " cskoi8r koi8r koi8\n"
#endif
#if defined(_ICONV_FROM_ENCODING_KOI8_RU) || defined(_ICONV_TO_ENCODING_KOI8_RU)
            ICONV_ENCODING_KOI8_RU " koi8ru\n"
#endif
#if defined(_ICONV_FROM_ENCODING_KOI8_U) || defined(_ICONV_TO_ENCODING_KOI8_U)
            ICONV_ENCODING_KOI8_U " koi8u\n"
#endif
#if defined(_ICONV_FROM_ENCODING_KOI8_UNI) || defined(_ICONV_TO_ENCODING_KOI8_UNI)
            ICONV_ENCODING_KOI8_UNI " koi8uni\n"
#endif
#if defined(_ICONV_FROM_ENCODING_TIS_620) || defined(_ICONV_TO_ENCODING_TIS_620)
            ICONV_ENCODING_TIS_620 "\n"
#endif
#if defined(_ICONV_FROM_ENCODING_UCS_2) || defined(_ICONV_TO_ENCODING_UCS_2)
            ICONV_ENCODING_UCS_2 " ucs2 iso_10646_ucs_2 iso10646_ucs_2 iso_10646_ucs2 iso10646_ucs2 iso10646ucs2 csUnicode\n"
#endif
#if defined(_ICONV_FROM_ENCODING_UCS_2_INTERNAL) || defined(_ICONV_TO_ENCODING_UCS_2_INTERNAL)
            ICONV_ENCODING_UCS_2_INTERNAL " ucs2_internal ucs_2internal ucs2internal\n"
#endif
#if defined(_ICONV_FROM_ENCODING_UCS_2BE) || defined(_ICONV_TO_ENCODING_UCS_2BE)
            ICONV_ENCODING_UCS_2BE " ucs2be\n"
#endif
#if defined(_ICONV_FROM_ENCODING_UCS_2LE) || defined(_ICONV_TO_ENCODING_UCS_2LE)
            ICONV_ENCODING_UCS_2LE " ucs2le\n"
#endif
#if defined(_ICONV_FROM_ENCODING_UCS_4) || defined(_ICONV_TO_ENCODING_UCS_4)
            ICONV_ENCODING_UCS_4 " ucs4 iso_10646_ucs_4 iso10646_ucs_4 iso_10646_ucs4 iso10646_ucs4 iso10646ucs4\n"
#endif
#if defined(_ICONV_FROM_ENCODING_UCS_4_INTERNAL) || defined(_ICONV_TO_ENCODING_UCS_4_INTERNAL)
            ICONV_ENCODING_UCS_4_INTERNAL " ucs4_internal ucs_4internal ucs4internal\n"
#endif
#if defined(_ICONV_FROM_ENCODING_UCS_4BE) || defined(_ICONV_TO_ENCODING_UCS_4BE)
            ICONV_ENCODING_UCS_4BE " ucs4be\n"
#endif
#if defined(_ICONV_FROM_ENCODING_UCS_4LE) || defined(_ICONV_TO_ENCODING_UCS_4LE)
            ICONV_ENCODING_UCS_4LE " ucs4le\n"
#endif
#if defined(_ICONV_FROM_ENCODING_US_ASCII) || defined(_ICONV_TO_ENCODING_US_ASCII)
            ICONV_ENCODING_US_ASCII " ansi_x3.4_1968 ansi_x3.4_1986 iso_646.irv:1991 ascii iso646_us us ibm367 cp367 csascii iso_ir_6\n"
#endif
#if defined(_ICONV_FROM_ENCODING_UTF_16) || defined(_ICONV_TO_ENCODING_UTF_16)
            ICONV_ENCODING_UTF_16 " utf16\n"
#endif
#if defined(_ICONV_FROM_ENCODING_UTF_16BE) || defined(_ICONV_TO_ENCODING_UTF_16BE)
            ICONV_ENCODING_UTF_16BE " utf16be\n"
#endif
#if defined(_ICONV_FROM_ENCODING_UTF_16LE) || defined(_ICONV_TO_ENCODING_UTF_16LE)
            ICONV_ENCODING_UTF_16LE " utf16le\n"
#endif
#if defined(_ICONV_FROM_ENCODING_UTF_8) || defined(_ICONV_TO_ENCODING_UTF_8)
            ICONV_ENCODING_UTF_8 " utf8\n"
#endif
#if defined(_ICONV_FROM_ENCODING_WIN_1250) || defined(_ICONV_TO_ENCODING_WIN_1250)
            ICONV_ENCODING_WIN_1250 " cp1250 windows_1250\n"
#endif
#if defined(_ICONV_FROM_ENCODING_WIN_1251) || defined(_ICONV_TO_ENCODING_WIN_1251)
            ICONV_ENCODING_WIN_1251 " cp1251 windows_1251\n"
#endif
#if defined(_ICONV_FROM_ENCODING_WIN_1252) || defined(_ICONV_TO_ENCODING_WIN_1252)
            ICONV_ENCODING_WIN_1252 " cp1252 windows_1252\n"
#endif
#if defined(_ICONV_FROM_ENCODING_WIN_1253) || defined(_ICONV_TO_ENCODING_WIN_1253)
            ICONV_ENCODING_WIN_1253 " cp1253 windows_1253\n"
#endif
#if defined(_ICONV_FROM_ENCODING_WIN_1254) || defined(_ICONV_TO_ENCODING_WIN_1254)
            ICONV_ENCODING_WIN_1254 " cp1254 windows_1254\n"
#endif
#if defined(_ICONV_FROM_ENCODING_WIN_1255) || defined(_ICONV_TO_ENCODING_WIN_1255)
            ICONV_ENCODING_WIN_1255 " cp1255 windows_1255\n"
#endif
#if defined(_ICONV_FROM_ENCODING_WIN_1256) || defined(_ICONV_TO_ENCODING_WIN_1256)
            ICONV_ENCODING_WIN_1256 " cp1256 windows_1256\n"
#endif
#if defined(_ICONV_FROM_ENCODING_WIN_1257) || defined(_ICONV_TO_ENCODING_WIN_1257)
            ICONV_ENCODING_WIN_1257 " cp1257 windows_1257\n"
#endif
#if defined(_ICONV_FROM_ENCODING_WIN_1258) || defined(_ICONV_TO_ENCODING_WIN_1258)
            ICONV_ENCODING_WIN_1258 " cp1258 windows_1258\n"
#endif
                                      ""};
