#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

struct my_msgbuf {
    long mtype;
    char mtext[200];
};
struct rcv_msgbuf {
    long mtype;
    char mtext[200];
};
void* recieve()
{
struct rcv_msgbuf recmsg;
int maxind, ind, msqid;
key_t key;

struct msginfo msginfo;
/* Obtain size of kernel 'entries' array */
msqid = msgget(key,IPC_CREAT | 0666);
		if(msqid == -1)
		{
			perror("msgget");
			exit(EXIT_FAILURE);
		}
		
		int msgrec;
		msgrec = msgrcv(msqid, &recmsg, sizeof recmsg.mtext, 0,0);
		if(msgrec == -1)
		{
			perror("msgrec");
			exit(EXIT_FAILURE);
		}
        printf("spock: \"%s\"\n", recmsg.mtext);
exit(EXIT_SUCCESS);
}


int main(int argc,char* argv[])
{
int maxind, ind,msqidint;
struct msqid_ds ds;
struct my_msgbuf buf;
char buff[102];
char msg[1024];
struct msginfo msginfo;
int msqid,msqid2;
pthread_t pth;
    key_t key;

	
    key = (key_t)getpid();

    if ((msqid = msgget(key, 0666 | IPC_CREAT)) == -1) {
        perror("msgget");
        exit(1);
    }
    
    printf("Enter lines of text, ^D to quit:\n");

    buf.mtype = 1; /* we don't really care in this case */

    while(fgets(buf.mtext, sizeof buf.mtext, stdin) != NULL) 
{
        
  int len = strlen(buf.mtext);

        /* ditch newline at end, if it exists */
        if (buf.mtext[len-1] == '\n') buf.mtext[len-1] = '\0';

	maxind = msgctl(0, MSG_INFO,(struct msqid_ds*)&msginfo);
		if(maxind == -1)
		{
			perror("msgctl");
			exit(EXIT_FAILURE);
		}
		for(ind = 0; ind <= maxind; ind++)
		{
			msqidint = msgctl(ind, MSG_STAT, &ds);
			if(msqidint == -1)
			{
				if(errno != EINVAL && errno != EACCES)
				{
					continue;
				}
			}
			if((int)ds.msg_perm.__key != (int)getpid())
			{
				int tempmsqid;
				size_t length = strlen(msg); 
				tempmsqid = msgget(ds.msg_perm.__key,IPC_CREAT | 0666);
				if(msgsnd(tempmsqid, &buf.mtext, length+1,0) == -1)
				{
					perror("msgsnd");
					exit(EXIT_FAILURE);
				}
			}

		}
	
}

pthread_create(&pth,NULL,recieve,NULL);
pthread_join(pth,NULL);
	recieve();

    return 0;
}
