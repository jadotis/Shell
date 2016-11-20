all: clean
	gcc -L./usr/local/lib -I./usr/local/include -w -g -o myShell myShell.c -lreadline 
	gcc -L./usr/local/lib -I./usr/local/include -w -ggdb matmult_p.c -o matmult_p -lreadline
	gcc multiply.c -w -o multiply
	gcc -L./usr/local/lib -I./usr/local/include -w -g matformatter.c -o matformatter -lreadline
clean:
	rm myShell || true
	rm multiply || true
	rm matmult_p || true
	rm matformatter || true
