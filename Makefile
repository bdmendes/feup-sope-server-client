COMP := gcc -Wall -Werror -pedantic

BIN_DIR := bin
SRC_DIR := src

CLIENT_SRC_FILES = client.c common/utils/utils.c common/logs/logs.c client/input_validation/input_validation.c common/message/message.c
CLIENT_DEPS = 
CLIENT_SRC_FILES_PREF = $(addprefix ${SRC_DIR}/, ${CLIENT_SRC_FILES})
CLIENT_DEPS_PREF = $(addprefix ${BIN_DIR}/, ${CLIENT_DEPS})

all: bin/s bin/c 

bin/s: bin/server.o bin/lib.o src/delay.c src/delay.h
	${COMP} -DDELAY=0 -o ${BIN_DIR}/s src/delay.c bin/lib.o bin/server.o -pthread

bin/c: ${CLIENT_SRC_FILES_PREF} ${CLIENT_DEPS_PREF}
	${COMP} -o ${BIN_DIR}/c ${CLIENT_SRC_FILES_PREF} ${CLIENT_DEPS_PREF} -pthread
	
# server.o: server.c
# 	gcc -Wall -c -o server.o server.c

# lib.o: lib.c lib.h
# 	gcc -Wall -c -o lib.o lib.c

clean:
	rm -f ${BIN_DIR}/s ${BIN_DIR}/c
