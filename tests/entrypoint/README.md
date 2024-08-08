test for docker_userid_fixer: reusing existing entrypoint
=========================================================

## Overview

This example docker container configures a user **user1** which should have the default userID 1000 (default on ubuntu for the first user).

The pre-existing entrypoint `entrypoint.sh` is a simple shell script that prints the user IDs, 
and add a message using a parameter.

We now supercharge this existing entrypoint with docker_userid_fixer:

```
ENTRYPOINT ["/usr/local/bin/docker_userid_fixer","user1","/usr/local/bin/entrypoint.sh"]
```

## Testing it

N.B: Type `make build` to build the example docker image before testing.


### default user IDs

Using the default user (i.e. using `docker run` without the `--user` argument:

```
%make run/default 
id=uid=1000(user1) gid=1000(user1) groups=1000(user1): using default user
```

The entrypoint still works as expected, prints the user with its default IDs, and accept 
an argument: `default user`.


### using custom user ID

```
%make run/custom 
id=uid=2222(user1) gid=3333 groups=3333: using customized user
```

The entrypoint still works as expected, prints the user with its custom IDs, and accept 
an argument: `customized user`.