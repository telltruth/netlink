
main.out:main.o netlink.o NetIntfBuilder.o
	g++ -o main.out -pthread main.o netlink.o NetIntfBuilder.o
netlink.o:
	g++ -Wl,--no-as-needed -c -std=c++0x -pthread netlink.cpp -o netlink.o
NetIntfBuilder.o:
	g++ -Wl,--no-as-needed -c -std=c++0x -pthread NetIntfBuilder.cpp -o NetIntfBuilder.o
main.o: clean
	g++ -Wl,--no-as-needed -c -std=c++0x -pthread main.cpp -o main.o
clean:
	rm -f *.o main.out 

