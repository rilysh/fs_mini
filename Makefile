FLAGS   = -O2
INCLUDE = -I./include
PROGRAM = example

all:
	${CXX} ${FLAGS} ${INCLUDE} ./test/test.cpp -o example

clean:
	rm -f example hello.txt test.txt
