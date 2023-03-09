#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/sendfile.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>


const size_t MAX_LENGTH = 4096;
const size_t BODY_LINE_MAX_LENGTH = 65536;
int socket_fd = -1;
int client_fd = -1;
int directory_fd = -1;
char workdir[256];
char *server_header = "Server: caos-http-server";

// Additional functions:
int remove_recursive(const char *path) {
    DIR *dir = opendir(path);
    if (dir) {
        struct dirent *diren;
        while ((diren=readdir(dir))) {
            if (!strcmp(".", diren->d_name) || !strcmp("..", diren->d_name))
                continue;
            char file_name[strlen(path) + strlen(diren->d_name) + 2];
            int (*const remove_func) (const char*) = diren->d_type == DT_DIR ? remove_recursive : remove;
            if (remove_func(diren->d_name)) {
                closedir(dir);
                return -1;
            }
        }
        if (closedir(dir)) {
            return -1; 
        }
    }
   return remove(path);
}



// 4 main functions GET, POST, PUT, DELETE

// GET
void get(char *file_name) {
    char response_header[MAX_LENGTH];
    memset(response_header, '\0', sizeof(response_header));
    int file_fd = openat(directory_fd, file_name, O_RDONLY);


    if (file_fd == -1 && errno != EACCES) {
        // File or directory doesn't exist
        sprintf(response_header, "HTTP/1.1 404 Not Found\r\n%s\r\n\r\n",server_header);
        write(client_fd, response_header, strlen(response_header));
    }
    else {
        // File or directory exists
        struct stat file_stat;
        fstatat(directory_fd, file_name, &file_stat, AT_SYMLINK_NOFOLLOW);

        if(S_ISDIR(file_stat.st_mode)) {
            char response_content[BODY_LINE_MAX_LENGTH];
            memset(response_content, '\0', sizeof(response_content));

            DIR *dir = fdopendir(file_fd);
            struct dirent *directory_content;

            strcat(response_content, "DIRECTORY\r\n");
            
            // Take the content of directory
            if (dir) {
                while ((directory_content = readdir(dir)) != NULL) {
                    if (strcmp(directory_content->d_name, ".") != 0 && strcmp(directory_content->d_name, "..") != 0) {
                        strcat(response_content, directory_content->d_name);
                        if (directory_content->d_type == DT_DIR) {
                            strcat(response_content, "/");
                        }
                        strcat(response_content, "\r\n");
                    }
                }
                closedir(dir);
            }

            sprintf(response_header, "HTTP/1.1 200 OK\r\n%s\r\nContent-Type: text/plain\r\nContent-Length: %lu\r\n\r\n"
            , server_header, strlen(response_content)
            );

            write(client_fd, response_header, strlen(response_header));
            write(client_fd, response_content, strlen(response_content));
        } else {
            sprintf(response_header,
                    "HTTP/1.1 200 OK\r\n%s\r\nContent-Type: text/plain\r\nContent-Length: %ld\r\n\r\nREGULAR FILE\n",
                    server_header,file_stat.st_size
                    );

            write(client_fd, response_header, strlen(response_header));
            sendfile(client_fd, file_fd, NULL, file_stat.st_size);
        }
    }
    close(file_fd);
}


