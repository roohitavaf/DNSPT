CXXFLAGS = -I/usr/include/mysql -I/usr/local/include/mysql++ 
LDFLAGS = -L/usr/local/lib
LDLIBS = -lmysqlpp -lmysqlclient -lldns


all:
	g++ *.cpp -o dnspt -std=c++11 $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

clean: 
	rm *.o