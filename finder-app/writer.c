#include <stdio.h>
#include <errno.h>
#include <syslog.h>

int main(int argc, char *argv[]){

    char * writefile  = argv[1];
    char * writestr  = argv[2];


// args number
// printf("%i\n", argc);
if (argc < 3)
{
    printf("Error: Not enough arguments specified\n");
    openlog ("writer", 0, LOG_USER);
syslog (LOG_ERR, "Not enough arguments specified");
closelog ();

    return 1;
}


// filename
// printf("%s\n", argv[0]);

// test argv
printf("Writefile: %s\n", writefile);
printf("Writestr: %s\n", writestr);


// file write
    FILE *fptr;
    fptr = fopen(writefile, "w");

    if (fptr == NULL){
        perror("Error: File not created.\n");
        printf("Errno: %d\n", errno);

            openlog ("writer", 0, LOG_USER);
syslog (LOG_ERR, "File not created.");
closelog ();
    return 1;
}

    
    fprintf(fptr, "%s", writestr);
    fclose(fptr);
    printf ("Writing %s to %s.\n", writestr, writefile);


openlog ("writer", 0, LOG_USER);
syslog (LOG_DEBUG, "Writing %s to %s.\n", writestr, writefile);
closelog ();



return 0;
}