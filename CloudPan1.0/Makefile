.PHONY:clean all
exec:
	#g++ -c md5.cpp;
	#ar -rc md5.a md5.o;
	#g++ -c MyDB.cpp;
	#ar -rc MyDB.a MyDB.o;

	#g++ -o cli conCli.cpp md5.a;
	#g++ -o cli testcli.cpp conCli.a md5.a;
	#g++ -o ser conSer.cpp MyDB.a -L/usr/lib/mysql -lmysqlclient --std=c++0x;
	
	g++ -o cli conCli.cpp md5.cpp
	g++ -o ser conSer.cpp MyDB.cpp -I /usr/include/libmemcached/include -L/usr/include/libmemcached/lib -L/usr/lib/mysql -lmysqlclient -lmemcached -lpthread --std=gnu++0x;
	g++ -o FTS fileTrans.cpp md5.cpp MyDB.cpp -I /usr/include/libmemcached/include -L/usr/include/libmemcached/lib -L/usr/lib/mysql -lmysqlclient -lmemcached -lpthread --std=gnu++0x;
clean:
	rm -f *.o;
	rm -f *~;
	rm *.a;


