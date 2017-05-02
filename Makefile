make: netclient.c libnetfiles.o libnetfiles.c netfileserver.c
	gcc -Wall -c libnetfiles.c 
	gcc -g -Werror -Wall -fsanitize=address -o client libnetfiles.o netclient.c
	gcc -g -Werror -Wall -fsanitize=address -lpthread -o server netfileserver.c 	
clean:
	rm server rm client	rm libnetfiles.o