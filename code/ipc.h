/*
 * ipc.h
 *
 *  Created on: 21 oct. 2014
 *      Author: Fabien Poussin
 */

#ifndef IPC_H_
#define IPC_H_

#include "ch.h"

#define MSG_GO 0x1234ABCD
#define POOL_SIZE 10

extern mailbox_t knock_mb;
extern mailbox_t vr1_mb;
extern mailbox_t vr2_mb;
extern mailbox_t vr3_mb;

typedef struct {
  void* location;
  size_t size;
} samples_message_t;

void setupIPC(void);
bool allocSendSamplesI(mailbox_t* mb, void * buffer, size_t size);
bool recvFreeSamples(mailbox_t* mb, void ** buffer, size_t * size);

#endif /* IPC_H_ */
