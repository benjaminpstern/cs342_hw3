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
#define BUF_SIZE 1024
static int num_threads;
static pthread_mutex_t lock;
static arraylist* messages;

void wait(int fd, int num_millisecs) {
    struct timeval tv;
    tv.tv_sec = num_millisecs / 1000;
    tv.tv_usec = (num_millisecs % 1000) * 1000;
    fd_set set;
    FD_ZERO(&set);
    FD_SET(fd, &set);
    select(fd + 1, &set, NULL, NULL, &tv);
}

int create_thread_variables() {
    pthread_mutex_lock(&lock);
    int threadno = num_threads;
    num_threads++;
    arraylist* thread_messages = arraylist_init(sizeof(char*), 1);
    arraylist_addEnd(messages, &thread_messages);
    pthread_mutex_unlock(&lock);
    return threadno;
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
        arraylist_addEnd(thread_messages, &broadcast_string);
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
    int threadno = create_thread_variables();
    int sock = *(int*) sockptr;
    free(sockptr);
    char buf[255];
    int recv_count;
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
                broadcast_message(buf, recv_count, threadno);
            }
            read_messages(sock, threadno); 
        }
    }
    arraylist* my_messages = *(arraylist**)arraylist_get(messages, threadno);
    arraylist_free(my_messages);
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
