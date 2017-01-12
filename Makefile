CFLAGS = -Wall -ansi
ParameterManager : ParameterManager.o
	ar rvs libpm.a *.o
ParameterManager.o : ParameterManager.c ParameterManager.h
	gcc $(CFLAGS) -c ParameterManager.c
