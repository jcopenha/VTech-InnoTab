/*****************************************************************
|
|   File Input Module
|
|   (c) 2002-2006 Gilles Boccon-Gibod
|   Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

#ifndef _BLT_FILE_INPUT_H_
#define _BLT_FILE_INPUT_H_

/**
 * @ingroup plugin_modules
 * @defgroup file_input_module File Input Module 
 * Plugin module that creates media nodes that read files.
 * This module will respond to probes with names that have the 
 * following syntax: 
 * file:<filename>
 * The module will also respond to probes with unqualified names, but
 * with a low matching score, to that it will be selected for 'naked'
 * names only if no other module responds to the probe with a higher 
 * matching score.
 * If no mime-type is explicitely set, this module will try to guess the
 * mime type based on the file extension, using the registered file 
 * extensions.
 * @{ 
 */

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "BltTypes.h"
#include "BltModule.h"

/*----------------------------------------------------------------------
|   module
+---------------------------------------------------------------------*/
BLT_Result BLT_FileInputModule_GetModuleObject(BLT_Module** module);

/** @} */

#endif /* _BLT_FILE_INPUT_H_ */
