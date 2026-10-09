#include <stdio.h>
#include<string.h>
 #include "seen.h"
//#include "decode.h"
#include "types.h"

int main(int argc,char *argv[])
{
    if(argc!=3)
    {
        printf("Invalid : Error");
        return 1;
    }
    _ViewInfo view;
        OperationType opt=(check_operation_type(argv[1][1])); 
        if(opt==e_view)
        {
           if(read_and_validate_view_args(argv,&view)==e_failure)
            {
            // printf("error");//error msg;
                return e_failure;
             }
             else{
                if(do_view(argv,&view)==e_success)
                printf("View is Succesfully complete");
             }
         }
        
    

}
OperationType check_operation_type(char opt)
{
    if(opt=='v')
    {
        return e_view;
    }
    else if(opt=='e')
    {
        return e_edit;
    }
    else if(opt=='h')
    {
        return e_help;
    }
    else
    return e_unsupported;
}