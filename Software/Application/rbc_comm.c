#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringSerial.h>

#include <unistd.h>
#include "button.h"

#define DEBUG_PRINT 0


/*------------------- Global Declarations -----------------*/
int pipe_fd[2], serial_port, chcount = 0, nbytes, n;
int sockfd, portno;
char vcp_read_buff[256], pipe_read_buff[256], cat_buffer[50], buffer[50];
struct hostent *server;
struct sockaddr_in serv_addr;
int bcat_buff = 0;
unsigned int Audio_Gain = 0, AGC_Gain = 0, RX_Gain = 0;
/*------------------- Global Declarations -----------------*/

void splash_screen (void)
{
	printf("/*******************************************/\n");
	printf("/**          W E L C O M E                **/\n");
	printf("/**               T O                     **/\n");
	printf("/**  piHPSDR Controller for Radioberry    **/\n");
	printf("/**               V1.0                    **/\n");
	printf("/**-------------------------------------- **/\n");
	printf("/**          DE VU2DLE Dileep             **/\n");
	printf("/*******************************************/\n");
}


void* pico_vcp_read(void* arg)
{
#if DEBUG_PRINT
	printf("Entering: pico_vcp_read\n");
#endif
	while(1)
	{
		chcount = 0;	
		while(serialDataAvail (serial_port))
		{ 
			vcp_read_buff[chcount++] = serialGetchar (serial_port); /* receive char serially*/	
			if(chcount == 4)
			{
				vcp_read_buff[4] = '\0';
				write(pipe_fd[1], vcp_read_buff, 4);
			}
		}
	}
    // exit the current thread
    pthread_exit(NULL);
#if DEBUG_PRINT
    printf("Leaving: pico_vcp_read\n");
#endif
}


void* pihpsdr_cat_interface(void* arg)
{
#if DEBUG_PRINT
	printf("Entering: pihpsdr_cat_interface\n");
#endif
	while(1)
	{
		nbytes = read(pipe_fd[0], pipe_read_buff, 5);
		pipe_read_buff[nbytes] = '\0';
#if DEBUG_PRINT		
		printf("Received string: %s %d\n", pipe_read_buff, nbytes);
#endif
		if (!strcmp(pipe_read_buff, "EARI")) {strcpy(cat_buffer,"ZZAF01;"); bcat_buff = 1;}
		else if (!strcmp(pipe_read_buff, "EALE")) {strcpy(cat_buffer,"ZZAE01;"); bcat_buff = 1;}

		/*-------------------- [ AF Gain Control] -----------------------*/
		else if (!strcmp(pipe_read_buff, "EBLE")) {
			if(Audio_Gain) Audio_Gain--;
			sprintf(cat_buffer, "ZZAG%03d;",Audio_Gain);
			bcat_buff = 1;
		}
		else if (!strcmp(pipe_read_buff, "EBRI")) {
			if(Audio_Gain < 100) Audio_Gain++;
			sprintf(cat_buffer, "ZZAG%03d;",Audio_Gain);
			bcat_buff = 1;
		}

		/*-------------------- [ AGC Gain Control] -----------------------*/
		else if (!strcmp(pipe_read_buff, "ECLE")) {
			if(AGC_Gain) AGC_Gain--;
			sprintf(cat_buffer, "ZZAR%03d;", (AGC_Gain - 20));
			bcat_buff = 1;
		}
		else if (!strcmp(pipe_read_buff, "ECRI")) {
			if(AGC_Gain < 140) AGC_Gain++;
			sprintf(cat_buffer, "ZZAR%03d;",(AGC_Gain - 20));
			bcat_buff = 1;
		}

		/*-------------------- [ RX Gain Control] -----------------------*/
		else if (!strcmp(pipe_read_buff, "EDLE")) {
			if(RX_Gain) RX_Gain--;
			sprintf(cat_buffer, "RA%02d;", RX_Gain);
			bcat_buff = 1;
		}
		else if (!strcmp(pipe_read_buff, "EDRI")) {
			if(RX_Gain < 100) RX_Gain++;
			sprintf(cat_buffer, "RA%02d;", RX_Gain);
			bcat_buff = 1;
		}
		/*----------------------- [ RIT +/- ]----------------------------*/
		else if (!strcmp(pipe_read_buff, "EERI")) {strcpy(cat_buffer,"ZZRU;"); bcat_buff = 1;}
		else if (!strcmp(pipe_read_buff, "EELE")) {strcpy(cat_buffer,"ZZRD;"); bcat_buff = 1;}

		/*------------------- [ push switch handler ]---------------------*/
		else if (pipe_read_buff[0] == 'K') {
			printf ("pipe_read_buff: %s\n", pipe_read_buff);
			push_button_handler(pipe_read_buff);
		}
		if(bcat_buff) {
			bcat_buff = 0;
			printf ("cat_buffer: %s\n", cat_buffer);
			fflush (stdout) ;

			/* Send message to the server */
			n = write(sockfd, cat_buffer, strlen(cat_buffer));
			if (n < 0) {
					perror("ERROR writing to socket");
					exit(1);
			}
		}
	}
	
    // exit the current thread
    pthread_exit(NULL);
#if DEBUG_PRINT    
	printf("Leaving: pihpsdr_cat_interface\n");
#endif
}



int main(int argc, char *argv[]) {

	pthread_t ptid_vcp, ptid_cat;

    if (pipe(pipe_fd) == -1)
    {
        fprintf(stderr, "Pipe Failed" );
        return 1;
    }  


	// Creating threads
	pthread_create(&ptid_vcp, NULL, &pico_vcp_read, NULL);
	pthread_create(&ptid_cat, NULL, &pihpsdr_cat_interface, NULL);


//	if (argc < 3) {
//	  fprintf(stderr,"usage %s hostname port\n", argv[0]);
//	  exit(0);
//	}
	
//	portno = atoi(argv[2]);
	portno = 19090;

	/* Create a socket point */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
	  perror("ERROR opening socket");
	  exit(1);
	}

//	server = gethostbyname(argv[1]);
	server = gethostbyname("0.0.0.0");
//	server = gethostbyname("169.254.18.86");

	if (server == NULL) {
	  fprintf(stderr,"ERROR, no such host\n");
	  exit(0);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	/* Now connect to the server */
	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
	  perror("ERROR connecting");
	  exit(1);
	}


   	/* open serial port */  
   	if ((serial_port = serialOpen ("/dev/ttyACM0", 115200)) < 0)	
	{
    	fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    	return 1 ;
  	}
  	/* initializes wiringPi setup */
  	if (wiringPiSetup () == -1)					
  	{
    	fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
   		return 1 ;
  	}
	
	splash_screen();

	/* Get current Audio Gain from pihpsdr*/
	n = write(sockfd, "ZZAG;", 5);
	n = read(sockfd, buffer, 50);

	Audio_Gain = atoi(&buffer[4]);
	fprintf (stdout, "Audio_Gain: %d\n", Audio_Gain) ;

	/* Get current AGC Gain from pihpsdr*/
	n = write(sockfd, "ZZAR;", 5);
	n = read(sockfd, buffer, 50);

	AGC_Gain = (atoi(&buffer[4])) + 20;
	fprintf (stdout, "AGC_Gain: %d\n", AGC_Gain) ;

	/* Get current RX-Gain Attenuator from pihpsdr*/
	n = write(sockfd, "RA;", 3);
	n = read(sockfd, buffer, 50);

	RX_Gain = atoi(&buffer[2]);
	if(RX_Gain) { RX_Gain /= 100; }
	fprintf (stdout, "RX-Gain: %d\n", RX_Gain);
   
    while(1);
}
