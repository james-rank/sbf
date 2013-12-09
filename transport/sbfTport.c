#include "sbfTport.h"
#include "sbfTportPrivate.h"

typedef struct
{
    sbfTport       mTport;
    sbfTportStream mTportStream;

    sbfMwThread    mThread;
    struct event   mEvent;
} sbfTportDestroyStreamClosure;

static void
sbfTportSecondDestroyStreamCb (int fd, short events, void* closure)
{
    sbfTportDestroyStreamClosure* closure0 = closure;
    sbfTport                      tport = closure0->mTport;

    pthread_cond_broadcast (&tport->mStreamsCond);

    free (closure0);
}

static void
sbfTportFirstDestroyStreamCb (int fd, short events, void* closure)
{
    sbfTportDestroyStreamClosure* closure0 = closure;
    sbfTportStream                tstream = closure0->mTportStream;
    sbfTportTopic                 ttopic;
    sbfPub                        pub;
    sbfSub                        sub;

    RB_FOREACH (ttopic, sbfTportTopicTreeImpl, &tstream->mTopics)
    {
        TAILQ_FOREACH (pub, &ttopic->mPubs, mEntry)
            sbfPub_destroy (pub);
        TAILQ_FOREACH (sub, &ttopic->mSubs, mEntry)
            sbfSub_destroy (sub);
    }

    /*
     * Queue to wakeup after the publisher and subscriber events (which will
     * have freed and removed the stream).
     */
    closure0->mTportStream = NULL;
    sbfMw_enqueue (closure0->mThread,
                   &closure0->mEvent,
                   sbfTportSecondDestroyStreamCb,
                   closure0);
}

static void
sbfTportFree (sbfTport tport)
{
    pthread_cond_destroy (&tport->mStreamsCond);
    pthread_mutex_destroy (&tport->mStreamsLock);

    pthread_mutex_destroy (&tport->mNextThreadLock);

    sbfKeyValue_destroy (tport->mProperties);

    free (tport);
}

sbfTport
sbfTport_create (sbfMw mw,
                 uint64_t mask,
                 const char* type,
                 sbfKeyValue properties)
{
    sbfTport         tport;
    sbfHandlerTable* table;
    sbfHandler       handler;

    table = sbfHandler_load (type);
    if (table == NULL)
        return NULL;

    tport = xcalloc (1, sizeof *tport);
    tport->mMw = mw;

    if (properties != NULL)
        tport->mProperties = sbfKeyValue_copy (properties);
    else
        tport->mProperties = sbfKeyValue_create ();

    tport->mThreadMask = mask;

    sbfLog_debug ("creating %p: type %s", tport, type);

    tport->mNextThread = 0;
    pthread_mutex_init (&tport->mNextThreadLock, NULL);

    TAILQ_INIT (&tport->mStreams);
    pthread_mutex_init (&tport->mStreamsLock, NULL);
    pthread_cond_init (&tport->mStreamsCond, NULL);

    handler = table->mCreate (tport, tport->mProperties);
    if (handler == NULL)
        goto fail;
    tport->mHandlerTable = table;
    tport->mHandler = handler;

    return tport;

fail:
    sbfTportFree (tport);
    return NULL;
}

void
sbfTport_destroy (sbfTport tport)
{
    sbfTportStream                tstream;
    sbfTportDestroyStreamClosure* closure0;

    sbfLog_debug ("destroying %p", tport);

    pthread_mutex_lock (&tport->mStreamsLock);
    TAILQ_FOREACH (tstream, &tport->mStreams, mEntry)
    {
        closure0 = xmalloc (sizeof *closure0);
        closure0->mTport = tport;
        closure0->mTportStream = tstream;

        closure0->mThread = tstream->mThread;
        sbfMw_enqueue (closure0->mThread,
                       &closure0->mEvent,
                       sbfTportFirstDestroyStreamCb,
                       closure0);
    }
    while (!TAILQ_EMPTY (&tport->mStreams))
    {
        sbfLog_debug ("waiting for streams");
        pthread_cond_wait (&tport->mStreamsCond, &tport->mStreamsLock);
    }
    pthread_mutex_unlock (&tport->mStreamsLock);

    tport->mHandlerTable->mDestroy (tport->mHandler);

    sbfLog_debug ("freeing %p", tport);
    sbfTportFree (tport);
}
