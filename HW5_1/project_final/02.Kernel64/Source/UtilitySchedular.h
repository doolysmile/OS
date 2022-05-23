#ifndef __UTILITYSCHEDULAR_H__
#define __UTILITYSCHEDULAR_H__

#include "Types.h"
#include "Task.h"
#include "List.h"

int kCalculateTicket(QWORD listCount);
QWORD kGetRandom(int totaltickets);
QWORD kGetMinPass(LIST* list);
#endif