// POST
void post(char *path_name, char* direction) {
    char response_header[MAX_LENGTH];
    memset(response_header, '\0', sizeof(response_header));
    char parent[MAX_LENGTH];
    memset(parent, '\0', sizeof (parent));
    
    // Get the parent directory of current file
    strcpy(parent, workdir);
    strcpy(parent, path_name);
    unsigned it = strlen(parent) - 1;
    while (parent[it] != '/' && it >= 1) {
        parent[it] = '\0';
        it--;
    }

    DIR *dir = opendir(parent);

    if (dir) {
        int fd = openat(directory_fd, path_name, O_RDONLY);
        if (fd == -1 && ENOENT == errno) {
            FILE *file = fopen(path_name, "w");
            unsigned it = 0;
            while (it < strlen(direction)) {
                if (direction[it] == '\n' && direction[it + 1] == '\n') {
                    break;
                }
                ++it;
            }
            it += 2;
            while (it < strlen(direction)) {
                fputc(direction[it], file);
                ++it;
            }
            fclose(file);
            // Post successfully
            sprintf(response_header, "HTTP/1.1 200 OK\r\n%s\r\n", server_header);
        }
        else {
            // Conflict error 409: Directory has already existed
            sprintf(response_header, "HTTP/1.1 409 Conflict\r\n%s\r\n\r\n", server_header);
        }
        closedir(dir);
    }
    else {
        // The directory doesn't exist
        sprintf(response_header, "HTTP/1.1 404 Not Found\r\n%s\r\n\r\n", server_header);
    }

    write(client_fd, response_header, strlen(response_header));
}

// PUT
void put(char *path_name) {
    char response_header[MAX_LENGTH];
    memset(response_header, '\0', sizeof(response_header));
    char parent[MAX_LENGTH];
    memset(parent, '\0', sizeof (parent));
    
    // Get the parent directory of current file
    strcpy(parent, workdir);
    strcpy(parent, path_name);
    unsigned it = strlen(parent) - 1;
    while (parent[it] != '/' && it >= 1) {
        parent[it] = '\0';
        it--;
    }

    DIR *dir = opendir(parent);

    if (dir) {
        int fd = openat(directory_fd, path_name, O_RDONLY);
        if (fd == -1 && ENOENT == errno) {
            mkdir(path_name, 0700);
            // Put successfully
            sprintf(response_header,"HTTP/1.1 200 OK\r\n%s\r\n",server_header);
        }
        else {
            // Conflict error 409: Directory has already existed
            sprintf(response_header, "HTTP/1.1 409 Conflict\r\n%s\r\n\r\n", server_header);
        }
        closedir(dir);
    } else {
        // The directory doesn't exist
        sprintf(response_header, "HTTP/1.1 404 Not Found\r\n%s\r\n\r\n", server_header);
    }
    write(client_fd, response_header, strlen(response_header));
}

// DELETE
void delete(char *file_name) {
    char response_header[MAX_LENGTH];
    memset(response_header, '\0', sizeof(response_header));

    int file_fd = openat(directory_fd, file_name, O_RDONLY);
    if (file_fd == -1) {
        sprintf(response_header, "HTTP/1.1 404 Not Found\r\n%s\r\n\r\n", server_header);
    }else {
        // Finding the directory
        char path[MAX_LENGTH];
        memset(path, '\0', sizeof(path));
        strcpy(path, workdir);
        strcat(path, file_name);

        struct stat file_stat;
        fstatat(directory_fd, file_name, &file_stat, AT_SYMLINK_NOFOLLOW);

        if (S_ISDIR(file_stat.st_mode)) {
            DIR *dir = opendir(path);
            struct dirent *diren;
            if (dir) {
                while ((diren = readdir(dir)) != NULL) {
                    char temp[MAX_LENGTH];
                    memset(temp, '\0', MAX_LENGTH);
                    strcpy(temp, workdir);
                    strcat(temp, file_name);

                    if (strcmp(diren->d_name, ".") != 0 && strcmp(diren->d_name, "..") != 0) {
                        if (diren->d_type == DT_DIR) {
                            strcat(temp, "/");
                            strcat(temp, diren->d_name);
                            remove_recursive(temp);
                        } else {
                            strcat(temp, "/");
                            strcat(temp, diren->d_name);
                            unlink(temp);
                        }
                    }
                }
                closedir(dir);
            }
            rmdir(path);
        }
        else {
            if (unlink(path) != 0) {
                return;
            }
        }
        sprintf(response_header, "HTTP/1.1 200 OK\r\n%s\r\n", server_header);
    }  
    write(client_fd, response_header, strlen(response_header));
    close(file_fd);
}

