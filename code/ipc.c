/*
 * ipc.c
 *
 *  Created on: 21 oct. 2014
 *      Author: Fabien Poussin
 */

#include "ipc.h"

static msg_t buf1[4];
static msg_t buf2[4];
static msg_t buf3[4];
static msg_t buf4[4];

MAILBOX_DECL(knock_mb, buf1, sizeof(buf1)/sizeof(msg_t));
MAILBOX_DECL(vr1_mb, buf2, sizeof(buf2)/sizeof(msg_t));
MAILBOX_DECL(vr2_mb, buf3, sizeof(buf3)/sizeof(msg_t));
MAILBOX_DECL(vr3_mb, buf4, sizeof(buf4)/sizeof(msg_t));

static samples_message_t samples_messages[POOL_SIZE] __attribute__((aligned(sizeof(stkalign_t))));
static MEMORYPOOL_DECL(samples_pool, sizeof(samples_message_t), PORT_STACK_ALIGN, NULL);

void setupIPC(void)
{
  size_t i;
  chMBObjectInit(&knock_mb, buf1, sizeof(buf1) / sizeof(msg_t));
  chMBObjectInit(&vr1_mb, buf2, sizeof(buf2) / sizeof(msg_t));
  chMBObjectInit(&vr2_mb, buf3, sizeof(buf3) / sizeof(msg_t));
  chMBObjectInit(&vr3_mb, buf4, sizeof(buf4) / sizeof(msg_t));

  // We need to init and "free" each place.
  chPoolObjectInit(&samples_pool, sizeof(samples_message_t), NULL);
  for(i=0; i < POOL_SIZE; i++)
      chPoolFree(&samples_pool, &samples_messages[i]);
}

bool allocSendSamplesI(mailbox_t* mb, void * buffer, size_t size)
{
  if (buffer == NULL) return false;

  samples_message_t* info = chPoolAllocI(&samples_pool);

  if (info == NULL) return false;

  info->location = buffer;
  info->size = size;
  chMBPostI(mb, (msg_t)info);

  return true;
}

bool recvFreeSamples(mailbox_t* mb, void ** buffer, size_t * size, systime_t timeout)
{
  msg_t msg;
  samples_message_t data;

  if(chMBFetchTimeout(mb, &msg, timeout) != MSG_OK)
    return false;

  data = *(samples_message_t*)mb;
  chPoolFree(&samples_pool, (void*)mb);

  if (data.location == NULL) return false;

  *buffer = data.location;
  *size = data.size;

  return true;
}
