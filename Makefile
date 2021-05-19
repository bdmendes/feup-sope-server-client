CC=gcc
CC_FLAGS=-Wall -Werror
LINKED_LIBS=-pthread -lrt

BIN_DIR=bin
OBJ_DIR=obj
SRC_DIR=src

CLIENT_NAME = c
_CLIENT_FILES = client.c common/utils/utils.c client/parser/parser.c common/message/message.c common/fifo/fifo.c common/timer/timer.c common/log/log.c client/request/request.c
CLIENT_FILES = $(addprefix ${SRC_DIR}/, ${_CLIENT_FILES})

SERVER_NAME = s
_SERVER_FILES = server.c server/message_queue/message_queue.c common/message/message.c common/timer/timer.c common/log/log.c common/fifo/fifo.c server/lib/lib.c server/producer_consumer/producer_consumer.c server/lib/delay.c
SERVER_FILES = $(addprefix ${SRC_DIR}/, ${_SERVER_FILES})

all: ${BIN_DIR} ${BIN_DIR}/${SERVER_NAME} ${BIN_DIR}/${CLIENT_NAME}

${BIN_DIR}:
	mkdir -p $@

${BIN_DIR}/${SERVER_NAME}: ${SERVER_FILES}
	${CC} ${CC_FLAGS} -o $@ $^ ${LINKED_LIBS}

${BIN_DIR}/${CLIENT_NAME}: ${CLIENT_FILES}
	${CC} ${CC_FLAGS} -o $@ $^ ${LINKED_LIBS}

clean:
	rm -f ${BIN_DIR}/${SERVER_NAME} ${BIN_DIR}/${CLIENT_NAME}
