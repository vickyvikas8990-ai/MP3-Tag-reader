#include<stdio.h>
#include<string.h>
#include "types.h"
#include "seen.h"
#include "edit.h"

void menu(){
    printf("1. -v -> to view mp3 file contens\n");
    printf("2. -3 -> to edit mp3 file contens\n");
    printf("\t2.1. -t -> to edit song title\n");
    printf("\t2.2. -a -> to edit artist name\n");
    printf("\t2.3. -A -> to edit ablbum name\n");
    printf("\t2.4. -y -> to edit year\n");
    printf("\t2.5. -m -> to edit content\n");
    printf("\t2.6. -c -> to edit comment\n");
}

char* get_frame(char ch){
    switch(ch){
        case 'a' : return "TPE1";
        case 't' : return "TIT2";
        case 'A' : return "TALB";
        case 'y' : return "TYER";
        case 'm' : return "TCON";
        case 'c' : return "COMM";
        default  : return NULL;
    }
}

Status validate_edit_tag(char ch){
    if(get_frame(ch) != NULL) 
       return e_success;

    return e_failure;
}

Status read_and_validate_edit_args(char *argv[],_ViewInfo *view)
{
    /* Validating edit option */
    if(validate_edit_tag(argv[2][1]) == e_failure){
        printf("Invalid edit option\n");
        return e_failure;
    }

    view->edit_frame = get_frame(argv[2][1]);   // Storing edit Frame ID 

    view->edit_data = argv[3];      // Storing edit data 

    
    /* Validate MP3 file name */
    char *dot = strchr(argv[4],'.');
    if((dot == NULL) || (strcmp(dot,".mp3") != 0)){
        printf("Error : File must be .mp3 file\n");
        return e_failure;
    }
    
    view->mp3_file_nme = argv[4];      // Storing mp3 file name

    view->fp_mp3 = fopen(view->mp3_file_nme,"r");

    if(view->fp_mp3 == NULL){
        printf("Error : MP3 file not opened\n");
        return e_failure;
    }
    
    return e_success;
}

Status do_edit(_ViewInfo *view)
{
    if(create_file(view) == e_failure){
        printf("Error : File not opened\n");
        return e_failure;
    }

    if(copy_header(view) == e_failure){
        printf("Error : Header not copied\n");
        return e_failure;
    }

    
    while(1){ 
        char tag_buffer[5];     // Tag ID copy 
        
        if(fread(tag_buffer,4,1,view->fp_mp3) == 0){
            printf("Error : Unable to read tag\n");
            return e_failure;
        }

        tag_buffer[4] = '\0';
        
        /* if tag is matched */
        if(strcmp(tag_buffer,view->edit_frame) == 0)
        {
            if(edit_tag(view,tag_buffer) == e_failure){
                printf("Error : Unable to edit tag data\n");
                return e_failure;
            }

            break;
        }
        else        // tag is not matched
        {
            if(copy_data(view,tag_buffer) == e_failure){
                printf("Error : Unable to copy data\n");
                return e_failure;
            }
        }
    }

    int ch;
    while((ch = fgetc(view->fp_mp3)) != EOF){
        fwrite(&ch,1,1,view->fptr_temp_mp3);
    }

    fclose(view->fp_mp3);
    fclose(view->fptr_temp_mp3);


    /* Deleting the main file */
    if(remove(view->mp3_file_nme) != 0){
        printf("Error : Unable to change main file name\n");
        return e_failure;
    }

    /* Renaming the temp file to main file */
    if(rename(view->temp_mp3_fname,view->mp3_file_nme) != 0){
        printf("Error : Unable to change name file name\n");
        return e_failure;
    }

    return e_success;
}

Status create_file(_ViewInfo *view)
{
    /* Creating temp.mp3 file */
    view->temp_mp3_fname = "temp.mp3";

    view->fptr_temp_mp3 = fopen(view->temp_mp3_fname,"w");

    if(view->fptr_temp_mp3 == NULL){
        printf("Error : Temp file not opened\n");
        return e_failure;
    }

    return e_success;
}

