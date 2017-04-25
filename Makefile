make: libnetfiles.c
	gcc -g -Werror -Wall -fsanitize=address -o libnetfiles libnetfiles.c
server: netfileserver.c
	gcc -g -Werror -Wall -fsanitize=address -o netfileserver netfileserver.c	
run_server:
	./netfileserver
run_server:
	./libnetfiles
clean_server:
	rm netfileserver
clean_client:
	rm libnetfiles