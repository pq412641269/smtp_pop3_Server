objects = central.o devicecontrol.o net.o mailrecv.o parsemail.o 

smtp_pop3 : ${objects}
	gcc *.o -o smtp_pop3 -lpthread -lsqlite3
	rm *.o -rf

%.o : %.c %.h
	gcc -g -Wall -c *.c -lpthread -lsqlite3
