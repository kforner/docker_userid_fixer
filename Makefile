
# N.B: shamelessly borrowed from https://github.com/ncopa/su-exec/blob/master/Makefile
CFLAGS ?= -Wall -Werror -g
LDFLAGS ?=

PROG := docker_userid_fixer
SRCS := $(PROG).c

build:
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LDFLAGS)
