make: netclient.c libnetfiles.c
	gcc -Wall -c libnetfiles.c 
	gcc -g -Werror -Wall -fsanitize=address netclient.c
server: netfileserver.c
	gcc -g -Werror -Wall -fsanitize=address -o server netfileserver.c	
clean:
	rm server rm client	