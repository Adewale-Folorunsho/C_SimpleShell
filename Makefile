shell2: shell2.c
	gcc -o shell2 shell2.c -I.

shell: shell.c
	gcc -o shell shell.c -I.
	
timer: timer.c
	gcc -o timer timer.c -I.
