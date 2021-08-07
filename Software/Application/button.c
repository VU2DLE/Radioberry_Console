#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringSerial.h>

#include<unistd.h>
#include "button.h"



char buffer[256], na_buff[10], nb_buff[10];
int n, vfo_mode, filter, vfo_step, snb, anf, locked;
extern int sockfd; 


void push_button_LOCK_handler (char *key_received)
{
	memset(buffer, 0, sizeof(buffer));
	n = write(sockfd, "LK;", 3);
	n = read(sockfd, buffer, 50);
	locked = atoi(&buffer[2]);
	printf("buffer: %s %d\n",buffer, locked);
	if(locked) {
		n = write(sockfd, "LK00;", 5);
	}
	else {
		n = write(sockfd, "LK11;", 5);
	}
}

void push_button_vfoabswap_handler (char *key_received)
{

	if(!strcmp(key_received, "KM10")) {
		n = write(sockfd, "ZZCB;", 5);
	}
	
	else if(!strcmp(key_received, "KM14")) {
		n = write(sockfd, "ZZCF;", 5);
	}
	
	else if(!strcmp(key_received, "KM09")) {
		n = write(sockfd, "ZZCD;", 5);
	}
}

void push_button_ANF_handler (char *key_received)
{
	memset(buffer, 0, sizeof(buffer));
	n = write(sockfd, "ZZNT;", 5);
	n = read(sockfd, buffer, 50);
	anf = atoi(&buffer[4]);
	printf("buffer: %s %d\n",buffer, anf);
	if(anf) {
		n = write(sockfd, "ZZNT0;", 6);
	}
	else {
		n = write(sockfd, "ZZNT1;", 6);
	}
}

void push_button_SNB_handler (char *key_received)
{
	memset(buffer, 0, sizeof(buffer));
	n = write(sockfd, "ZZNN;", 5);
	n = read(sockfd, buffer, 50);
	snb = atoi(&buffer[4]);
	printf("buffer: %s %d\n",buffer, snb);
	if(snb) {
		n = write(sockfd, "ZZNN0;", 6);
	}
	else {
		n = write(sockfd, "ZZNN1;", 6);
	}

}

void push_button_NR_handler (char *key_received)
{
	memset(na_buff, 0, sizeof(na_buff));
	n = write(sockfd, "NR;", 3);
	usleep(1000);
	n = read(sockfd, buffer, 50);
	strncpy(na_buff,buffer,3);
	printf("na_buff: %s\n",na_buff);
	if(!strcmp(na_buff, "NR0")) {
		n = write(sockfd, "NR1;", 4);
	}
	if(!strcmp(na_buff, "NR1")) {
		n = write(sockfd, "NR2;", 4);
	}
	if(!strcmp(na_buff, "NR2")) {
		n = write(sockfd, "NR0;", 4);
	}
}

void push_button_NB_handler (char *key_received)
{
	n = write(sockfd, "ZZNB;", 5);
	bzero(buffer,50);
	n = read(sockfd, buffer, 255);
	strncpy(nb_buff,buffer,5);
	printf("nb_buff: %s [%d]\n",nb_buff, n);

	n = write(sockfd, "ZZNA;", 5);
	bzero(buffer,50);
	n = read(sockfd, buffer, 255);
	strncpy(na_buff,buffer,5);
	printf("na_buff: %s [%d]\n",na_buff, n);
	
	if((!strcmp(na_buff, "ZZNA0")) && (!strcmp(nb_buff, "ZZNB0"))) {
		n = write(sockfd, "ZZNA1;", 6);
	}

	if((!strcmp(na_buff, "ZZNA1")) && (!strcmp(nb_buff, "ZZNB0"))) {
		n = write(sockfd, "ZZNB1;", 6);
	}

	if(!strcmp(nb_buff, "ZZNB1")) {
		n = write(sockfd, "ZZNB0;", 6);
	}
}


void push_button_vfostep_handler (char *key_received)
{
	/* Get current filter from pihpsdr*/
	n = write(sockfd, "ZZAC;", 5);
	n = read(sockfd, buffer, 50);
	vfo_step = atoi(&buffer[4]);
	
	if(!strcmp(key_received, "KM03")) {
		if(vfo_step != 14) {
			vfo_step ++;
			printf("buffer: %s\n", buffer);
			sprintf(buffer, "ZZAC%02d;",vfo_step);
			n = write(sockfd, buffer, strlen(buffer));
		}
	}

	if(!strcmp(key_received, "KM07")) {
		if(vfo_step) {
			vfo_step --;
			printf("buffer: %s\n", buffer);			
			sprintf(buffer, "ZZAC%02d;",vfo_step);
			n = write(sockfd, buffer, strlen(buffer));
		}
	}


}

