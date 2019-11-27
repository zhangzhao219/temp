#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define NAMELENGTH 20//定义进程PID名称的最大长度
#define RAM 5//规定内存中的道数
int count = RAM;//内存中的道数
int TotalTime = 1;//定义所有程序的运行时间，过程中有插入可更新时间，初始时间为1

//定义PCB块
typedef struct pcb{
    char PID[NAMELENGTH];//进程名
    int runningtime;//运行时间
    int priority;//优先权，数值越大优先权越高
    int status;//状态，运行态1，就绪态0，挂起态-1
    int property;//进程属性：0为独立进程，1非独立进程
    char FrontPID[NAMELENGTH];//前趋进程的PID
}pcb;



typedef struct Node{
    pcb PCB_contents;//PCB内容
    struct Node* Next;//PCB指向下一个PCB的指针
}PCB;

//定义队列结构
typedef struct Queue{
    PCB *Front;//队头指针
    PCB *Rear;//队尾指针
}Queue;

//初始化队列
void InitQueue(Queue *q){
    q->Front = q->Rear = (PCB*)malloc(sizeof(PCB));
    if((!q->Front)||(!q->Rear)){
        printf("创建队列失败！");
        return;
    }
    q->Front->Next = NULL;
}

//输入PCB信息
void InputPCBInformation(Queue *ReadyQueue,Queue *BackupQueue){
    int j;
    PCB *p = NULL;
    printf("请输入进程数：");
    int tempsum;//保存进程数
    scanf("%d",&tempsum);
    for(j=0;j<tempsum;j++){
        //录入进程的相关信息
        p = (PCB*)malloc(sizeof(PCB));
        printf("请输入第%d个进程的相关信息：\n",j+1);
        printf("进程名：");
        scanf("%s",p->PCB_contents.PID);
        printf("进程运行时间：");
        scanf("%d",&p->PCB_contents.runningtime);
        TotalTime += p->PCB_contents.runningtime;
        printf("进程优先权：");
        scanf("%d",&p->PCB_contents.priority);
        printf("进程是否为独立进程？为独立进程请输入0，为非独立进程请输入1：");
        scanf("%d",&p->PCB_contents.property);
        if(p->PCB_contents.property == 1){
                printf("请输入前趋进程的进程名：");
                scanf("%s",p->PCB_contents.FrontPID);
        }
        p->Next = NULL;
    
        //判断内存中道数数量是否足够，数量够将进程送入就绪队列中，数量不够将进程送入后备队列中
        if(count <= 0){
            p->PCB_contents.status = -1;
            BackupQueue->Rear->Next = p;
            BackupQueue->Rear = p;
        }
        else{
            p->PCB_contents.status = 0;
            ReadyQueue->Rear->Next = p;
            ReadyQueue->Rear = p;
            count--;
        }
    }
};

void SortProcess(Queue *ReadyQueue){
    if(ReadyQueue->Front == ReadyQueue->Rear){
        return;
    }
    PCB *Head = ReadyQueue->Front;
    PCB *p, *q = NULL;
    pcb t;//中间变量

    //将就绪队列中的进程按照优先级进行排序
    for(q = Head->Next;q->Next != NULL;q = q->Next){
        for(p = Head->Next;p->Next != NULL;p = p->Next){
            if(p->PCB_contents.priority < p->Next->PCB_contents.priority){
                t = p->PCB_contents;
                p->PCB_contents = p->Next->PCB_contents;
                p->Next->PCB_contents = t;
            }
        }
    }
};
//打印就绪队列中的进程
void PrintReadyQueue(Queue *ReadyQueue){
    PCB *p = ReadyQueue->Front;
    if(ReadyQueue->Front == ReadyQueue->Rear){
        printf("就绪队列中没有进程！\n");
    }
    else{
        printf("\n");
        printf("进程名\t进程运行时间\t进程优先权\t前趋进程进程名\n");
        while(p->Next!=NULL){
            printf("%s\t%d\t%d\t",p->Next->PCB_contents.PID,p->Next->PCB_contents.runningtime,p->Next->PCB_contents.priority);
            if(p->Next->PCB_contents.property == 1){
                printf("%s\t",p->Next->PCB_contents.FrontPID);
            }
            printf("\n");
            p = p->Next;
        }
    }
    printf("\n");
};
//打印后备队列中的进程
void PrintBackupQueue(Queue *BackupQueue){
    PCB *p = BackupQueue->Front;
    if(BackupQueue->Front == BackupQueue->Rear){
        printf("后备队列中没有进程！\n");
    }
    else{
        printf("\n");
        printf("进程名\t进程运行时间\t进程优先权\t前趋进程进程名\t后继进程进程名\n");
        while(p->Next!=NULL){
            printf("%s\t%d\t%d\t",p->Next->PCB_contents.PID,p->Next->PCB_contents.runningtime,p->Next->PCB_contents.priority);
            if(p->Next->PCB_contents.property== 1){
                printf("%s\t",p->Next->PCB_contents.FrontPID);
            }
            p = p->Next;
        }
    }
    printf("\n");
};

