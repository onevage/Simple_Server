server : main.o http_conn.o
	g++ -o server main.o http_conn.o

#添加了头文件的依赖后，头文件的修改才会触发重新编译
main.o : main.cpp  http_conn.o threadpool.h http_conn.h
	g++  -o main.o main.cpp

http_conn.o:http_conn.cpp http_conn.h
	g++ -o http_conn.cpp 

clean:
	rm  server main.o http_conn.o