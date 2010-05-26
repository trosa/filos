all: helper logico
helper: helper.c helper_sock.c fila.c
	gcc -o helper helper.c helper_sock.c fila.c -lpthread
logico: logico.c logico_sock.c fila.c
	gcc -o logico logico.c logico_sock.c fila.c -lpthread
clean:
	rm -f logico helper