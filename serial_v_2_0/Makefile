objects = main.o
#generate final exe file using the exsiting -o or -a lib file
serial:$(objects) 
	gcc -o serial $(objects) -lm
#generata -o file ,it is lib file 
$(objects):main.c data.h dir.h serial.h protocol.h
	gcc -c main.c -std=gnu99

#clean operation
.PHONY:clean
clean:
	-rm serial $(objects)
