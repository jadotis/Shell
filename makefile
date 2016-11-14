all:
	gcc -L./usr/local/lib -I./usr/local/include -g -o myShell myShell.c -lreadline 

clean:
	rm myShell || true
