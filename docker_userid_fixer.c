// for asprintf
#define _GNU_SOURCE 
// for get*uid
#include <sys/types.h>

// for err, warn
#include <err.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


// return -1 if not integer
long string_to_integer(char* s) {
  char* end = NULL;
  long res = strtol(s, &end, 10);
  if (*end != '\0') return -1;
  return res;
}

struct passwd * fetch_user_info_by_name(char* user_name) {
  struct passwd *pw = getpwnam(user_name);
  if (pw == NULL)
    err(1, "getpwnam(%s)", user_name);
  return pw;
}


void modify_user_id(char* username, uid_t new_uid) {
  char* cmd = NULL;
  int nb = asprintf(&cmd, "usermod -u %i -o %s", new_uid, username);
  if (nb < 0) err(1, "asprintf failed");

  int code = system(cmd);
  if (code != 0) err(code, "cmd '%s' failed with code %i", cmd, code);

  free(cmd);
}

// void create_user(char* username, uid_t uid) {
//   char* cmd = NULL;
//   int nb = asprintf(&cmd, "useradd -u %i -o %s", uid, username);
//   if (nb < 0) err(1, "asprintf failed");

//   int code = system(cmd);
//   if (code != 0) err(code, "cmd '%s' failed with code %i", cmd, code);

//   free(cmd);
// }

// void delete_user(char* username) {
//   char* cmd = NULL;
//   int nb = asprintf(&cmd, "deluser %s", username);
//   if (nb < 0) err(1, "asprintf failed");

//   int code = system(cmd);
//   if (code != 0) err(code, "cmd '%s' failed with code %i", cmd, code);

//   free(cmd);
// }


int main(int argc, char *argv[])
{
  // char* argv0 = argv[0];
	if (argc < 3) {
    warnx("usage: username cmd ...");
    return 1;
  }

  char* debug = getenv("DUF_DEBUG");
  int DEBUG = debug != NULL ? strlen(debug) > 0 : 0;

	char *user = argv[1];
	char **cmdargv = &argv[2];


	uid_t uid = getuid();

  if (DEBUG) {
    uid_t euid = geteuid();
    gid_t gid = getgid();
    warnx("effective user id=%i", euid);
    warnx("real user id=%i", uid);
    warnx("real group id=%i", gid);
  }


  if (DEBUG) {
    struct passwd *user_info = getpwuid(uid);
    if (user_info == NULL) {
      warnx("no user info for uid %i", uid);

    } else {
      warnx("real user name=%s", user_info->pw_name);
      warnx("real user dir=%s", user_info->pw_dir);
      warnx("NOTHING TO CHANGE");
    }
  }

  struct passwd* target_user_info = fetch_user_info_by_name(user);
  if (target_user_info == NULL) err(1, "can not fetch info for user %s", user);

  uid_t tuid = target_user_info->pw_uid;

  if (uid != 0 && uid != tuid) {

    setreuid(0, 0);

    setenv("HOME", target_user_info != NULL ? target_user_info->pw_dir : "/", 1);

    modify_user_id(user, uid);
    if (DEBUG)
      warnx("modified user %s id from %i to %i", user, tuid, uid);


    // target_user_info->pw_uid = uid;
    // target_user_info->pw_gid = gid;

    // FILE* fd = fopen("/etc/passwd", "a");
    // int code = putpwent(target_user_info, fd);
    // if (code != 0) err(code, "putpwent failed");
    // fclose(fd);

    // setenv("HOME", target_user_info != NULL ? target_user_info->pw_dir : "/", 1);
  }


  if (DEBUG)
    warnx("executing cmd under user %s...", user);

  setreuid(uid, uid);
	execvp(cmdargv[0], cmdargv);

	return 1;
}