Status copy_header(_ViewInfo *view)
{
    rewind(view->fp_mp3);    // Bring cursor back to 0th byte in song.mp3 file

    /* Copying header to temp.mp3 */
    char header_buffer[10];

    if(fread(header_buffer,10,1,view->fp_mp3) == 0){
        printf("Error : Header not copied\n");
        return e_failure;
    }

    if(fwrite(header_buffer,10,1,view->fptr_temp_mp3) == 0){
        printf("Error : Unable to write header in temp file\n");
        return e_failure;
    }

    return e_success;
}

Status edit_tag(_ViewInfo *view, char tag_buffer[])
{
    /* Copy the tag */
    if(fwrite(tag_buffer,4,1,view->fptr_temp_mp3) == 0){
        printf("Error : Tag not copied\n");
        return e_failure;
    }

    /* Copying the size of meta data */
    view->new_frame_size = strlen(view->edit_data) + 1;

    /* Copy size in temp file */
    char size_buffer[4];
    uint size = view->new_frame_size;
    for(int i=3;i>=0;i--){          // Converting integer to big endian
        size_buffer[i] = size & 0xFF;
        size >>= 8;
    }
    
    if(fwrite(size_buffer,4,1,view->fptr_temp_mp3) == 0){
        printf("Error : Unable to copy size of meta data to temp file\n");
        return e_failure;
    }

    /* Storing old size of metadata */
    char buffer[4];
    if(fread(buffer,4,1,view->fp_mp3) == 0)
        return e_failure;

    big_endian_to_integer(buffer,view);

    /* Copying flag + '\0' */

    char flag_buffer[3];
    if(fread(flag_buffer,3,1,view->fp_mp3) == 0){
        return e_failure;
    }

    if(fwrite(flag_buffer,3,1,view->fptr_temp_mp3) == 0){
        printf("Error : Unable to copy flag in temp file\n");
        return e_failure;
    }

    if(fwrite(view->edit_data,view->new_frame_size - 1, 1, view->fptr_temp_mp3) == 0){
        printf("Error : Unable to copy new data to temp file\n");
        return e_failure;
    }

    /* moving the cursor */
    fseek(view->fp_mp3,view->frame_size - 1, SEEK_CUR);

    return e_success;
}

Status copy_data(_ViewInfo *view, char tag_buffer[])
{
    /* Copying the tag */
    if(fwrite(tag_buffer,4,1,view->fptr_temp_mp3) == 0){
        printf("Error : Tag not copied\n");
        return e_failure;
    }

    /* Copy size of metadata */
    char size_buffer[4];
    if(fread(size_buffer,4,1,view->fp_mp3) == 0){
        printf("Error : Unable to read frame size\n");
        return e_failure;
    }

    if(fwrite(size_buffer,4,1,view->fptr_temp_mp3) == 0){
        printf("Error : Unable to copy size in temp file\n");
        return e_failure;
    }

    big_endian_to_integer(size_buffer,view);       //Convert big-endian bytes to integer

    /* Copying flag */
    char flag_buffer[2];
    if(fread(flag_buffer,2,1,view->fp_mp3) == 0){
        printf("Error : Unable to read flags\n");
        return e_failure;
    }

    if(fwrite(flag_buffer,2,1,view->fptr_temp_mp3) == 0){
        printf("Error : Unable to copy flag in temp file\n");
        return e_failure;
    }

    /* copying meta data */
    char data_buffer[view->frame_size];
    if(fread(data_buffer,view->frame_size,1,view->fp_mp3) == 0){
        printf("Error : Unable to read meta data of [%s]\n",tag_buffer);
        return e_failure;
    }

    if(fwrite(data_buffer,view->frame_size,1,view->fptr_temp_mp3) == 0){
        printf("Error : Unable to copy meta data in temp file\n");
        return e_failure;
    }

    return e_success;
}

Status big_endian_to_integer(char buffer[], _ViewInfo *view)
{
    view->frame_size = 0;

    for(int i=0;i<4;i++){
        view->frame_size = (view->frame_size << 8) | (unsigned char)buffer[i];
    }

    return e_success;
}