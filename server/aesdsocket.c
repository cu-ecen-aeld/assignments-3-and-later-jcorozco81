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

#define BUFFER_SIZE 99999
#define PORT 9000
#define BACKLOG 5
#define FILEPATH "/var/tmp/aesdsocketdata"

int daemonize()
{

    // Daemon
    pid_t pid, sid;

    // Frist Fork
    pid = fork();
    printf("Daemon PID: %d\n", pid);

    if (pid < 0)
    {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    // New session
    if (setsid() == -1)
    {
        perror("setsid failed");
        return -1;
    }

    // Second Fork
    // pid = fork();
    // printf("Daemon PID (2): %d\n", pid);

    // if (pid < 0)
    // {
    //     perror("fork failed");
    //     exit(EXIT_FAILURE);
    // }
    // else if (pid > 0)
    // {
    //     exit(EXIT_SUCCESS);
    // }

    // Umask
    umask(0);

    // Change Dir
    if (chdir("/") == -1)
        return -1;

    // Clode FD
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    open("/dev/null", O_RDWR);
    dup(0);
    dup(0);
}

// Delete File Function

int delete_file()
{
    if (unlink(FILEPATH) == 0)
    {
        printf("File deleted successfully\n");
    }
    else
    {
        perror("Error deleting the file");
        return 1;
    }
    return 0;
}

// Signal Handler Function
void signal_handler(int signal)
{
    if (signal == SIGINT || signal == SIGTERM)
    {
        openlog("aesdsocket", 0, LOG_USER);
        syslog(LOG_INFO, "Caught signal, exiting");
        closelog();
    }
    delete_file();
    exit(0);
}

int client_handler(int cfd)
{
    int fd;
    char *buffer;
    char *rbuffer;
    ssize_t bytes_recv, bytes_read, send_result;
    ssize_t bytes_sent = 0;
    off_t fileSize;

    fd = open("/var/tmp/aesdsocketdata", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        perror("Error opening file");
        return -1;
    }

    buffer = (char *)malloc(BUFFER_SIZE);
    if (buffer == NULL)
    {
        perror("Malloc Failed");
        return -1;
    }

    if ((bytes_recv = recv(cfd, buffer, BUFFER_SIZE, 0)) == -1)
    {
        perror("recv error");
    }
    if ((write(fd, buffer, bytes_recv)) != bytes_recv)
    {
        perror("Write error");
        close(fd);
        return -1;
    }

    close(fd);

    // Send
    FILE *fptr;
    fptr = fopen("/var/tmp/aesdsocketdata", "r");
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

}

int main(int argc, char *argv[])
{

    int sockfd, cfd;
    struct sockaddr_in myaddr, peer_addr;
    socklen_t myaddr_size = sizeof(myaddr);
    socklen_t peeraddr_size = sizeof(peer_addr);
    int reuse = 1;

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    if (argc > 1 && strcmp(argv[1], "-d") == 0)
    {
        int daemonize();
        printf("%s Daemon Starting\n", argv[1]);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Socket failed.");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Socket created\n");
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
    {
        perror("setsockopt");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(PORT);
    myaddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&myaddr, myaddr_size) < 0)
    {
        perror("Bind failed.");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Socket bound to port: %d\n. ", PORT);

    if (listen(sockfd, BACKLOG) < 0)
    {
        perror("Listen failed.");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Connections

    while (1)
    {

        if ((cfd = accept(sockfd, (struct sockaddr *)&peer_addr, (socklen_t *)&peeraddr_size)) == -1)
        {
            perror("Connection failed.");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        printf("Connection accepted from %s.\n", inet_ntoa(myaddr.sin_addr));
        openlog("aesdsocket", 0, LOG_USER);
        syslog(LOG_INFO, "Accepted connection from %s", inet_ntoa(myaddr.sin_addr));
        closelog();

        pid_t pid = fork();
        // printf("PID(Client Handler): %d\n", pid);
        if (pid == -1)
        {
            perror("fork failed");
            close(cfd);
            continue;
        }
        if (pid == 0)
        {
            close(sockfd);
            int c_handler_status = client_handler(cfd);
            close(cfd);
            exit(EXIT_SUCCESS);
        }
        else
        {
            close(cfd);
        }
    }
    close(sockfd);

    return 0;
}
