
# N.B: shamelessly borrowed from https://github.com/ncopa/su-exec/blob/master/Makefile
CFLAGS ?= -Wall -Werror -O2
LDFLAGS ?=

PROG := docker_userid_fixer
SRCS := $(PROG).c

build:
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LDFLAGS)

build_in_docker:
	$(MAKE) -C compiler
	docker run --rm -ti -v $(PWD):/code -w /code --user `id -u`:`id -g` docker_userid_fixer_compiler make build

clean:
	rm -f $(PROG)
