all: ser.cpp mysql.cpp mysql.hpp cli.cpp
	g++ -o ser ser.cpp mysql.cpp mysql.hpp -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient /usr/Jsoncpp/jsoncpp-src-0.5.0/libs/linux-gcc-5.2.1/libjson_linux-gcc-5.2.1_libmt.a -lpthread -levent 
	g++ -o cli cli.cpp /usr/Jsoncpp/jsoncpp-src-0.5.0/libs/linux-gcc-5.2.1/libjson_linux-gcc-5.2.1_libmt.a -lpthread

