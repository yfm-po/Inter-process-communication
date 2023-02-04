all: main proc_p1 proc_p2 proc_t proc_d proc_serv2

main: main.cpp
	g++ -std=c++11 main.cpp -o main -Wno-write-strings

proc_p1: proc_p1.cpp
	g++ -std=c++11 proc_p1.cpp -o proc_p1

proc_p2: proc_p2.cpp
	g++ -std=c++11 proc_p2.cpp -o proc_p2

proc_t: proc_t.cpp	
	g++ -std=c++11 proc_t.cpp -o proc_t

proc_d: proc_d.cpp
	g++ -std=c++11 proc_d.cpp -o proc_d

proc_serv2: proc_serv2.cpp
	g++ -std=c++11 proc_serv2.cpp -o proc_serv2

clean:
	rm main proc_p1 proc_p2 proc_t proc_d proc_serv2