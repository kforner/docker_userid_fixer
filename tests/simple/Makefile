TOPLEVEL=../..

IMAGE=docker_userid_fixer_simple_example

build/exe:
	$(MAKE) -C $(TOPLEVEL) build

build: build/exe
	docker build --build-context exec=$(TOPLEVEL) -t $(IMAGE) .

UID=2222
GID=3333

DEBUG=

DOCKER_RUN=docker run -ti --rm -e DUF_DEBUG="$(DEBUG)"
DOCKER_RUN_CUSTOM = $(DOCKER_RUN) --user $(UID):$(GID)
DOCKER_RUN_ROOT = $(DOCKER_RUN) --user 0:0

id/default:
	@$(DOCKER_RUN) $(IMAGE) id

id/custom:
	@$(DOCKER_RUN_CUSTOM) $(IMAGE) id

id/root:
	@$(DOCKER_RUN_ROOT) $(IMAGE) id

bash/default:
	@$(DOCKER_RUN) $(IMAGE) bash

bash/custom:
	@$(DOCKER_RUN_CUSTOM) $(IMAGE) bash