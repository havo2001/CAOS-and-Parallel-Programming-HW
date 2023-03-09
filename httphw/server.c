#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <dirent.h>

const size_t MAX_LENGTH = 8192;
char current_directory[8192];
int client_fd = -1;
int directory_fd = -1;
int socket_fd = -1;
char *ok_response = "HTTP/1.1 200 OK";
char *conflict_response = "HTTP/1.1 409 Conflict";
char *not_found_response = "HTTP/1.1 404 Not Found";
char *server_name = "Server: caos-http-server";
char command[8192];
char path[8192];
char predecessor[8192];
char path_name[8192];

void handle_path() {
    if (strcmp(path, "/") == 0) {
        memset(path, '\0', sizeof(path));
        strcpy(path, ".");
    } else {
        memmove(path, path + 1, strlen(path));
    }
}

void get_path_name() {
    memset(path_name, '\0', sizeof(path_name));
    strcpy(path_name, current_directory);
    strcat(path_name, path);
}

void get_predecessor() {
    strcpy(predecessor, current_directory);
    strcat(predecessor, path);
    int i = strlen(predecessor) - 1;
    while (i >= 1) {
        if (predecessor[i] == '/') {
            break;
        } else {
            predecessor[i] = '\0';
            i--;
        }
    }
};

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

int remove_directory(const char *target) {
    DIR *dir = opendir(target);
    if (dir) {
        struct dirent *dire;
        while ((dire = readdir(dir)) != NULL) {
            if (!strcmp(".", dire->d_name) || !strcmp("..", dire->d_name))
                continue;
            int (*const remove_function) (const char*) = dire->d_type == DT_DIR ? remove_directory : remove;
            if (remove_function(dire->d_name)) {
                closedir(dir);
                return -1;
            }
        }
        if (closedir(dir)) {
            return -1; 
        }
    }
   return remove(target);
}

void get() {
    char response[MAX_LENGTH];
    memset(response, '\0', sizeof(response));
    int fd = openat(directory_fd, path, O_RDONLY);
    // The current file exists
    if (fd != -1) {
        struct stat file_stat;
        fstatat(directory_fd, path, &file_stat, AT_SYMLINK_NOFOLLOW);
        if(S_ISDIR(file_stat.st_mode)) {
            char second_response[65536];
            memset(second_response, '\0', sizeof(second_response));
            DIR *dir = fdopendir(fd);
            struct dirent *dire;
            strcat(second_response, "DIRECTORY\r\n");
            if (dir) {
                while ((dire = readdir(dir)) != NULL) {
                    if (strcmp(dire->d_name, ".") != 0 && strcmp(dire->d_name, "..") != 0) {
                        strcat(second_response, dire->d_name);
                        if (dire->d_type == DT_DIR) {
                            strcat(second_response, "/");
                        }
                        strcat(second_response, "\r\n");
                    }
                }
                closedir(dir);
            }
            sprintf(response, "%s\r\n%s\r\nContent-Type: text/plain\r\nContent-Length: %lu\r\n\r\n",
            ok_response, server_name, strlen(second_response));
            write(client_fd, response, strlen(response));
            write(client_fd, second_response, strlen(second_response));
        } else {
            sprintf(response, "%s\r\n%s\r\nContent-Type: text/plain\r\nContent-Length: %ld\r\n\r\nREGULAR FILE\n",
            ok_response, server_name, file_stat.st_size);
            write(client_fd, response, strlen(response));
            sendfile(client_fd, fd, NULL, file_stat.st_size);
        }
    }
    else {
        sprintf(response, "%s\r\n%s\r\n\r\n", not_found_response, server_name);
        write(client_fd, response, strlen(response));     
    }
    close(fd);
}


void post() {
    char response[MAX_LENGTH];
    memset(response, '\0', sizeof(response));
    DIR *dir = opendir(predecessor);
    if (dir) {
        int fd = openat(directory_fd, path, O_RDONLY);
        if (fd != -1) {
            sprintf(response, "%s\r\n%s\r\n\r\n", conflict_response, server_name);
        }
        else {
            FILE *temp = fopen(path, "w");
            int begin = strlen(command) - strlen(strstr(command, "\r\n\r\n"));
            begin += 4;
            while (begin < strlen(command)) {
                fputc(command[begin], temp);
                ++begin;
            }
            fclose(temp);
            sprintf(response, "%s\r\n%s\r\n", ok_response, server_name);
        }
        close(fd);
        closedir(dir);
    }
    else {
        sprintf(response, "%s\r\n%s\r\n\r\n", not_found_response, server_name);
    }
    write(client_fd, response, strlen(response));
}

