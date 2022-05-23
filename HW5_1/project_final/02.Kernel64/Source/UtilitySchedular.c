#include "UtilitySchedular.h"

int kCalculateTicket(QWORD bPriority){
    int ticket = 0;
    switch (bPriority){
        case TASK_FLAGS_HIGHEST:
            ticket = 91;
            break;
        case TASK_FLAGS_HIGH:
            ticket = 55;
            break;
        case TASK_FLAGS_MEDIUM:
            ticket = 28;
            break;
        case TASK_FLAGS_LOW:
            ticket = 10;
            break;
        case TASK_FLAGS_LOWEST:
            ticket = 1;
            break;
    }
    return ticket;
}
// 난수를 발생시키기 위한 변수
static volatile QWORD gs_qwRandomValue = 0;

// 임의의 난수를 반환
QWORD kGetRandom(int totaltickets)
{
    gs_qwRandomValue = ( gs_qwRandomValue * 412153 + 5571031 ) >> 16;
    return gs_qwRandomValue % (totaltickets + 1);
}

QWORD kGetMinPass(LIST* list)
{
	QWORD pass, min = MAXQWORD;
	TCB* tmp;
	for(int i=0; i<TASK_MAXREADYLISTCOUNT; i++)
	{
		tmp = (TCB*)kGetHeaderFromList(&list[i]);
		while(tmp)
		{
			if( min >= tmp->pass)
			{
				min = tmp->pass;
			}
			tmp = kGetNextFromList(&list[i], tmp);
		}
	}
	if(min == MAXQWORD)
		min = 0;
	return min;
}