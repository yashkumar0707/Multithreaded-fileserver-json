#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <json-c/json.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

typedef struct
{
	int sock;
	struct sockaddr address;
	int addr_len;
} connection_t;

void *process(void *ptr)
{
	char *buffer;
	int len;
	connection_t *conn;
	long addr = 0;
	char *hello = "hello from server";
	json_object *jobj = json_object_new_object();
	json_object *jarray = json_object_new_array();
	int f = 0;
	if (!ptr)
		pthread_exit(0);
	conn = (connection_t *)ptr;

	/* read length of message */
	read(conn->sock, &len, sizeof(int));
	if (len > 0)
	{
		addr = (long)((struct sockaddr_in *)&conn->address)->sin_addr.s_addr;
		char buffer[1024];

		/* read message */
		read(conn->sock, buffer, 1024);
		if (strcmp(buffer, "") != 0)
		{
			printf("client requested for %s\n", buffer);
			struct json_object *path;
			struct json_object *verb;
			struct json_object *parsed_json;
			//printf("%s", &buf);
			parsed_json = json_tokener_parse(buffer);
			json_object_object_get_ex(parsed_json, "path", &path);
			json_object_object_get_ex(parsed_json, "verb", &verb);
			//printf("yashshh");
			printf("%s \n", json_object_get_string(path));
			char path1[1000];
			strcpy(path1, json_object_get_string(path));
			if (strcmp(json_object_get_string(verb), "LIST") == 0)
			{
				int n = 0, i = 0;
				DIR *d;
				struct dirent *dir;
				d = opendir(json_object_get_string(path));
				int check = 0;
				if (ENOENT != errno)
				{
					//printf("ASDasd");
					while ((dir = readdir(d)) != NULL)
					{
						if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, ".."))
						{
						}
						else
						{
							check = 1;
							n++;
						}
					}
				}
				if (check == 1)
				{

					rewinddir(d);

					char *filesList[n];
					// json_object *jobj1 = json_object_new_object();
					// json_object *jarray = json_object_new_array();
					while ((dir = readdir(d)) != NULL)
					{
						if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, ".."))
						{
						}
						else
						{
							filesList[i] = (char *)malloc(strlen(dir->d_name) + 1);
							strncpy(filesList[i], dir->d_name, strlen(dir->d_name));
							json_object *jstring1 = json_object_new_string(dir->d_name);
							json_object_array_add(jarray, jstring1);
							i++;
						}
					}
					json_object *jstring2 = json_object_new_string("false");
					json_object_object_add(jobj, "error", jstring2);
					json_object_object_add(jobj, "result", jarray);
					char temp[1000];
					strcpy(temp, json_object_to_json_string(jobj));
					printf("%s \n", temp);

					rewinddir(d);
					char hello[1000] = {0};
					for (int i = 0; i < n; i++)
					{
						strcat(hello, filesList[i]);
						strcat(hello, ",");
					}
					// int converted_number = htonl(n);

					// write(conn->sock, &converted_number, sizeof(converted_number));

					write(conn->sock, temp, sizeof(hello));

					// for(int i=0;i<n;i++)
					// {
					// 	len = strlen(filesList[i]);
					// 	write(conn->sock, &len, sizeof(int));
					// 	write(conn->sock, filesList[i], len);
					// }
					/* print message */
					// printf("%d.%d.%d.%d: path %s\n",
					// 	   (int)((addr)&0xff),
					// 	   (int)((addr >> 8) & 0xff),
					// 	   (int)((addr >> 16) & 0xff),
					// 	   (int)((addr >> 24) & 0xff),
					// 	   buffer);
					printf("files in the path requested has been sent\n");
					memset(buffer, 0, sizeof buffer);
				}
				else
				{
					//printf("ASDa");
					//printf("\nno files at requested path");
					json_object *jstring2 = json_object_new_string("true");
					json_object *jarray = json_object_new_string("No files found at requested path");
					json_object_object_add(jobj, "error", jstring2);
					json_object_object_add(jobj, "result", jarray);
					char temp[1000];
					strcpy(temp, json_object_to_json_string(jobj));
					write(conn->sock, temp, 1000);
					memset(buffer, 0, sizeof buffer);
					//free(buffer);
				}
				//free(buffer);
			}
			else
			{
				printf("yeah %s", path1);
				FILE *fp = NULL;
				int c;
				int a[2];
				int i = 0;
				char path[100];
				//printf("%s", json_object_get_string(path));
				//fp = fopen(json_object_object_get_ex(parsed_json, "path", &path), "rb");
				fp = fopen(path1, "rb");
				json_object *jstring2 = json_object_new_string("false");
				json_object *jstring3 = json_object_new_string("true");

				if (fp == NULL)
				{
					json_object *jstring4 = json_object_new_string("File Doesnt exist");
					json_object_object_add(jobj, "error", jstring3);
					json_object_object_add(jobj, "result", jstring4);
					printf("File doesnt exist\n");
				}

				// c = fgetc(fp);
				// // printf("%d\n",c);
				// if (c == EOF)
				// {
				// 	printf("empty\n");
				// 	fclose(fp);
				// 	return 0;
				// }
				// else
				// {
				// 	ungetc(c, fp);
				// }
				else
				{
					while ((c = fgetc(fp)) != EOF)
					{
						//in this range ascii files dont exist
						if (c < 0 || c > 127)
						{
							//printf("%d\n",c);
							printf("utf-8\n");
							f = 1;
							break;
						}
					}
					// char temp[1000];
					// strcpy(temp, json_object_to_json_string(jobj));
					// printf("%s \n", temp);
					//printf("%d",c);
					if (f == 1)
					{
						json_object *jstring4 = json_object_new_string("UTF-8");
						json_object_object_add(jobj, "error", jstring2);
						json_object_object_add(jobj, "result", jstring4);
					}
					else
					{
						printf("ascii\n");
						json_object *jstring4 = json_object_new_string("ASCII");
						json_object_object_add(jobj, "error", jstring2);
						json_object_object_add(jobj, "result", jstring4);
					}
					fclose(fp);
				}
				char temp[1000];
				strcpy(temp, json_object_to_json_string(jobj));
				printf("%s \n", temp);
				printf("%d", c);
				write(conn->sock, temp, 1024);
			}
		}
	}

	/* close socket and clean up */
	close(conn->sock);
	free(conn);
	pthread_exit(0);
}