void put() {
    char response[MAX_LENGTH];
    memset(response, '\0', sizeof(response));
    DIR *dir = opendir(predecessor);
    if (dir) {
        int fd = openat(directory_fd, path, O_RDONLY);
        if (fd != -1) {
            sprintf(response, "%s\r\n%s\r\n\r\n", conflict_response, server_name);         
        }
        else {
            mkdir(path, 0700);
            sprintf(response,"%s\r\n%s\r\n", ok_response, server_name);
        }
        close(fd);
        closedir(dir);
    } else {
        sprintf(response, "%s\r\n%s\r\n\r\n", not_found_response, server_name);
    }
    write(client_fd, response, strlen(response));
}

void delete() {
    char response[MAX_LENGTH];
    memset(response, '\0', sizeof(response));
    int fd = openat(directory_fd, path, O_RDONLY);
    if (fd != -1) {
        get_path_name();   
        struct stat file_stat;
        fstatat(directory_fd, path, &file_stat, AT_SYMLINK_NOFOLLOW);
        if (S_ISDIR(file_stat.st_mode)) {
            DIR *dir = opendir(path_name);
            struct dirent *dire;
            if (dir) {
                while ((dire = readdir(dir)) != NULL) {
                    char copy[MAX_LENGTH];
                    memset(copy, '\0', sizeof(copy));
                    strcpy(copy, current_directory);
                    strcat(copy, path);
                    if (strcmp(dire->d_name, ".") != 0 && strcmp(dire->d_name, "..") != 0) {
                        strcat(copy, "/");
                        strcat(copy, dire->d_name);
                        if (dire->d_type == DT_DIR) {
                            remove_directory(copy);
                        } else {
                            unlink(copy);
                        }
                    }
                }
                closedir(dir);
            }
            rmdir(path_name);
        }
        else {
            if (unlink(path_name) != 0) {
                return;
            }
        }
        sprintf(response, "%s\r\n%s\r\n", ok_response, server_name);
       
    }else {
        sprintf(response, "%s\r\n%s\r\n\r\n", not_found_response, server_name);
    }  
    write(client_fd, response, strlen(response));
    close(fd);
}


void process_client () {
    memset(command, '\0', sizeof(command));
    read(client_fd, command, 65536);
    int begin = strlen(command) - strlen(strchr(command, ' '));
    int end = strlen(command) - strlen(strstr(command, "HTTP/1.1"));
    memset(path, '\0', sizeof(path));
    memcpy(path, command + begin + 1, end - begin - 2);
    handle_path();
    memset(predecessor, '\0', sizeof (predecessor));
    get_predecessor();  
    if (strncmp(command, "GET", 3) == 0) {
        get();
    }
    else if (strncmp(command, "POST", 4) == 0) {
        post();
    }
    else if (strncmp(command, "PUT", 3) == 0) {
        put();
    }
    else if (strncmp(command, "DELETE", 6) == 0){
        delete();
    }
}

int main (int argc, char **argv) {
    attach_signal_listeners();
    memset(current_directory, '\0', sizeof(current_directory));
    strcpy(current_directory, argv[2]);
    unsigned port = strtoul(argv[1], NULL, 0);
    directory_fd = open(current_directory, O_RDONLY | O_DIRECTORY);
    if (current_directory[strlen(current_directory) - 1] != '/') {
        strcat(current_directory, "/");
    }

    if (directory_fd < 0) {
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
    int set_sock_opt_value = 1;
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &set_sock_opt_value, sizeof(set_sock_opt_value));
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &set_sock_opt_value, sizeof(set_sock_opt_value));

    struct sockaddr_in server_address = {
            .sin_family = AF_INET,
            .sin_addr.s_addr = INADDR_ANY,
            .sin_port = htons(port)
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

	if (client_fd < 0) {
		perror("Error in accept");
		shutdown_server();
	} else {
		printf("Server accepted client\n");
	}
	printf("Client connected\n");
	process_client();
	printf("Client disconnected\n\n");
	shutdown(client_fd, SHUT_RDWR);
	close(client_fd);
}
