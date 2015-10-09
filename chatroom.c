#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>
#include "arraylist.h"
#include "linkedlist.h"
#define BUF_SIZE 1024

void broadcast_message(char* message, int count, int threadno);

static int num_threads;
static pthread_mutex_t lock;
static arraylist* messages;
static arraylist* names;
static linkedlist* openspaces;

void wait(int fd, int num_millisecs) {
    struct timeval tv;
    tv.tv_sec = num_millisecs / 1000;
    tv.tv_usec = (num_millisecs % 1000) * 1000;
    fd_set set;
    FD_ZERO(&set);
    FD_SET(fd, &set);
    select(fd + 1, &set, NULL, NULL, &tv);
}

int create_thread_variables(char* name) {
    pthread_mutex_lock(&lock);
    int threadno;
    arraylist* thread_messages = arraylist_init(sizeof(char*), 1);
    char* name_buf = malloc(BUF_SIZE);
    if (linkedlist_size(openspaces)) {
        threadno = *(int*)linkedlist_rmend(openspaces);
        arraylist_set(messages, threadno, &thread_messages); 
        arraylist_set(names, threadno, &name_buf);
    }
    else {
        threadno = num_threads;
        arraylist_addEnd(messages, &thread_messages);
        arraylist_addEnd(names, &name_buf);
    }
    num_threads++;
    pthread_mutex_unlock(&lock);
    return threadno;
}

void delete_thread_variables(int threadno) {
    pthread_mutex_lock(&lock);
    num_threads--;
    arraylist* my_messages = *(arraylist**)arraylist_get(messages, threadno);
    arraylist_free(my_messages);
    void* null = NULL;
    arraylist_set(messages, threadno, &null);
    linkedlist_addfront(openspaces, &threadno); 
    pthread_mutex_unlock(&lock);
}

void graceful_exit(int threadno, char* name) {
    char buf[512];
    int len = strlen(name) - 2;
    memcpy(buf, name, len);
    buf[len] = 0;
    strcat(buf, " has exited\n");
    broadcast_message(buf, strlen(buf), threadno);
    delete_thread_variables(threadno);
}

void broadcast_message(char* message, int count, int threadno) {
    char* broadcast_string = malloc(sizeof(char) * 256);
    memcpy(broadcast_string, message, count);
    broadcast_string[count] = 0;
    pthread_mutex_lock(&lock);
    for (int i = 0; i < arraylist_size(messages); i++) {
        if (i == threadno) {
            continue;
        }
        arraylist* thread_messages = *(arraylist**)arraylist_get(messages, i);
        if (thread_messages) {
            arraylist_addEnd(thread_messages, &broadcast_string);
        }
    }
    pthread_mutex_unlock(&lock);
}

void read_messages(int sock, int threadno) {
    arraylist* my_messages = *(arraylist**)arraylist_get(messages, threadno);
    pthread_mutex_lock(&lock);
    for (int i = 0; i < arraylist_size(my_messages); i++) {
        // TODO make synchronization more efficient
        char* message = *(char**)arraylist_get(my_messages, i);
        int len = strlen(message);
        write(sock, message, len);
    }
    arraylist_clear(my_messages);
    pthread_mutex_unlock(&lock);
}

void* user_thread(void* sockptr) {
    int sock = *(int*) sockptr;
    free(sockptr);
    write(sock, "What is your name?\n", 19);
    char buf[255];
    char name_buf[255];
    int recv_count;
    while((recv_count = recv(sock, name_buf, 255, 0)) < 0) {
        wait(sock, 30000);
    }
    int threadno = create_thread_variables(name_buf);
    char name_msg[512];
    int name_len = recv_count - 2;
    if (recv_count == 0) {
        write(sock, "No name provided\n", 17);
        graceful_exit(threadno, name_buf);
        return NULL;
    }
    memcpy(name_msg, name_buf, recv_count - 1);
    strcpy(name_msg + name_len, " has entered the room\n");
    broadcast_message(name_msg, recv_count + 22, threadno);
    name_msg[name_len] = ':';
    name_msg[name_len + 1] = ' ';
    name_len += 2;
    int done = 0;
    while (!done) {
        wait(sock, 300);
        recv_count = recv(sock, buf, 255, 0);
        if(recv_count == 0) {
            done = 1;
        }
        else {
            if(recv_count<0) {
                if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    perror("Receive failed");
                }
            }
            else {
                strncpy(name_msg + name_len, buf, recv_count);
                broadcast_message(name_msg, recv_count + name_len, threadno);
            }
            read_messages(sock, threadno);
        }
    }
    graceful_exit(threadno, name_buf);
    return NULL;
}
int main(int argc, char** argv) {
    int port;
    if (argc < 2) {
        port = 8080;
    }
    else {
        int port_status = sscanf(argv[1], "%d", &port);
        if (!port_status) {
            perror("Port must be a number ");
            exit(1);
        }
    }
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Creating socket failed: ");
        exit(1);
    }

    // allow fast reuse of ports
    int reuse_true = 1;
    setsockopt(server_sock,
            SOL_SOCKET,
            SO_REUSEADDR,
            &reuse_true,
            sizeof(reuse_true));

    struct sockaddr_in addr; // internet socket address data structure
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port); // byte order is significant
    addr.sin_addr.s_addr = INADDR_ANY; // listen to all interfaces

    int res = bind(server_sock, (struct sockaddr*)&addr, sizeof(addr));
    if (res < 0) {
        perror("Error binding to port");
        exit(1);
    }

    struct sockaddr_in remote_addr;
    unsigned int socklen = sizeof(remote_addr);
    // wait for a connection
    res = listen(server_sock,0);
    if (res < 0) {
        perror("Error listening for connection");
        exit(1);
    }
    pthread_mutex_init(&lock, NULL);
    num_threads = 0;
    messages = arraylist_init(sizeof(arraylist*), 8);
    names = arraylist_init(sizeof(char*), 8);
    openspaces = linkedlist_init(sizeof(int));
    pthread_t thread;
    while (1) {
        int* sockptr = (int*)malloc(sizeof(int));
        *sockptr = accept(server_sock, (struct sockaddr*)&remote_addr, &socklen);
        int sock = *sockptr;
        if(sock < 0) {
            perror("Error accepting connection");
        }
        else {
            if (fcntl(sock, F_SETFL, fcntl(sock, F_GETFL) | O_NONBLOCK) < 0) {
                perror("Couldn't make socket nonblocking");
            }
            // create a thread to handle requests from client side
            pthread_attr_t attr;
            pthread_attr_init( &attr );
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
            pthread_create(&thread, &attr, user_thread, (void*)sockptr);
        }
    }
    pthread_mutex_destroy(&lock);
    shutdown(server_sock,SHUT_RDWR);
}
