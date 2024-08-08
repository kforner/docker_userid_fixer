FROM ubuntu:22.04

COPY entrypoint.sh /usr/local/bin
RUN chmod a+rx /usr/local/bin/entrypoint.sh

COPY --from=exec docker_userid_fixer /usr/local/bin/
# configure setuid root
RUN cd /usr/local/bin/ && chown root:root docker_userid_fixer && chmod u+s docker_userid_fixer

# RUN apt install -y sudo
# RUN echo "ALL ALL=NOPASSWD:ALL" >> /etc/sudoers

RUN useradd -ms /bin/bash user1
USER user1


# USER root
WORKDIR /home/user1
ENTRYPOINT ["/usr/local/bin/docker_userid_fixer","user1","/usr/local/bin/entrypoint.sh"]
