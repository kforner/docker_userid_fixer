FROM ubuntu:22.04

RUN apt update && apt install -y bash-completion

COPY --from=exec docker_userid_fixer /usr/local/bin/
# configure setuid root
RUN cd /usr/local/bin/ && chown root:root docker_userid_fixer && chmod u+s docker_userid_fixer


# RUN echo "ALL ALL=NOPASSWD:ALL" >> /etc/sudoers
RUN apt update && apt install -y bash-completion make sudo

# N.B: use a USER ID > 1000 to reproduce the case where the docker user ID does not match
# N.B: very important so set sudo as primary group since with docker_userid_fixer the other groups will be lost in
# the docker user session for a reason yet to be determined
RUN useradd -u 3456 -g sudo -s /bin/bash -m user1

# add additional group --> they currently disappear when the user id is modified. why ????!?!
RUN groupadd group2
RUN adduser user1 group2

# make it a sudoer
# RUN adduser user1 sudo
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
USER user1
# init the sudo to avoid annoying messages
RUN sudo id

# USER root
WORKDIR /home/user1
ENTRYPOINT ["/usr/local/bin/docker_userid_fixer","user1"]
