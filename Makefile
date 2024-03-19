all:
	gcc game_thread_group1.c -o game_thread -lpthread
	gcc game_process_group1.c -o game_process

clean:
		rm -fr game_process game_thread

