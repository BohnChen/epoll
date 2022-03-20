CC:=g++
srcs:=$(wildcard ./ServerSrc/*.cpp)
objs:=$(srcs:%.cpp=%.o)
srcs2:=$(wildcard ./ClientSrc/*.cpp)
objs2:=$(srcs2:%.cpp=%.o)
out:=./ServerSrc/server
out2:=./ClientSrc/client
all:$(out) $(out2)
$(out2):$(objs2)
	$(CC) $(objs2) -g -o $(out2) 
%.o:%.cpp
	$(CC) -g -Wall -c $^ -o $@
$(out):$(objs)
	$(CC) $(objs) -g -o $(out) 
%.o:%.cpp
	$(CC) -g -Wall -c $^ -o $@
.PHONY:clean rebuild
clean:
	$(RM) $(objs) $(out) $(objs2) $(out2)
rebuild:clean all