void RunProcess(Queue *ReadyQueue){
    //队头两个进程出队
    if(ReadyQueue->Front == ReadyQueue->Rear){
        return;
    }
    PCB *p1 = ReadyQueue->Front->Next;
    ReadyQueue->Front= p1;
    pcb t1;

    t1 = p1->PCB_contents;
    t1.status = 1;
    t1.runningtime -= 1;
    t1.priority -= 1;
    printf("正在运行的进程：\n");
    printf("进程名\t进程剩余运行时间\t进程优先权\n");
    printf("%s\t%d\t%d\n",t1.PID,t1.runningtime,t1.priority);
    printf("---------------------------------------------------------------------------------\n");
    //没有运行完的进程入队尾

    if(t1.runningtime > 0){
        t1.status = 0;
        PCB* p = (PCB*)malloc(sizeof(PCB));
        p->PCB_contents = t1;
        p->Next = NULL;
        ReadyQueue->Rear->Next = p;
        ReadyQueue->Rear = p;
    }
};


int main(void){
    Queue ReadyQueue;//定义就绪队列
    Queue BackupQueue;//定义后备队列
    InitQueue(&ReadyQueue);
    InitQueue(&BackupQueue);
    char c;//记录用户操作
    int i,j,k;
    for(i=0;i<TotalTime;i++){
        // //让用户选择可以将挂起状态的进程解挂
        // if(BackupQueue.Front != BackupQueue.Rear){
        //     printf("检测到有挂起状态的进程，是否解挂？输入“y”选择进程并解挂，输入“n”不解挂：");
        //     scanf("%c",&c);
        //     if(c == 'y'){

        //     }
        //     else{
        //         break;
        //     }
        // }
        //让用户增加进程
        fflush(stdin);//清除上一个回车符
        printf("是否要新增加进程？输入“y”增加，输入“n”不增加：");
        scanf("%c",&c);
            if(c == 'y'){
                InputPCBInformation(&ReadyQueue,&BackupQueue);
                //TotalTime;
            }
        //对队列中的进程进行排序
        SortProcess(&ReadyQueue);
        printf("----------------------------------------------------------------------------");
        //显示目前就绪队列与后备队列
        printf("\n就绪队列：\n");
        PrintReadyQueue(&ReadyQueue);
        printf("\n后备队列：\n");
        PrintBackupQueue(&BackupQueue);

        //模拟运行进程
        RunProcess(&ReadyQueue);
        printf("\nTotalTime:%d\ti=%d",TotalTime,i);

    }

}


/*测试数据
y
3
one
5
6
0
two
6
7
0
three
4
5
0
*/ 

/*测试数据2
y
7
one
5
6
1
0
1
sdfdsf
two
6
7
1
1
dsfdsfds
0
three
4
5
0
four
8
9
0
five
9
10
0
six
3
6
0
seven
8
10
0


*/