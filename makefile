all:
	gcc -L./usr/local/lib -I./usr/local/include -g -o myShell myShell.c -lreadline 
	gcc -L./usr/local/lib -I./usr/local/include -ggdb matmult_p.c -o matmult_p -lreadline
clean:
	rm myShell || true
