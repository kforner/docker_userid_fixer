simple test for docker_userid_fixer
==================================

## Overview

This example docker container configures a user **user1** which should have the default userID 1000 (default on ubuntu for the first user).

We define a docker entrypoint: `ENTRYPOINT ["/usr/local/bin/docker_userid_fixer","user1"]` 
which means that the entrypoint should switch to the user **user1** if the container is set to 
a *previously non-existing user* using the `docker run --user` feature.

## Test it

N.B: Type `make build` to build the example docker image before testing.


### default user 

Using the default user (i.e. using `docker run` without the `--user` argument:

```
%make id/default
uid=1000(user1) gid=1000(user1) groups=1000(user1)
```

The **user1** IDs are unchanged.

### setting a custom userID

Now let's use the `--user` argument to set a custom userID:

```
make id/custom 
uid=2222(user1) gid=3333 groups=3333
```

Now **user1** has our custom IDs. You may set other IDs by overridding the Makefile UID and GID variables:
```
%make id/custom  UID=1111 GID=staff
uid=1111(user1) gid=50(staff) groups=50(staff)
```

## Further checking: Home directory and shell profile

I had to take care in order to fix the home directory and have the shell profile working properly.
We can check that:

Using the default user:
```
%make bash/default
$pwd
/home/user1
$ alias
alias alert='notify-send --urgency=low -i "$([ $? = 0 ] && echo terminal || echo error)" "$(history|tail -n1|sed -e '\''s/^\s*[0-9]\+\s*//;s/[;&|]\s*alert$//'\'')"'
alias egrep='egrep --color=auto'
alias fgrep='fgrep --color=auto'
alias grep='grep --color=auto'
alias l='ls -CF'
alias la='ls -A'
alias ll='ls -alF'
```

Using a customized user:
```
%make bash/custom
$id
uid=2222(user1) gid=3333 groups=3333

$pwd
/home/user1
$ alias
alias alert='notify-send --urgency=low -i "$([ $? = 0 ] && echo terminal || echo error)" "$(history|tail -n1|sed -e '\''s/^\s*[0-9]\+\s*//;s/[;&|]\s*alert$//'\'')"'
alias egrep='egrep --color=auto'
alias fgrep='fgrep --color=auto'
alias grep='grep --color=auto'
alias l='ls -CF'
alias la='ls -A'
alias ll='ls -alF'
```



