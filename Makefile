CC=gcc
CC_FLAGS=-Wall -Werror -pedantic
LINKED_LIBS=-pthread

BIN_DIR=bin
OBJ_DIR=obj
SRC_DIR=src

CLIENT_NAME = c
_CLIENT_FILES = client.c common/utils/utils.c common/logs/logs.c client/input_validation/input_validation.c common/message/message.c
CLIENT_FILES = $(addprefix ${SRC_DIR}/, ${_CLIENT_FILES})

SERVER_NAME = s
_SERVER_FILES = server/delay/delay.c
SERVER_FILES = $(addprefix ${SRC_DIR}/, ${_SERVER_FILES})
_SERVER_DEPS = server.o lib.o
SERVER_DEPS = $(addprefix ${OBJ_DIR}/, ${_SERVER_DEPS})


all: ${BIN_DIR} ${BIN_DIR}/${SERVER_NAME} ${BIN_DIR}/${CLIENT_NAME}

${BIN_DIR}:
	mkdir -p $@

${BIN_DIR}/${SERVER_NAME}: ${SERVER_DEPS} ${SERVER_FILES}
	${CC} ${CC_FLAGS} -DDELAY=0 -o $@ $^ ${LINKED_LIBS}

${BIN_DIR}/${CLIENT_NAME}: ${CLIENT_FILES}
	${CC} ${CC_FLAGS} -o $@ $^ ${LINKED_LIBS}
	
# server.o: server.c
# 	gcc -Wall -c -o server.o server.c

# lib.o: lib.c lib.h
# 	gcc -Wall -c -o lib.o lib.c

clean:
	rm -f ${BIN_DIR}/${SERVER_NAME} ${BIN_DIR}/${CLIENT_NAME}
