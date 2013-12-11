#ifndef _SBF_COMMON_LINUX_H_
#define _SBF_COMMON_LINUX_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <arpa/inet.h>

#include <netinet/in.h>

#include <assert.h>
#include <ctype.h>
#include <dlfcn.h>
#include <errno.h>
#include <ifaddrs.h>
#include <limits.h>
#include <pthread.h>
#include <regex.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

SBF_BEGIN_DECLS

typedef int sbfSocket;

#define SBF_PACKED(d) d __attribute__ ((__packed__))
#define SBF_PRINTFLIKE(a, b) __attribute__ ((format (printf, a, b)))
#define SBF_UNUSED __attribute__((__unused__))
#define SBF_INLINE __inline

#define SBF_LIKELY(e) __builtin_expect (!!(e), 1)
#define SBF_UNLIKELY(e) __builtin_expect (!!(e), 0)

#define SBF_ASSERT(x) assert (x)

typedef pthread_t sbfThread;
#define sbfThread_create(t, cb, closure) pthread_create (t, NULL, cb, closure)
#define sbfThread_join(t) pthread_join (t, NULL)

typedef pthread_cond_t sbfCondVar;
#define sbfCondVar_init(c) pthread_cond_init (c, NULL)
#define sbfCondVar_destroy(c) pthread_cond_destroy (c)
#define sbfCondVar_wait(c, m) pthread_cond_wait (c, m)
#define sbfCondVar_broadcast(c) pthread_cond_broadcast (c)
#define sbfCondVar_signal(c) pthread_cond_signal (c)

typedef pthread_mutex_t sbfMutex;
#define sbfMutex_init(m) pthread_mutex_init (m, NULL)
#define sbfMutex_destroy(m) pthread_mutex_destroy (m)
#define sbfMutex_lock(m) pthread_mutex_lock (m)
#define sbfMutex_unlock(m) pthread_mutex_unlock (m)

typedef pthread_spinlock_t sbfSpinLock;
#define sbfSpinLock_init(s) pthread_spin_init (s, 0)
#define sbfSpinLock_destroy(s) pthread_spin_destroy (s)
#define sbfSpinLock_lock(s) pthread_spin_lock (s)
#define sbfSpinLock_unlock(s) pthread_spin_unlock (s)

SBF_END_DECLS

#endif /* _SBF_COMMON_LINUX_H_ */

