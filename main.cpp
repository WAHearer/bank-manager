#include<iostream>
#include<algorithm>
#define MAX 100000
int chunksize;
struct Complete{
    int solved,sum;
};
class HeapNode{
    friend class Heap;
private:
    explicit HeapNode(int val):val(val),father(nullptr),left(nullptr),right(nullptr){}
    HeapNode *father,*left,*right;
    int val;
};
class Heap{
private:
    HeapNode *root;
    HeapNode *getAvailableNode(HeapNode *now){
        if(!now->left||!now->right)
            return now;
        return rand()%2?getAvailableNode(now->left):getAvailableNode(now->right);
    }
    HeapNode *getLeaf(HeapNode *now){
        if(!now->left&&!now->right)
            return now;
        else if(now->left&&!now->right)
            return getLeaf(now->left);
        else if(!now->left&&now->right)
            return getLeaf(now->right);
        else
            return rand()%2?getLeaf(now->left):getLeaf(now->right);
    }
    void pushUp(HeapNode *now){
        if(now->father&&now->val<now->father->val){
            std::swap(now->val,now->father->val);
            pushUp(now->father);
        }
    }
    void pushDown(HeapNode *now){
        if(now->left&&now->val>now->left->val){
            std::swap(now->val,now->left->val);
            pushDown(now->left);
        }
        else if(now->right&&now->val>now->right->val){
            std::swap(now->val,now->right->val);
            pushDown(now->right);
        }
    }
public:
    Heap():root(nullptr){}
    void push(int val){
        HeapNode *node=new HeapNode(val);
        if(root==nullptr)
            root=node;
        else{
            HeapNode *fatherNode=getAvailableNode(root);
            if(!fatherNode->left)
                fatherNode->left=node;
            else
                fatherNode->right=node;
            node->father=fatherNode;
            pushUp(node);
        }
    }
    int top(){
        return root->val;
    }
    int pop(){
        int val=root->val;
        if(!root->left&&!root->right){
            delete root;
            root=nullptr;
            return val;
        }
        HeapNode *leaf=getLeaf(root);
        std::swap(root->val,leaf->val);
        if(leaf==leaf->father->left)
            leaf->father->left=nullptr;
        else
            leaf->father->right=nullptr;
        delete leaf;
        pushDown(root);
        return val;
    }
};
class CustNode{
public:
    CustNode(int arrtime,int durtime,int amount):arrtime(arrtime),durtime(durtime),leavetime(-1),amount(amount){}
    int arrtime,durtime,leavetime,amount;
};
class QueueNode{
    friend class CircularQueue;
public:
    CustNode node;
private:
    QueueNode *next;
    QueueNode():node(CustNode(0,0,0)),next(nullptr){}
};
class CircularQueue{
private:
    QueueNode *head,*tail;
    int length,size,chunksize;//length为总空间，size为已分配空间
    void expand(){
        QueueNode *p=this->tail;
        for(int i=1;i<=this->chunksize;i++){
            p->next=new QueueNode;
            p=p->next;
        }
        p->next=this->head;
        this->length+=this->chunksize;
    }
public:
    explicit CircularQueue(int chunksize):length(chunksize),size(0),chunksize(chunksize){
        this->head=new QueueNode;
        QueueNode *p=this->head;
        for(int i=1;i<chunksize;i++){
            p->next=new QueueNode;
            p=p->next;
        }
        p->next=this->head;
        this->tail=this->head;
    }
    ~CircularQueue(){
        QueueNode *p=this->head,*nxt;
        for(int i=1;i<=this->length;i++){
            if(i!=this->length)
                nxt=p->next;
            free(p);
            if(i!=this->length)
                p=nxt;
        }
    }
    void push(CustNode node){
        if(this->tail->next==this->head)
            this->expand();
        this->tail->node=node;
        this->tail=this->tail->next;
        this->size++;
    }
    QueueNode pop(){
        QueueNode head=*this->head;
        this->head=this->head->next;
        this->size--;
        return head;
    }
    int getLength(){
        return this->length;
    }
    int getSize(){
        return this->size;
    }
    bool empty(){
        return !this->size;
    }
};
class CustNodeInList:public CustNode{
    friend class Eventlist;
public:
    CustNodeInList():CustNode(0,0,0),next(-1){}
    CustNodeInList(int arrtime,int durtime,int amount):CustNode(arrtime,durtime,amount),next(-1){}
private:
    int next;
};
class Eventlist{
private:
    Heap usableMemory;
    CustNodeInList eventlist[MAX];
    int size;
public:
    Eventlist():size(0){
        for(int i=1;i<MAX;i++)
            usableMemory.push(i);
    }
    void insert(int arrtime,int durtime,int amount){
        int p=0;
        while(eventlist[p].next!=-1&&eventlist[eventlist[p].next].arrtime<=arrtime){
            p=eventlist[p].next;
        }
        int location=usableMemory.pop();
        CustNodeInList temp(arrtime,durtime,amount);
        eventlist[location]=temp;
        eventlist[location].next=eventlist[p].next;
        eventlist[p].next=location;
        size++;
    }
    CustNodeInList query(int x){
        int p=0;
        for(int i=1;i<=x;i++){
            p=eventlist[p].next;
        }
        return eventlist[p];
    }
    void setLeaveTime(int arrtime,int amount,int leavetime){
        int p=0;
        while(p!=-1){
            if(eventlist[p].arrtime==arrtime&&eventlist[p].amount==amount){
                eventlist[p].leavetime=leavetime;
                return;
            }
            p=eventlist[p].next;
        }
    }
    Complete del(){
        int p=0,solved=0,sum=0;
        while(eventlist[p].next!=-1){
            int temp=eventlist[p].next;
            if(eventlist[temp].leavetime!=-1){
                solved++;
                sum+=(eventlist[temp].leavetime-eventlist[temp].arrtime);
                eventlist[p].next=eventlist[temp].next;
                usableMemory.push(temp);
                size--;
            }
            else
                p=eventlist[p].next;
        }
        return {solved,sum};
    }
    int getSize(){
        return size;
    }
    void print(){
        int p=eventlist[0].next;
        while(p!=-1){
            std::cout<<eventlist[p].arrtime<<" "<<eventlist[p].durtime<<" "<<eventlist[p].amount<<std::endl;
            p=eventlist[p].next;
        }
    }
};
class Bank{
private:
    int total,closetime,maxDurtime,minDurtime,maxAmount,minAmount,maxInterval,minInterval;
    CircularQueue q1,q2;
    Eventlist list;
    void insert(int arrtime,int durtime,int amount){
        list.insert(arrtime,durtime,amount);
    }
    void generate(){
        int arrtime=0,durtime=rand()%(maxDurtime-minDurtime+1)+minDurtime,amount=rand()%(maxAmount-minAmount+1)+minAmount,interval=rand()%(maxInterval-minInterval+1)+minInterval;
        while(arrtime<=closetime){
            if(arrtime+durtime<=closetime)
                insert(arrtime,durtime,amount);
            arrtime+=interval;
            durtime=rand()%(maxDurtime-minDurtime+1)+minDurtime,amount=rand()%(maxAmount-minAmount+1)+minAmount,interval=rand()%(maxInterval-minInterval+1)+minInterval;
        }
    }
    void run(){
        int size=list.getSize();
        for(int i=1;i<=size;i++){
            CustNodeInList node=list.query(i);
            q1.push(node);
        }
        int time=0;
        while(!q1.empty()){
            int oldtotal=total;
            CustNode now=q1.pop().node;
            if(std::max(time,now.arrtime)+now.durtime>closetime)
                break;
            if(total+now.amount<0){
                q2.push(now);
                continue;
            }
            else{
                total+=now.amount;
                time=std::max(time,now.arrtime)+now.durtime;
                list.setLeaveTime(now.arrtime,now.amount,time);
            }
            if(now.amount>0){
                int cnt=1,size=q2.getSize();
                while(cnt<=size&&total>oldtotal){
                    now=q2.pop().node;
                    if(total+now.amount>=0){
                        total+=now.amount;
                        time=std::max(time,now.arrtime)+now.durtime;
                        list.setLeaveTime(now.arrtime,now.amount,time);
                    }
                    else
                        q2.push(now);
                    cnt++;
                }
            }
        }
    }
public:
    Bank():q1(chunksize),q2(chunksize){
        std::cout<<"请输入银行初始资金total"<<std::endl;
        std::cin>>total;
        std::cout<<"请输入银行关闭时间closetime"<<std::endl;
        std::cin>>closetime;
        std::cout<<"请输入办理业务时间durtime的上界与下界"<<std::endl;
        std::cin>>maxDurtime>>minDurtime;
        std::cout<<"请输入业务金额amount的上界与下界"<<std::endl;
        std::cin>>maxAmount>>minAmount;
        std::cout<<"请输入业务时间间隔interval的上界与下界"<<std::endl;
        std::cin>>maxInterval>>minInterval;
    }
    void print(){
        list.print();
    }
    float imitate(int days){
        int solved=0,sum=0;
        for(int i=1;i<=days;i++){
            generate();
            run();
            Complete result=list.del();
            solved+=result.solved;
            sum+=result.sum;
        }
        return 1.0*sum/solved;
    }
};
int main(){
    srand(time(nullptr));
    std::cout<<"请输入循环队列初始大小chunksize"<<std::endl;
    std::cin>>chunksize;
    int days;
    std::cout<<"请输入模拟运行的天数days"<<std::endl;
    std::cin>>days;
    Bank bank;
    std::cout<<"模拟运行的平均逗留时间为："<<bank.imitate(days);
    return 0;
}
