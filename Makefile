
main.out: main.o SchemaFactory.o netlink.o NetIntfBuilder.o
	g++ -Wall -Wextra -Wl,--no-as-needed -o main.out -std=c++0x -lpthread main.o netlink.o NetIntfBuilder.o SchemaFactory.o
SchemaFactory.o:
	g++ -Wall -Wextra -Wl,--no-as-needed -c -std=c++0x -lpthread SchemaFactory.cpp -o SchemaFactory.o
netlink.o:
	g++ -Wall -Wextra -Wl,--no-as-needed -c -std=c++0x -lpthread netlink.cpp -o netlink.o
NetIntfBuilder.o:
	g++ -Wall -Wextra -Wl,--no-as-needed -c -std=c++0x -lpthread NetIntfBuilder.cpp -o NetIntfBuilder.o
main.o: clean
	g++ -Wall -Wextra -Wl,--no-as-needed -c -std=c++0x -lpthread main.cpp -o main.o
clean:
	rm -f *.o main.out 

