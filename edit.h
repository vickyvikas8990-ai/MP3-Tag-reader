#ifndef EDIT_H
#define EDIT_H

#include "seen.h"

/* Edit Function prototype*/

/* Menu */
void menu();

/* Get frame id */
char* get_frame(char ch);

/* Tag Validation */
Status validate_edit_tag(char ch);

/* Read and validate edit args from argv */
Status read_and_validate_edit_args(char *argv[],_ViewInfo *view);

/* Perform the edit */
Status do_edit(_ViewInfo *view);

/* Creating temp.mp3 file */
Status create_file(_ViewInfo *view);

/* Copying the header to temp file */
Status copy_header(_ViewInfo *view);

/* Edit the tag data and size */
Status edit_tag(_ViewInfo *view, char tag_buffer[]);

/* Copy the data */
Status copy_data(_ViewInfo *view, char tag_buffer[]);

/* Reading value in Big endian format */
Status big_endian_to_integer(char buffer[], _ViewInfo *view);

#endif 