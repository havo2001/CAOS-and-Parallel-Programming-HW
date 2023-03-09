#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>


int main (int argc, char ** argv) {
    int socket_fd;

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error in creating socket's file descriptor");
        exit(1);
    }

    struct sockaddr_in server_addr = {
            .sin_family = AF_INET,
            .sin_port = htons(8000),
            .sin_addr.s_addr = inet_addr("127.0.0.1")
    };

    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        perror("Error connecting to the server");
        exit(1);
    }

    char req[4096];
    memset(req, '\0', sizeof(req));

    snprintf(req, sizeof(req),
             "DELETE %s HTTP/1.1\n", argv[1]
    );

    send(socket_fd, req, strlen(req), 0);

    char receivement[4096];
    memset(receivement, '\0', sizeof(receivement));

    recv(socket_fd, receivement, sizeof(receivement), 0);
    printf("%s\n", receivement);

    close(socket_fd);
    return 0;
}
