#ifndef SEEN_H
#define SEEN_H
#include<stdio.h>
#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

//#define MAX_SECRET_BUF_SIZE 1
// #define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_HEADER 5

typedef struct _ViewInfo
{
    char*mp3_file_nme;
    FILE*fp_mp3;
    

    char frame_id[MAX_FILE_HEADER];
    uint frame_size;

    /* Edit info */
    char *edit_frame;
    char *edit_data;
    char *temp_mp3_fname;
    FILE *fptr_temp_mp3;
    uint new_frame_size;


} _ViewInfo;


/* Encoding function prototype */

/* Check operation type */
OperationType check_operation_type(char opt);

/* Read and validate Encode args from argv */
Status read_and_validate_view_args(char *argv[], _ViewInfo *view);

/* Perform the encoding */
Status do_view(_ViewInfo*view);
Status do_validate_frame(char frameid[]);

 //Get File pointers for i/p and o/p files 
// Status open_files(_ViewInfo *view);
#endif