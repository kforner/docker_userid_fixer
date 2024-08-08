TOPLEVEL=../..

IMAGE=docker_userid_fixer_entrypoint_example

build/exe:
	$(MAKE) -C $(TOPLEVEL) build

build: build/exe
	docker build --build-context exec=$(TOPLEVEL) -t $(IMAGE) .

UID=2222
GID=3333

DEBUG=

DOCKER_RUN=docker run -ti --rm -e DUF_DEBUG="$(DEBUG)"
DOCKER_RUN_CUSTOM = $(DOCKER_RUN) --user $(UID):$(GID)

run/default:
	@$(DOCKER_RUN) $(IMAGE) default user

run/custom:
	@$(DOCKER_RUN_CUSTOM) $(IMAGE) customized user
