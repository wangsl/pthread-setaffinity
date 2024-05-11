
// gcc -fPIC -shared -Wl,-soname,libpthread-setaffinity.so -ldl -o libpthread-setaffinity.so pthread-setaffinity.c
// export LD_PRELOAD=libpthread-setaffinity.so

#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>

typedef int (*pthread_setaffinity_np_t)(pthread_t thread, size_t cpusetsize, const cpu_set_t *cpuset);

static pthread_setaffinity_np_t __orig_pthread_setaffinity_np = NULL;

static void *get_lib_function(const char *funcname)
{
  void *func = dlsym(RTLD_NEXT, funcname);
  if(func == NULL) {
    char *error = dlerror();
    fprintf(stderr, "Failed to locate lib function '%s' error: %s", funcname, error);
      exit(EXIT_FAILURE);
  }
  return func;
}

int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize, const cpu_set_t *cpuset)
{
  if(__orig_pthread_setaffinity_np == NULL) {
    __orig_pthread_setaffinity_np = (pthread_setaffinity_np_t) get_lib_function("pthread_setaffinity_np");
  }

  cpu_set_t cpuset_;
  CPU_ZERO(&cpuset_);
  CPU_SET(sched_getcpu(), &cpuset_);

  return __orig_pthread_setaffinity_np(thread, cpusetsize, &cpuset_);
}
