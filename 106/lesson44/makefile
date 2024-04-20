cc=g++
LD=-DMYSELF
.PHONY:all
all:calserver calclient
calserver:calServer.cc
	$(cc) -o $@ $^ -std=c++11 -ljsoncpp #-lpthread ${LD}
calclient:calClient.cc
	$(cc) -o $@ $^ -std=c++11 -ljsoncpp -Wl,-rpath,./jsoncpp/build/debug/lib #${LD} 
.PHONY:clean
clean:
	rm -f calserver calclient
