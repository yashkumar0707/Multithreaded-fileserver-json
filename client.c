#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <../json-c/json.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <linux/in.h>

int main(int argc, char **argv)
{
	while (1)

	{
		int port;
		int sock = -1;
		struct sockaddr_in address;
		struct hostent *host;
		int len, n, val;

		/* checking commandline parameter */
		if (argc != 4)
		{
			printf("usage: %s hostname port text\n", argv[0]);
			return -1;
		}

		/* obtain port number */
		if (sscanf(argv[2], "%d", &port) <= 0)
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

		/* connect to server */
		address.sin_family = AF_INET;
		address.sin_port = htons(port);
		host = gethostbyname(argv[1]);
		if (!host)
		{
			fprintf(stderr, "%s: error: unknown host %s\n", argv[0], argv[1]);
			return -4;
		}
		memcpy(&address.sin_addr, host->h_addr_list[0], host->h_length);
		if (connect(sock, (struct sockaddr *)&address, sizeof(address)))
		{
			fprintf(stderr, "%s: error: cannot connect to host %s\n", argv[0], argv[1]);
			return -5;
		}

		/* send text to server */
		json_object *jobj = json_object_new_object();

		char verb[10];
		printf("LIST verb or GET verb? : ");
		gets(verb);
		if (strcmp(verb, "list") == 0)
		{
			char temp_buff[1000];
			printf("enter path to directory : ");
			//gets(*argv);
			char temp[1000];
			gets(temp);
			len = strlen(argv[3]);
			json_object *jstring = json_object_new_string(temp);
			json_object *jstring1 = json_object_new_string("LIST");
			json_object_object_add(jobj, "verb", jstring1);
			json_object_object_add(jobj, "path", jstring);
			if (strcpy(temp_buff, json_object_to_json_string(jobj)) == NULL)
			{
				perror("strcpy");
				return EXIT_FAILURE;
			}
			for (int i = 0; i < strlen(temp_buff); i++)
			{
				printf("%c", temp_buff[i]);
			}
			write(sock, &len, sizeof(int));
			if (write(sock, temp_buff, strlen(temp_buff)) == -1)
			{
				perror("write");
				return EXIT_FAILURE;
			}

			// write(sock, argv[3], len);

			// char *received[]={};
			// int received_int=0;
			char files[1000] = {0};

			val = read(sock, files, sizeof(files));
			printf("%s", files);
			if (val == 0)
			{
				printf("no files found at requested path");
			}
		}

		else
		{
			char temp_buff[1000];
			printf("enter path to directory : ");
			//gets(*argv);
			char temp[1000];
			gets(temp);
			len = strlen(argv[3]);
			json_object *jstring = json_object_new_string(temp);
			json_object *jstring1 = json_object_new_string("GET");
			json_object_object_add(jobj, "verb", jstring1);
			json_object_object_add(jobj, "path", jstring);
			if (strcpy(temp_buff, json_object_to_json_string(jobj)) == NULL)
			{
				perror("strcpy");
				return EXIT_FAILURE;
			}
			for (int i = 0; i < strlen(temp_buff); i++)
			{
				printf("%c", temp_buff[i]);
			}
			write(sock, &len, sizeof(int));
			if (write(sock, temp_buff, strlen(temp_buff)) == -1)
			{
				perror("write");
				return EXIT_FAILURE;
			}

			// write(sock, argv[3], len);

			// char *received[]={};
			// int received_int=0;
			char files[1000] = {0};

			val = read(sock, files, sizeof(files));
			// if (val > 0)
			// {
			// 	for (int i = 0; files[i] != '\0'; i++)
			// 	{
			// 		if ((files[i]) == ',')
			// 		{
			// 			printf("\n");
			// 		}
			// 		else
			// 		{
			// 			printf("%c", files[i]);
			// 		}
			// 	}
			// }
			printf("%s", files);
		}

		// for(int i=0;i<ntohl(received_int);i++)
		// {
		// 	char buffer[1024]={0};
		// 	val=read(sock, buffer, strlen(buffer));
		// 	printf("%d\n",val);
		// 	printf("%s\n",buffer);
		// }

		// printf("%d\n",n);
		printf("\n");
	}

	/* close socket */
	//close(sock);

	return 0;
}