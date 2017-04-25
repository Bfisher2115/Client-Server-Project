make: libnetfiles.c
	gcc -g -Werror -Wall -fsanitize=address -o client libnetfiles.c
server: netfileserver.c
	gcc -g -Werror -Wall -fsanitize=address -o server netfileserver.c	
run_server:
	./server
run_client:
	./client
clean:
	rm server rm client	