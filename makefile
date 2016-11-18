all: clean
	gcc -L./usr/local/lib -I./usr/local/include -g -o myShell myShell.c -lreadline 
	gcc -L./usr/local/lib -I./usr/local/include -ggdb matmult_p.c -o matmult_p -lreadline
	gcc -L./usr/local/lib -I./usr/local/include -ggdb matformatter.c -o matformatter -lreadline
	gcc multiply.c -o multiply
clean:
	rm myShell || true
	rm multiply || true
