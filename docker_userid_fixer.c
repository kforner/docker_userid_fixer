/*
Copyright Karl Forner - karl.forner@gmail.com
docker_userid_fixer.c: utility meant to be used as a docker ENTRYPOINT that switches
to a predefined user using if applicable the user ids set by the `--user` argument of the 
`docker run` command.

cf https://github.com/kforner/docker_userid_fixer

*/

// for asprintf
#define _GNU_SOURCE 
#include <stdio.h>

// for get*uid, getpwnam
#include <sys/types.h>
#include <pwd.h>

// for getgrgid
#include <grp.h>

// for err, warnx
#include <err.h>
#include <errno.h>
#include <grp.h>

// for system, free, getenv
#include <stdlib.h>
// for strlen
#include <string.h>
// for getuid, setreuid
#include <unistd.h>

void usage() {
    warnx("usage: docker_userid_fixer username cmd [arg1 arg2 ... argn]");
    warnx("you can enable the debug mode by setting the environment variable DUF_DEBUG to a non-empty string");
}

// fetches user information (from /etc/passwd) given a user name
struct passwd * fetch_user_info_by_name(char* user_name) {
  struct passwd *pw = getpwnam(user_name);
  // N.B: pw must NOT be freed()
  if (pw == NULL)
    errx(1, "Error, could not get user information: getpwnam(%s)", user_name);
  return pw;
}

// fetches group information (from /etc/group) given a group name
struct group* fetch_group_info_by_id(gid_t group_id) {
  struct group *grp = getgrgid(group_id);
    // N.B: grp must NOT be freed()
  // if (grp == NULL)
  //   errx(1, "Error, could not get group information: getgrgid(%i)", group_id);
  return grp;
}

// modify the user ID of a user (using the usermod command)
void modify_user_id(char* username, uid_t new_uid) {
  char* cmd = NULL;
  int nb = asprintf(&cmd, "usermod -u %i %s", new_uid, username);
  if (nb < 0) err(1, "asprintf failed");

  int code = system(cmd);
  if (code != 0) errx(code, "cmd '%s' failed with code %i", cmd, code);

  free(cmd);
}

// modify the user ID of a user (using the groupmod command)
void modify_group_id(char* group_name, uid_t new_gid) {
  char* cmd = NULL;
  int nb = asprintf(&cmd, "groupmod -o -g %i %s", new_gid, group_name);
  if (nb < 0) err(1, "asprintf failed");

  int code = system(cmd);
  if (code != 0) errx(code, "cmd '%s' failed with code %i", cmd, code);

  free(cmd);
}

// // create a group in /etc/group for a given group id
// void create_group_for_id(char* group_name, gid_t group_id) {
//   char* cmd = NULL;
//   int nb = asprintf(&cmd, "groupadd -g %i %s", group_id, group_name);
//   if (nb < 0) err(1, "asprintf failed");

//   int code = system(cmd);
//   if (code != 0) {
//     errx(code, "cmd '%s' failed with code %i", cmd, code);
//   }

//   free(cmd);
// }

int main(int argc, char *argv[])
{
  // DEBUG mode? enabled by setting the DUF_DEBUG env var to a non empty string
  char* debug = getenv("DUF_DEBUG");
  int DEBUG = debug != NULL ? strlen(debug) > 0 : 0;
  int code = -1;

  // argument processing
  if (argc < 3) {
    usage();
    return 1;
  }

	char *user = argv[1];
	char **cmdargv = &argv[2];

  // N.B: uid is the real user ID, i.e the user ID with which we should run the processes
  // in the docker container
	uid_t uid = getuid();
  gid_t gid = getgid();

  if (DEBUG) {
    // display the effective and real user IDs
    uid_t euid = geteuid();
    gid_t egid = getegid();
    warnx("effective user id=%i", euid);
    warnx("effective group id=%i", egid);
    warnx("real user id=%i", uid);
    warnx("real group id=%i", gid);
  }

  if (DEBUG) {
    // fetches user info [from /etc/passwd]
    struct passwd *user_info = getpwuid(uid);
    if (user_info == NULL) {
      // no user info --> probably because `docker run --user $id`was used, that creates a pseudo
      // user that does not exist in /etc/passwd --> use docker_userid_fixer
      if (DEBUG) warnx("no user info for uid %i", uid);
    } else {
      warnx("real user name=%s", user_info->pw_name);
      warnx("real user dir=%s", user_info->pw_dir);
      warnx("NOTHING TO CHANGE");
    }
  }
  
  // ================================================================================

  // fetches user information for the target user 
  struct passwd* target_user_info = fetch_user_info_by_name(user);
  // this should not happen since the target user must exist
  if (target_user_info == NULL) errx(1, "can not fetch info for user %s", user);

  uid_t tuid = target_user_info->pw_uid;

  // N.B:
  // - if we are running under root --> nothing to do
  // - the the real userID already matches the target userID --> nothing to do
  // - otherwise we'll modify the IDs of the target user to match the current (requested) IDs
  if (uid != 0 && uid != tuid) {
    // switch the process to be completely root: currently only the effective userID should be root
    // now we are also setting the real user ID to root, which fixes issues with syscalls when forking
    // (as when using system() )
    code = setreuid(0, 0);
    if (code != 0) err(code, "setreuid(0, 0) failed");

    // we also must set the HOME var explicitly 
    setenv("HOME", target_user_info != NULL ? target_user_info->pw_dir : "/", 1);

    // we now change the ID of the target user using `usermod`
    modify_user_id(user, uid);
    if (DEBUG)
      warnx("modified user %s id from %i to %i", user, tuid, uid);

    // groups: if the real group id != target user default group id --> change it
    if (gid != target_user_info->pw_gid) {
      // we need the name if of the target user default group
      struct group *grp = fetch_group_info_by_id(target_user_info->pw_gid);
      if (grp == NULL) {
        warnx("Warning: the target user default group %i does not exist in /etc/group", target_user_info->pw_gid);
        // the target user default group does not exist in /etc/group
        // this should not happen --> do nothing for now
      } else {
        // change the ID of target user default group 
        if (DEBUG) warnx("modifying group %s with gid %i", grp->gr_name, gid);
        modify_group_id(grp->gr_name, gid);
        // N.B: if the group ID already exists in the docker !!!?!??
        // --> because we use "usermod -o" we end with duplicated group ids in /etc/group. For now not a big deal
      }

    }
  }

  if (DEBUG)
    warnx("executing cmd under user %s...", user);

  // we now switch back from root to our requested user (that is now the same as the target user)
  code = setreuid(uid, uid);
  if (code != 0) err(code, "setreuid(uid, uid) failed");

  // we execute the requested command in the same process
	execvp(cmdargv[0], cmdargv);

  // N.B: execvp() should never return, unless on error
	return 1;
}