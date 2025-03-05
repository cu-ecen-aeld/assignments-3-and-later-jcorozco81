#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <signal.h>

#include <errno.h>

#include <syslog.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <fcntl.h>

#include <linux/fs.h>
#include <linux/limits.h>

#define BUFFER_SIZE 4096
#define PORT 9000
#define BACKLOG 5


int runasdaemon(){

    // Daemon
pid_t pid, sid;

// Create new process
pid = fork();
printf("PID: %d", pid);

if (pid == -1){
    perror("fork failed");
    exit(EXIT_FAILURE);
}
else if (pid != 0){
    exit(EXIT_SUCCESS);
}

if (setsid() == -1){
perror("setsid failed");
return -1;
}

if (chdir ("/") == -1)
return -1;

close (STDIN_FILENO);
close (STDOUT_FILENO);
close (STDERR_FILENO);

open("/dev/null", O_RDWR);
dup (0);
dup (0);

}






int main(int argc, char *argv[]){

    if (argc > 1 && strcmp(argv[1], "-d")== 0){
    int runasdaemon();
    printf("%s Daemon Starting\n", argv[1]);
    }



int sockfd, cfd;
struct sockaddr_in myaddr, peer_addr;
socklen_t myaddr_size = sizeof(myaddr);
socklen_t peeraddr_size = sizeof(peer_addr);
ssize_t bytes_recv, bytes_read;
char *buffer;
char *rbuffer;
char writefile[30] = "/var/tmp/aesdsocketdata";
off_t fileSize;



void signal_handler(int signal_number){
    if ( signal_number == SIGINT ){
        openlog ("aesdsocket", 0, LOG_USER);
        syslog (LOG_INFO, "Caught signal, exiting");
        closelog();

        close(sockfd);
if (unlink(writefile)==0){
    printf("File deleted successfully\n");
   
}
else{
    perror("Error deleting the file");
    }
 
    }

    exit(0);

    }












   sockfd = socket(AF_INET, SOCK_STREAM, 0);
if (sockfd ==-1){
    perror("Socket failed.");
    return -1;
}
else{
    printf ("Socket created\n");
}

    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(PORT);
    myaddr.sin_addr.s_addr = INADDR_ANY;

if (bind(sockfd, (struct sockaddr *) &myaddr, myaddr_size) == -1)
{
    perror("Bind failed.");
    exit(EXIT_FAILURE);
}

    printf ("Socket bound to port: %d\n", PORT);
    printf ("Listening...");






if (listen(sockfd, BACKLOG) == -1){
    perror("listen failed.");
}    exit(EXIT_FAILURE);






// Connections

while(1){


if(signal(SIGINT, signal_handler) == SIG_ERR){
    perror("Signal Error");
    return 1;
}

else if(signal(SIGTERM, signal_handler) == SIG_ERR){
    perror("Signal Error");
    return 1;
}



cfd = accept(sockfd, (struct sockaddr *) &peer_addr, (socklen_t*)&peeraddr_size);
if (cfd ==-1){
    perror("Connection failed.");
    continue;

}
else{
    printf ("Connection accepted.\n");
    openlog ("aesdsocket", 0, LOG_USER);
    syslog (LOG_INFO, "Accepted connection from %s", inet_ntoa(myaddr.sin_addr));
    closelog ();
}







FILE *fptr;
fptr = fopen(writefile, "a+");

if (fptr == NULL){
    perror("Error: File not created.\n");
    printf("Errno: %d\n", errno);

return 1;
}


while((bytes_recv = recv(cfd, buffer, BUFFER_SIZE-1, 0))>0);
buffer [bytes_recv] = '\0';
{
fwrite(buffer, 1, bytes_recv, fptr);
}
if (bytes_recv == -1){
    perror("Error: recv");
}
else{
    printf ("Data %s received.\n", buffer);
}
fclose(fptr);


fptr = fopen(writefile, "r");
if (fptr == NULL){
    perror("Error opening file.\n");
    printf("Errno: %d\n", errno);

return 1;
}

while((bytes_read = fread(rbuffer, 1, BUFFER_SIZE, fptr))> 0){
    if (send(cfd, rbuffer, bytes_read, 0)<0){
        perror("Error sending data");
    }
}

fclose(fptr);






close(cfd);





}





return 0;


}

