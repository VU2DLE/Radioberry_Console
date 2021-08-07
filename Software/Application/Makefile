CC = gcc

rbc_comm: 
	${CC} -c rbc_comm.c button.c -lpthread -lwiringPi
	${CC} -o rbc_comm rbc_comm.o button.o -lpthread -lwiringPi
clean:
	@echo "Cleaning up..."
	rm -rf rbc_comm.o button.o rbc_comm
