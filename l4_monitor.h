#ifndef __l4_monitor_h
#define __l4_monitor_h

#include <semaphore.h>



#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <iostream>

class Semaphore{
	int semid;
	public:
	Semaphore(key_t, int, int);
	~Semaphore();
	int p();
	int v();
};

union semun{
	int val;
	struct semid_ds *buf;
	unsigned short int *array;
	struct seminf *__buf;
};

//DEFINITION

Semaphore::Semaphore(key_t key, int semFlags, int value){
	semid =  semget(key, 1, semFlags);
	union semun argument;
	unsigned short values[1];
	values[0] = value;
	argument.array = values;

	//cout<<"Semaphore created"<<endl;

	semctl(semid, 0, SETALL, argument);
}

Semaphore::~Semaphore(){
	union semun ignored_argument;

	//cout<<"Semaphore destroyed"<<endl;

	semctl(semid, 1, IPC_RMID, ignored_argument);

}

int Semaphore::p(){

	//cout<<"Semaphore p"<<endl;

	struct sembuf op[1];
	op[0].sem_num = 0;
	op[0].sem_op = -1;
	op[0].sem_flg = SEM_UNDO;

return semop(semid, op, 1);;
}

int Semaphore::v(){

	//cout<<"Semaphore v"<<endl;

	struct sembuf op[1];
	op[0].sem_num = 0;
	op[0].sem_op = 1;
	op[0].sem_flg = SEM_UNDO;

	return semop(semid, op, 1);
}



















class Condition {
	friend class Monitor;
 private:
	Semaphore * s;//w
	int waitingCount; //threads waiting
 public://:s(123,0666 | IPC_CREAT,0)
	Condition(){
	s = new Semaphore(123,0666 | IPC_CREAT,0);
	waitingCount = 0;
	}
	void wait() {
	s->p();
	}
	bool signal() {
	if( waitingCount ) {
	--waitingCount;
	s->v();
	return true;
	}
	else
	return false;
	}
};

class Monitor {
 private:
	Semaphore * s;
 public://:s(123, 0666 | IPC_CREAT,1)
	Monitor(){
	s = new Semaphore(124, 0666 | IPC_CREAT,1);	
	}
	void enter() {
	s->p();
	}
	void leave() {
	s->v();
	}
	void wait( Condition & cond ) { 
	++cond.waitingCount;
	leave();
	cond.wait();
	}
	bool signal( Condition & cond ) {
	if( cond.signal() ) {
	enter();
	return true;
	}
	else
	return false;
	}
};

#endif
