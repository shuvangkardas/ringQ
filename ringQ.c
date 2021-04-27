#include "ringQ.h"





struct ramq_t *ramqNew(void *baseAddr, uint32_t len)
{
	struct ramq_t *me = malloc(sizeof(struct ramq_t));
	if(me != NULL)
	{
		me -> _baseAddr = baseAddr;
		me -> _len = len;
		me -> _endAddr = (uint8_t*)baseAddr + len;

		ramqReset(me);
	}
	return me;
}

bool  ramqPush(struct ramq_t *me, void *dataPtr, uint16_t len)
{
	if(me -> _isLock == false)
	{
		len += sizeof( struct qObj_t);
		uint8_t *nextHead;

		//reset logic for pointer
		if(nextHead > me -> _endAddr)
		{
			me -> _head.ptr = me -> _baseAddr;
			// me -> _head.nextPtr =  nextHead;
			me -> _leadingHead = false;
		}else
		{
			me -> _head.ptr = me -> _head.nextPtr;
		}

		nextHead = me -> _head.ptr + len; //update data end

		//determine lock conditions
		if(me -> _leadingHead == false)
		{
			if(nextHead >= me -> _tail.ptr)
			{
				//need to reset of cannot save data 
				me -> _isLock = true;
				return false;
			}
		}
		

		// //initiate first tail nextptr
		// if(me -> _head.ptr == _baseAddr)
		// {
		// 	me -> _tail.nextPtr = nextHead;
		// }

		me -> _head.len = len - sizeof(struct qObj_t);
		me -> _head.nextPtr = nextHead;

		memcpy(me -> _head.ptr, &(me ->_head),sizeof(struct qObj_t));
		
		uint8_t *dataPtrHead = me -> _head.ptr + sizeof(struct qObj_t);
		memcpy(dataPtrHead,dataPtr,me -> _head.len);

		//increment to next head
		// me -> _head.ptr = nextHead;
		if( me -> _tail.ptr == me -> _tail.nextPtr)
		{
			memcpy(&(me -> _tail) , &(me -> _head), sizeof(struct qObj_t));
		}

		me -> _qState  = RUNNING;
		return true;
	}
	return false;
	
}


struct qObj_t *ramqPop(struct ramq_t *me)
{
	if(me -> _qState == RUNNING)
	{
		struct qObj_t *currentTailObj= &(me -> _tail);
		uint8_t *nextTailPtr = me -> _tail.nextPtr;

		

		//reset logic for pointer. next tail will be
		// automically point in posh operation

		if(nextTailPtr != me -> _head.nextPtr)
		{
			memcpy(&(me -> _tail), me -> _tail.nextPtr, sizeof(struct qObj_t));
			if(nextTailPtr < me -> _head.nextPtr)
			{
				me ->_leadingHead = true;	
			}
			
		}else{
			me -> _qState = NO_DATA;
			me -> _leadingHead = false;
		}

		//check data availability
		// if(_leadingHead)
		// {
		// 	if(nextTail >= me -> _head.ptr)
		// 	{
		// 		me -> _qState = NO_DATA;
		// 	}
		// }

		return currentTailObj;
	}
	return NULL;
}

void ramqReset(struct ramq_t *me)
{
	memset(me -> _baseAddr, '\0',me -> _len);
	me -> _head.ptr = me -> _baseAddr;
	me -> _head.nextPtr = me -> _baseAddr;
	me -> _head.len = NULL;

	me -> _tail.ptr = me -> _baseAddr;
	me -> _tail.nextPtr = me -> _baseAddr;
	me -> _tail.len = NULL;


	me -> _isLock = false;
	me -> _leadingHead = true;
	me -> _qState = RESET;
}

bool ramqIsLocked(struct ramq_t *ramq)
{
	return ramq ->_isLock;
}

