#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<time.h>
#include<errno.h>
#include<string.h>
#include<fcntl.h>
#define SIZE 1024
static int InFd,OutFd;
volatile sig_atomic_t readFlag = 0;
FILE *InFilefp,*OutFilefp;
int writeToFile(char* buff);
int writetoConsole(char* incomingStr);
void readFromFile()
{
    readFlag=0;
    char* buff=NULL;
    buff = (char *)malloc(SIZE*sizeof(char));
    size_t len = sizeof(buff);
    int rd = (int)getline(&buff,&len,InFilefp);
    buff[rd] = '\0';
    printf("%s is buff in the readfile\n",buff);
    writetoConsole(buff);
    fflush(stdout);
    fflush(stdin);
    free(buff);
}

void readFromConsole()
{
    readFlag=1;
    time_t curtime;
    time(&curtime);
    char* buff2 = NULL;
    buff2 = (char *)malloc(SIZE*sizeof(char));
    int rd =(int)read(0,buff2,SIZE);
    buff2[rd-1] = '\0';
    fflush(stdout);
    fflush(stdin);
    writeToFile(buff2);
    free(buff2);
}

int writeToFile(char* inStr)
{
    time_t curtime;
    time(&curtime);
    strcat(inStr,",");
    strcat(inStr,ctime(&curtime));
    
    inStr[strlen(inStr)-1]='\n';
   
    write(OutFd,inStr,strlen(inStr));
    return 0;
    
}

int writetoConsole(char* incomingStr)
{
    printf("from buff %s\n",incomingStr);
    return 0;
}
int main(int argc,char* argv[])
{
    char inFile[100];
    char outFile[100];
    char sample[100];
    strcpy(sample,"aspfxyzzyspoon\0");
    strcpy(inFile,argv[1]);
    strcpy(outFile,argv[2]);
    InFilefp=fopen(inFile,"r");
    //OutFilefp=fopen(outFile,"w+");
    OutFd = open(outFile,O_WRONLY | O_CREAT | O_TRUNC,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    while(1)
    {
        if (readFlag == 1) {
            readFromFile();
        }
        else if (readFlag == 0){
            readFromConsole();
        }
    }
    readFromConsole();
    close(OutFd);
    fclose(InFilefp);

    fclose(OutFilefp);

}