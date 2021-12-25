#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 5600
#define BACKEND_IP "http://192.168.31.194:3000/api"


struct memory {
   char *response;
   size_t size;
};

static size_t cb(void *data, size_t size, size_t nmemb, void *userp)
{
   size_t realsize = size * nmemb;
   struct memory *mem = (struct memory *)userp;

   char *ptr = realloc(mem->response, mem->size + realsize + 1);
   if(ptr == NULL)
     return 0;  /* out of memory! */

   mem->response = ptr;
   memcpy(&(mem->response[mem->size]), data, realsize);
   mem->size += realsize;
   mem->response[mem->size] = 0;

   return realsize;
}

struct memory chunk = {0};

const struct memory empty = {0};

char* get_request(char *url)
{
  	CURL * curl;
	CURLcode res;

	chunk = empty;

	char address[1024];
	strcpy(address, BACKEND_IP);
	strcat(address, url);
	strtok(address, "\n");

	char new_address[1024] = "";

    for(size_t i = 0; i < strlen(address); i++) {
        if(address[i] == 32){
            strcat(new_address, "%20");
        } else {
            strncat(new_address, &address[i], 1);
        }
    }

    printf("Address: %s\n", new_address);

	strcpy(address, new_address);
	strtok(address, "\n");

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, address);

		/* example.com is redirected, so we tell libcurl to follow redirection */
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);

		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void * ) & chunk);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);

		/* Check for errors */
		if (res != CURLE_OK)
		  fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

		/* always cleanup */
		curl_easy_cleanup(curl);
	}

	return chunk.response;
}

int main(){
	int sockfd, ret;
	struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	char buffer[2000] = "";
	pid_t childpid;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("<< - >> Error in connection.\n");
		exit(1);
	}
	printf("<< + >> Server Socket is created\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);

	ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("<< - >> Error in binding.\n");
		exit(1);
	}
	printf("<< + >> Socket is on %d port\n", 5600);

	if(listen(sockfd, 10) == 0){
		printf("<< + >> Listening....\n");
	}else{
		printf("<< - >> Error in binding.\n");
	}


	while(1){
		newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
		if(newSocket < 0){
			exit(1);
		}
		printf("New connection - %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

		if((childpid = fork()) == 0){
			close(sockfd);

			while(1){
				recv(newSocket, buffer, 1024, 0);
				if(strcmp(buffer, ":exit") == 0){
					printf("Connection lost - %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
					break;
				}else{
					printf("Client - %d: %s\n", newSocket, buffer);

					strcpy(buffer, get_request(buffer));

					send(newSocket, buffer, strlen(buffer), 0);

					bzero(buffer, sizeof(buffer));
				}
			}
		}

	}

	close(newSocket);


	return 0;
}