int main(int argc, char **argv)
{
	int sock = -1;
	struct sockaddr_in address;
	int port;
	connection_t *connection;
	pthread_t thread;

	/* check for command line arguments */
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s port\n", argv[0]);
		return -1;
	}

	/* obtain port number */
	if (sscanf(argv[1], "%d", &port) <= 0)
	{
		fprintf(stderr, "%s: error: wrong parameter: port\n", argv[0]);
		return -2;
	}

	/* create socket */
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock <= 0)
	{
		fprintf(stderr, "%s: error: cannot create socket\n", argv[0]);
		return -3;
	}

	/* bind socket to port */
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	if (bind(sock, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) < 0)
	{
		fprintf(stderr, "%s: error: cannot bind socket to port %d\n", argv[0], port);
		return -4;
	}

	/* listen on port */
	if (listen(sock, 5) < 0)
	{
		fprintf(stderr, "%s: error: cannot listen on port\n", argv[0]);
		return -5;
	}

	printf("%s: ready and listening\n", argv[0]);

	while (1)
	{
		/* accept incoming connections */
		connection = (connection_t *)malloc(sizeof(connection_t));
		connection->sock = accept(sock, &connection->address, &connection->addr_len);
		if (connection->sock <= 0)
		{
			free(connection);
		}
		else
		{
			/* start a new thread but do not wait for it */
			pthread_create(&thread, 0, process, (void *)connection);
			pthread_detach(thread);
		}
	}

	return 0;
}