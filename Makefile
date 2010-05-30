all: helper logico
helper: helper.c helper_sock.c fila.c criacliente.c
	gcc -o helper helper.c helper_sock.c fila.c criacliente.c -lpthread
logico: logico.c
	gcc -o logico logico.c -lpthread
clean:
	rm -f logico helper
