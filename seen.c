#include<stdio.h>
#include<string.h>
#include"types.h"
#include"seen.h"



Status read_and_validate_view_args(char *argv[], _ViewInfo *view)
{

    if(argv[2]==NULL)
    {
        printf("Error");//help menu;
        return 0;
    }
    char *dot = strchr(argv[2],'.');
    if(dot==NULL||strcmp(dot,".mp3")!=0)
    {
        printf("it should be .mp3 file ");
        return 1;
    }
    view->mp3_file_nme = argv[2];
    view->fp_mp3 = fopen(view->mp3_file_nme,"rb");
    if(view->fp_mp3==NULL)
    {
        printf("file not open");
        return e_failure;
    }
    char buffer[3];
    if(fread(buffer,3,1,view->fp_mp3)==0)
    {
        printf("Error");//error msg;
        return e_failure;
    }
    if(strncmp(buffer,"ID3",3)!=0)
    {
        printf("Error");//error msg;
        return e_failure;
    }
    return e_success;

}
Status do_view(_ViewInfo*view)
{
    fseek(view->fp_mp3,10,SEEK_SET);      /* Move offset to 10th pos */

    
    for(int i=0;i<6;i++)
    {
        /* Read frame ID */
        if(fread(view->frame_id,4,1,view->fp_mp3) == 0){
            printf("Error : Unable to read Frame ID from MP3 file\n");
            return e_failure;
        }

        view->frame_id[4] = '\0';

        /* Read frame size */
        unsigned char buffer[4];

        if(fread(buffer,4,1,view->fp_mp3) == 0){
            printf("Error : Unable to read size of Frame from MP3 file\n");
            return e_failure;
        }

            /* Change endianess */
        view->frame_size = 0;

        for(int j=0;j<4;j++){
            view->frame_size = (view->frame_size << 8) | buffer[j];
        }

        
        fseek(view->fp_mp3,2,SEEK_CUR);       /* Moving offset by 2 pos */

        if(do_validate_frame(view->frame_id) == e_success)
        {
            printf("%d\t|\t%s\t|\t",i+1,view->frame_id);

            /* Printing the meta data */
            char data_buffer[view->frame_size + 1];

            if(fread(data_buffer,view->frame_size,1,view->fp_mp3) == 0){
                printf("Error : Unable to read Frame data\n");
                return e_failure;
            }

            data_buffer[view->frame_size] = '\0';

            printf("%s\n",data_buffer+1);
        }
        else
        {
            /* Skip unrecognised frame data */
            fseek(view->fp_mp3,view->frame_size,SEEK_CUR);
        }
    }
    // print_end_format();

    return e_success;
}

Status do_validate_frame(char frameid[]){
    if ((strcmp(frameid,"TPE1") == 0) ||
        (strcmp(frameid,"TIT2") == 0) ||
        (strcmp(frameid,"TALB") == 0) ||
        (strcmp(frameid,"TYER") == 0) ||
        (strcmp(frameid,"TCON") == 0) ||
        (strcmp(frameid,"COMM") == 0))
    {
        return e_success;
    }

    return e_failure;
}