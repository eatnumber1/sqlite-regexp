/*
 * Copyright (c) 2010 Russell Harmon <russ@eatnumber1.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <string.h>
#include <assert.h>

#include <pcre.h>

#include <sqlite3ext.h>
SQLITE_EXTENSION_INIT1

static void sqlite3_regexp( sqlite3_context *context, int argc, sqlite3_value **argv ) {
	assert(argc == 2);
	char *pcre_err = NULL;
	int pcre_erroffset, pcre_errcode;
	pcre *pattern = pcre_compile2((const char *) sqlite3_value_text(argv[0]), PCRE_UTF8, &pcre_errcode, (const char **) &pcre_err, &pcre_erroffset, NULL);
	if( pattern == NULL ) {
		// pcre error code 21 is out of memory as per pcreapi(3)
		if( pcre_errcode == 21 ) {
			sqlite3_result_error_nomem(context);
		} else {
			sqlite3_result_error(context, pcre_err, strlen(pcre_err));
		}
	} else {
		const unsigned char *subject = sqlite3_value_text(argv[1]);
		pcre_errcode = pcre_exec(pattern, NULL, (const char *) subject, strlen((const char *) subject), 0, 0, NULL, 0);
		if( pcre_errcode < 0 ) {
			char *errmsg;
			switch( pcre_errcode ) {
				case PCRE_ERROR_NOMATCH:
					sqlite3_result_int(context, 0);
					break;
				case PCRE_ERROR_NOMEMORY:
					sqlite3_result_error_nomem(context);
					break;
				default:
					errmsg = sqlite3_mprintf("pcre_exec: Error code %d\n", pcre_errcode);
					if( errmsg == NULL ) sqlite3_result_error_nomem(context);
					sqlite3_result_error(context, errmsg, strlen(errmsg));
					sqlite3_free(errmsg);
			}
		} else {
			sqlite3_result_int(context, 1);
		}

		pcre_free(pattern);
	}
}

int sqlite3_extension_init( sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi ) {
	SQLITE_EXTENSION_INIT2(pApi)
	return sqlite3_create_function(db, "regexp", 2, SQLITE_UTF8, 0, sqlite3_regexp, 0, 0);
}
