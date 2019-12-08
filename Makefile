#使用linux线程库的时候，要进行手动链接-pthread
server : main.o http_conn.o
	g++ -o server main.o http_conn.o -lpthread

#添加了头文件的依赖后，头文件的修改才会触发重新编译
main.o : main.cpp   threadpool.h http_conn.h
	g++  -c  main.cpp

http_conn.o:http_conn.cpp http_conn.h
	g++ -c http_conn.cpp 

clean:
	rm  server main.o http_conn.o