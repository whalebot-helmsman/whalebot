/*
 * This file is part of Hubbub.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef hubbub_tokeniser_tokeniser_h_
#define hubbub_tokeniser_tokeniser_h_

#include <stdbool.h>
#include <inttypes.h>

#include <hubbub/errors.h>
#include <hubbub/functypes.h>
#include <hubbub/types.h>

#include <parserutils/input/inputstream.h>

typedef struct hubbub_tokeniser hubbub_tokeniser;

/**
 * Hubbub tokeniser option types
 */
typedef enum hubbub_tokeniser_opttype {
	HUBBUB_TOKENISER_TOKEN_HANDLER,
	HUBBUB_TOKENISER_ERROR_HANDLER,
	HUBBUB_TOKENISER_CONTENT_MODEL,
	HUBBUB_TOKENISER_PROCESS_CDATA
} hubbub_tokeniser_opttype;

/**
 * Hubbub tokeniser option parameters
 */
typedef union hubbub_tokeniser_optparams {
	struct {
		hubbub_token_handler handler;
		void *pw;
	} token_handler;		/**< Token handling callback */

	struct {
		hubbub_error_handler handler;
		void *pw;
	} error_handler;		/**< Error handling callback */

	struct {
		hubbub_content_model model;
	} content_model;		/**< Current content model */

	bool process_cdata;		/**< Whether to process CDATA sections*/
} hubbub_tokeniser_optparams;

/* Create a hubbub tokeniser */
hubbub_error hubbub_tokeniser_create(parserutils_inputstream *input,
		hubbub_allocator_fn alloc, void *pw, 
		hubbub_tokeniser **tokeniser);
/* Destroy a hubbub tokeniser */
hubbub_error hubbub_tokeniser_destroy(hubbub_tokeniser *tokeniser);

/* Configure a hubbub tokeniser */
hubbub_error hubbub_tokeniser_setopt(hubbub_tokeniser *tokeniser,
		hubbub_tokeniser_opttype type,
		hubbub_tokeniser_optparams *params);

/* Process remaining data in the input stream */
hubbub_error hubbub_tokeniser_run(hubbub_tokeniser *tokeniser);

#endif

