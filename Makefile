all:
	gcc *.c -O3 -lm -o simulador

clean:
	rm simulador
