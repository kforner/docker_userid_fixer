docker userid fixer (DUF)
========================

## overview

This is a utility intended to be used as a **docker entrypoint**, to help using a pre-configured docker user
with a user ID that matches the host user ID.
This enables the container to interact seamlessly with the host user filesystem.

A common use case is a docker container that will compile some source code from a user folder
and generate an executable in the same folder:
  - it needs permission to write into the user folder
  - after the execution, if the user IDs do not match, the (host) user can not use or delete the generated file.


## setup

### docker_userid_fixer executable

First you need the `docker_userid_fixer` executable.
The safest and reproducible way is to download this repository, and compile the executable using the compiler docker container:

```
git clone https://github.com/kforner/docker_userid_fixer.git
cd docker_userid_fixer
make build_in_docker
```

This will (hopefully) generates the `docker_userid_fixer` file in the current folder.

### configure docker_userid_fixer as your docker entrypoint

You can look at the [tests](./tests/) folder for some example Dockerfiles.
Let's see on an excerpt:
```
# copy the docker_userid_fixer executable in the docker image
COPY docker_userid_fixer /usr/local/bin/

# make it executable as setuid root
RUN cd /usr/local/bin/ && chown root:root docker_userid_fixer && chmod u+s docker_userid_fixer

# configure a docker container user "user1"
RUN useradd -ms /bin/bash user1
USER user1

# use docker_userid_fixer as entrypoint
ENTRYPOINT ["/usr/local/bin/docker_userid_fixer","user1"]
```

## usage

Once your docker image is setup, you use the `docker run` `--user` argument to switch to a user ID
that is convenient, usually your local computer user ID.

For example:
```
docker run --rm -ti --user `id -u` MY_IMAGE
```

This should start your container with your pre-configured user `user1`, but with customized user IDs
to match your local user ID.

## how it works?

When you run your docker container with a fixed userid ID1, using the docker run `--user` option:

  - `docker run` creates the container, creates a pseudo user with userid ID1, then runs the entrypoint
    under this pseudo user as a process with PID 1
  - the `docker_userid_fixer` entrypoint process, which has setuid root, has a **root** **effective user ID** (0)
  - it can know the requested user id ID1 since it is the **real user ID**
  - from the `user1` arg of the docker_userid_fixer entrypoint, it can get the **target userid**, i.e
    the userid of `user1`
  - if those IDs do not match, `docker_userid_fixer` modifies the userid of `user1` to ID1 using the `usermod` cli command
  - then it will set the real and effective user ids of the entrypoint process (still PID1) to ID1
  - and finally it will execute the command passed as argument to the entrypoint, still as PID1