void push_button_filter_handler (char *key_received)
{
	/* Get current filter from pihpsdr*/
	n = write(sockfd, "ZZFI;", 5);
	n = read(sockfd, buffer, 50);
	filter = atoi(&buffer[4]);
	
	if(!strcmp(key_received, "KM11")) {
		if(filter != 11) {
			filter ++;
			printf("buffer: %s\n", buffer);
			sprintf(buffer, "ZZFI%02d;",filter);
			n = write(sockfd, buffer, strlen(buffer));
		}
	}

	if(!strcmp(key_received, "KM15")) {
		if(filter) {
			filter --;
			printf("buffer: %s\n", buffer);			
			sprintf(buffer, "ZZFI%02d;",filter);
			n = write(sockfd, buffer, strlen(buffer));
		}
	}


}


void push_button_band_handler (char *key_received)
{
	if(!strcmp(key_received, "KM04")) {
		sprintf(buffer, "ZZBU;");
		n = write(sockfd, buffer, strlen(buffer));
	}

	if(!strcmp(key_received, "KM08")) {
		sprintf(buffer, "ZZBD;");
		n = write(sockfd, buffer, strlen(buffer));
	}
}

void push_button_mode_handler (char *key_received)
{
	/* Get current mode from pihpsdr*/
	n = write(sockfd, "ZZMD;", 5);
	n = read(sockfd, buffer, 50);
	vfo_mode = atoi(&buffer[4]);
	
	if(!strcmp(key_received, "KM12")) {
		if(vfo_mode != 11) {
			vfo_mode ++;
			printf("ZZMD response: %s\n", buffer);
			printf("vfo_mode: %d\n", vfo_mode);
			sprintf(buffer, "ZZMD%02d;",vfo_mode);
			n = write(sockfd, buffer, strlen(buffer));
			printf("ZZMD command: %s\n", buffer);
		}
	}

	if(!strcmp(key_received, "KM16")) {
		if(vfo_mode) {
			vfo_mode --;
			printf("ZZMD response: %s\n", buffer);
			printf("vfo_mode: %d\n", vfo_mode);
			sprintf(buffer, "ZZMD%02d;",vfo_mode);
			n = write(sockfd, buffer, strlen(buffer));
			printf("ZZMD command: %s\n", buffer);
		}
	}

}

void push_button_handler (char *key_received)
{
	//printf ("push_button_handler: %s\n", key_received);

	if((!strcmp(key_received, "KM04")) || (!strcmp(key_received, "KM08"))) {
		//push_button_mode_handler(key_received);
		push_button_band_handler(key_received);
	}

	else if((!strcmp(key_received, "KM12")) || (!strcmp(key_received, "KM16"))) {
		//push_button_band_handler(key_received);
		push_button_mode_handler(key_received);
	}

	else if((!strcmp(key_received, "KM03")) || (!strcmp(key_received, "KM07"))) {
		//push_button_filter_handler(key_received);
		push_button_vfostep_handler(key_received);
	}

	else if((!strcmp(key_received, "KM11")) || (!strcmp(key_received, "KM15"))) {
		//push_button_vfostep_handler(key_received);
		push_button_filter_handler(key_received);
	}

	else if(!strcmp(key_received, "KM02")) {
		push_button_NB_handler(key_received);
	}

	else if(!strcmp(key_received, "KM06")) {
		push_button_NR_handler(key_received);
	}

	else if(!strcmp(key_received, "KM01")) {
		push_button_SNB_handler(key_received);
	}

	else if(!strcmp(key_received, "KM05")) {
		push_button_ANF_handler(key_received);
	}

	else if((!strcmp(key_received, "KM09")) || (!strcmp(key_received, "KM10")) || (!strcmp(key_received, "KM14"))) {
		push_button_vfoabswap_handler(key_received);
	}

	else if(!strcmp(key_received, "KM13")) {
		push_button_LOCK_handler(key_received);
	}
}
