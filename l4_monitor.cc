#include "l4_monitor.h"
#include <iostream>
#include <thread>
#include <unistd.h> //sleep
#include <string>

#include <stdlib.h>//rand
#include <time.h>
using namespace std;
int M=8;

class BUFFER : private Monitor
{
    struct Node
    {
        int key;
        string mssg;
        Node *next;
    };
// private member
private:
    Condition empty, full;
    Node *head, *tprior, *tail; //pointer to the first Node, tail of priority and tail
// public member
public:
    int nr_m; //number or items in the list
    // constructor
    BUFFER()
    {
        nr_m = 0;
        head = NULL; // set head to NULL
        tprior = NULL;
        tail= NULL;
    }
    // destructor
    ~BUFFER()
    {
        tprior=NULL;
        nr_m = 0;
        Node *next = head;
        while(next)
        {
            Node *temp = next;
            next = next->next;
            delete temp;
        }
    }
    // This prepends a new value at the beginning of the list
    void addm(string _mssg, int _key)
    {
        enter();
        //cout<<"nr "<<nr_m<<" M "<<M<<endl;
        while(nr_m == M)wait(full);
        Node *n = new Node();   // create new Node
        n->key = _key;
        n->mssg = _mssg;
        nr_m++;
	cout<<"Added "<<_mssg.substr(0,16)+" nr_m "<<nr_m<<endl;
        if (head == NULL) //if no nodes
        {
            n->next = NULL;
            head = tail= n;
            if (_key == 1)
                tprior=NULL;//if first message is regular
            else
                tprior = n;
        }//0 for high priority messages
        else if(head != NULL && _key == 0)
        {
            if (tprior == NULL) //if no prior messages
            {
                n->next = head;
                head = tprior = n;
            }
            else
            {
                n->next=tprior->next;
                tprior->next = n;
                tprior = n;
            }
            if (tprior->next == NULL)
                tail = n;
        }//1 for low priority messages
        else if(head != NULL && _key == 1 && tprior == NULL)
        {
            tail->next=n;
            n->next = NULL;
            tail = n;
        }
        else if(head != NULL && _key == 1 && tprior->next == NULL)
        {
            tprior->next=n;
            n->next = NULL;
            tail = n;
        }
        else if(head != NULL && _key == 1 && tprior->next != NULL)
        {
            tail->next=n;
            n->next = NULL;
            tail = n;
        }
        /*	cout<<"PRINTING CURRENT STORED MESSAGES"<<endl;
        fflush(stdout);
        	print();
        fflush(stdout);*/
        if(nr_m == 1)signal(empty);
        leave();
    }
    // returns the first element in the list and deletes the Node.
    string popm()
    {
        enter();
        if(nr_m ==0) wait(empty);
        Node *n = head;
        string m = n->mssg;
        head = head->next;
        if (n==tprior)
            tprior = NULL;
        delete n;
        nr_m--;
        cout<<"Popped\n"<<m<<endl;
        if (nr_m == M-1)signal(full);
        leave();
        return m;
    }
    void print()
    {
        Node *curr;
        curr=head;
        while(curr != NULL)
        {
            cout << curr->mssg<<" "<<curr->key;
            if (curr == tprior)
                cout<<" <- tprior\n";
            else
                cout<<"\n";
            curr = curr->next;
        }
    }
    int bufsize()
    {
        return nr_m;
    }
} buf; //BUFFER OBJECT
//-=-=-=-=-=-=-=-=-=-= END CLASS BUFFER -=-=-=-=-=-=-=-=-=-=-=

// RANDOM TEXT GENERATOR
string textgen()
{
    string word="";
    string abc="ABCDEFGHIJKLMNOPQRSTUVWXYzabcdefghijklmnopqrstuvwxyz";
    int rnd=(rand()%112)+16;
    for(int i=0; i<rnd; i++)
    {
        word = word + abc[rand()%abc.length()];
    }
    return word;
}
// -= -= -= -= -= -= -= -= -= -= CLIENT
void client(int tid)
{
    string txt;
    int pri;//priority
    srand(time(NULL));

    
    for(int i=0; i<30; i++)
    //while(true)
    {//usleep(500000);//for infinite loop test
        //pri=(tid==0)?1:(tid==4)?0:(rand()%10)%2;
	pri=1;
//(rand()%10)%2;//if client = 0, it is high priority
//	pri=(tid==4)?1:(rand()%10)%2;//if client = 4, it is low priority
        txt = "Pri "+to_string(pri)+" - Cli "+to_string(tid+1)+" #"+to_string(i+1)+" "+textgen();

//cout << "Pri "+to_string(pri)+" - Cli "+to_string(tid+1)+" #"+to_string(i+1)<<endl;
        buf.addm(txt,pri);
    }

}
// -= -= -= -= -= -= -= -= -= -= READER
void reader(int tid)
{
//for(int i=0;i<20;i++)
    while(true)
    {
        buf.popm();
//if (buf.bufsize()==0)
//	break;
    }

//cout<<tid<<endl;

}

void fillbuffer(){
string txt;
    int pri;//priority
    srand(time(NULL));
for (int i=0; i<7;i++)
	{
	pri=0;
	txt = "Pri "+to_string(pri)+" - Cli "+to_string(6)+" #"+to_string(i+1)+" "+textgen();
	buf.addm(txt,pri);
	}
}


int main()
{
    thread t[6];
    fillbuffer();
    for (int i=0; i<6; ++i)
    {
        if (i<5)
            t[i] = thread(client,i);
        else
        {
            cout<<"\n-=-=-=-=-=-=-=-=-=-= Reader sleeping, press enter"<<endl;
            cin.ignore();
            cout<<"\n-=-=-=-=-=-=-=-=-=-= Reader start popping messages out from buffer"<<endl;
            t[i] = thread(reader,i);
        }
    }

/*
cout<<"\n-=-=-=-=-=-=-=-=-=-= Reader start popping messages out from buffer\nEmpty Buffer\n"<<endl;
    t[5]=thread(reader,5);
cout<<"\n-=-=-=-=-=-=-=-=-=-= clients sleeping press enter to wake them up"<<endl;
cin.ignore();
        t[0] = thread(client,0);
        t[1] = thread(client,1);
        t[2] = thread(client,2);
        t[3] = thread(client,3);
        t[4] = thread(client,4);
       */ 
    



    for (int i=0; i<5; ++i)
    {
        t[i].join();//pauses until they finish
    }
    sleep(1);
    cout<<"Buffer is empty"<<endl;
    //t[5].detach();
    t[5].join();
    return 0;
}


