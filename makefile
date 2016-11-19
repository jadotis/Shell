all: clean
	gcc -L./usr/local/lib -I./usr/local/include -g -o myShell myShell.c -lreadline 
	gcc -L./usr/local/lib -I./usr/local/include -ggdb matmult_p.c -o matmult_p -lreadline
	gcc multiply.c -o multiply
	gcc -L./usr/local/lib -I./usr/local/include -g matformatter.c -o matformatter -lreadline
clean:
	rm myShell || true
	rm multiply || true
	rm matmult_p || true
	rm matformatter || true
