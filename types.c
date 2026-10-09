#include<stdio.h>
#include "types.h"
#include "seen.h"
#include "edit.h"

int main(int argc,char *argv[]){

    _ViewInfo view;

    if(argc < 2){
        printf("./a.out -v song.mp3\n");
        printf("./a.out -h\n");
        printf("./a.out -e -t/-a/-A/-m/-y/-c ""new data"" song.mp3\n");
        return e_failure;
    }

    OperationType op = check_operation_type(argv[1][1]);

    if(op == e_view)
    {
        if(argc != 3){
            printf("Invalid Input\n");
            return e_failure;
        }

        if(read_and_validate_view_args(argv,&view) == e_failure)
            return e_failure;
        else{
            if(do_view(&view) == e_success)
                printf("View is success\n");
        }
    }

    else if(op == e_edit)
    {    
        if(argc != 5){
            printf("Invalid Input\n");
            return e_failure;
        }

        if(read_and_validate_edit_args(argv,&view) == e_failure)
            return e_failure;
        else{
            if(do_edit(&view) == e_success)
                printf("Edit is success\n");
        }
    }

    else if(op == e_help){
        menu();
    }

    else{
        printf("Invalid operation\n");
        return e_failure;
    }    
}


OperationType check_operation_type(char ch){
    if(ch == 'v')
        return e_view;
    else if(ch == 'e')
        return e_edit;
    else if(ch == 'h')
        return e_help;    
    else
        return e_unsupported;
}