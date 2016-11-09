all:
	gcc -L./usr/local/lib -I./usr/local/include -o myShell myShell.c -lreadline 

clean:
	rm myShell || true
