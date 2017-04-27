make: netclient.c libnetfiles.o libnetfiles.c
	gcc -Wall -c libnetfiles.c 
	gcc -g -Werror -Wall -fsanitize=address -o client libnetfiles.o netclient.c
server: netfileserver.c
	gcc -g -Werror -Wall -fsanitize=address -o server netfileserver.c	
clean:
	rm server rm client	