// Receive request from client and process
void process_client () {
    char direction[BODY_LINE_MAX_LENGTH];
    memset(direction, '\0', sizeof(direction));
    read(client_fd, direction, BODY_LINE_MAX_LENGTH);

    int begin = strlen(direction) - strlen(strchr(direction, ' '));
    int end = strlen(direction) - strlen(strstr(direction, "HTTP/1.1"));
    char path[end - begin - 1];
    memset(path, '\0', sizeof(path));
    memcpy(path, direction + begin + 1, end - begin - 2);
    
    if (strcmp(path, "/") == 0) {
        memset(path, '\0', sizeof(path));
        strcpy(path, ".");
    } else {
        memmove(path, path + 1, strlen(path));
    }

    // GET, POST, PUT, DELETE
    if (direction[0] == 'G') {
        get(path);
    }
    else if (direction[0] == 'P' && direction[1] == 'O') {
        post(path, direction);
    }
    else if (direction[0] == 'P' && direction[1] == 'U') {
        put(path);
    }
    else {
        delete(path);
    }
}

void shutdown_server () {
    printf("Server is shutting down...\n");

    if (client_fd != -1) {
        shutdown(client_fd, SHUT_RDWR);
        close(client_fd);
    }

    if (socket_fd != -1) {
        shutdown(socket_fd, SHUT_RDWR);
        close(socket_fd);
    }

    if (directory_fd != -1) {
        close(directory_fd);
    }
    exit(0);
}


void handle_sigint_sigterm (int signum) {
    shutdown_server();
}


void attach_signal_listeners () {
    signal(SIGPIPE, SIG_IGN);

    struct sigaction action_int;
    memset(&action_int, 0, sizeof(action_int));
    action_int.sa_handler = handle_sigint_sigterm;
    action_int.sa_flags = SA_RESTART;
    sigaction(SIGINT, &action_int, NULL);

    struct sigaction action_term;
    memset(&action_term, 0, sizeof(action_term));
    action_term.sa_handler = handle_sigint_sigterm;
    action_term.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &action_term, NULL);
}



int main (int argc, char **argv) {
    attach_signal_listeners();

    memset(workdir, '\0', sizeof(workdir));
    strcpy(workdir, argv[2]);

 
    directory_fd = open(workdir, O_RDONLY | O_DIRECTORY);

    if (workdir[strlen(workdir) - 1] != '/') {
        strcat(workdir, "/");
    }

    if (directory_fd == -1) {
        perror("Error in open");
        shutdown_server();
    } else {
        printf("Directory opened successfully\n");
    }
    

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd == -1) {
        perror("Error in socket");
        shutdown_server();
    } else {
        printf("Socket created successfully\n");
    }
    

    int setsockopt_value = 1;
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &setsockopt_value, sizeof(setsockopt_value));
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &setsockopt_value, sizeof(setsockopt_value));

    struct sockaddr_in server_address = {
            .sin_family = AF_INET,
            .sin_addr.s_addr = INADDR_ANY,
            .sin_port = htons(atoi(argv[1]))
    };

    if (-1 == bind(socket_fd, (const struct sockaddr*)(&server_address), sizeof(server_address))) {
        perror("Error in bind");
        shutdown_server();
    } else {
        printf("Server binded successfully\n");
    }

    if (-1 == listen(socket_fd, SOMAXCONN)) {
        perror("Error in listen");
        shutdown_server();
    } else {
        printf("Server listening\n");
    }

    struct sockaddr_in client_address;
    socklen_t client_address_length;
    client_fd = accept(socket_fd, (struct sockaddr*)(&client_address), &client_address_length);

	if (client_fd == -1) {
		perror("Error in accept");
		shutdown_server();
	} else {
		printf("Server accepted client\n");
	}

	printf("Client connected\n");
    // Process client here we run GET, POST, PUT, DELETE
	process_client();
	printf("Client disconnected\n\n");

	shutdown(client_fd, SHUT_RDWR);
	close(client_fd);
}
