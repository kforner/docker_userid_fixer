TOPLEVEL=..
IMAGE=docker_userid_fixer_compiler

build:
	docker build -t $(IMAGE) .

run:
	docker run --rm -ti -v $(PWD):/code -w /code --user `id -u` $(IMAGE) bash