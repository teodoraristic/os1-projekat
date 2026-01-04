//=================THREAD_ID====================
/*
#include "modifikacija.hpp"
#include "../h/syscall_cpp.hpp"
#include "printing.hpp"

static volatile bool finishedA = false;
static volatile bool finishedB = false;
static volatile bool finishedC = false;


class ThreadA1 : public Thread {
public:
    void run() override {
        for (int i = 0; i < 10; i++) {
            int id = Thread::getThreadId();
            printString("A: id="); printInt(id); printString("\n");

            for (uint64 j = 0; j < 10000; j++) {
                for (uint64 k = 0; k < 30000; k++) { /* busy wait  }
                thread_dispatch();
            }
        }
        finishedA = true;
        printString("A finished!\n");
    }
};

class ThreadB1 : public Thread {
public:
    void run() override {
        for (int i = 0; i < 10; i++) {
            int id = Thread::getThreadId();
            printString("B: id="); printInt(id); printString("\n");

            for (uint64 j = 0; j < 10000; j++) {
                for (uint64 k = 0; k < 30000; k++) { /* busy wait  }
                thread_dispatch();
            }
        }

        printString("B finished!\n");
        finishedB = true;
        thread_dispatch();
    }
};

class ThreadC1 : public Thread {
public:
    void run() override {
        for (int i = 0; i < 10; i++) {
            int id = Thread::getThreadId();
            printString("C: id="); printInt(id); printString("\n");

            for (uint64 j = 0; j < 10000; j++) {
                for (uint64 k = 0; k < 30000; k++) { /* busy wait  }
                thread_dispatch();
            }
        }
        printString("C finished!\n");
        finishedC = true;
        thread_dispatch();
    }
};

void modif() {
    Thread* threads[3];

    threads[0] = new ThreadA1();
    threads[1] = new ThreadB1();
    threads[2] = new ThreadC1();

    printString("Starting threads...\n");

    for (int i = 0; i < 3; i++) {
        threads[i]->start();
    }

    while (!(finishedA && finishedB && finishedC)) {
        Thread::dispatch();
    }

    for (int i = 0; i < 3; i++) {
        delete threads[i];
    }
}
//=========PAIR_SYNC====================
class ThreadA2 : public Thread {
    int id;
public:
    ThreadA2(int id) : id (id) {}

    void run() override {
        for (int i = 0; i < 3; i++) {
            printString("Thread A-"); printInt(id);
            printString(": iteration "); printInt(i); printString(" before sync\n");

            //sync();  // Sinhronizacija sa partnerom

            printString("Thread A-"); printInt(id);
            printString(": iteration "); printInt(i); printString(" after sync\n");

            // Mali delay između iteracija
            for (volatile int j = 0; j < 10000; j++) {}
        }
        printString("Thread A-"); printInt(id); printString(" finished!\n");
    }

};

class ThreadB2 : public Thread {
    int id;
public:
    ThreadB2(int id) : id (id) {}
    void run() override {
        for (int i = 0; i < 3; i++) {
            printString("Thread B-"); printInt(id);
            printString(": iteration "); printInt(i); printString(" before sync\n");

            // Dodaj mali delay da bi se videla razlika
            for (volatile int j = 0; j < 5000; j++) {}

            //sync();  // Sinhronizacija sa partnerom

            printString("Thread B-"); printInt(id);
            printString(": iteration "); printInt(i); printString(" after sync\n");

            // Mali delay između iteracija
            for (volatile int j = 0; j < 10000; j++) {}
        }
        printString("Thread B-"); printInt(id); printString(" finished!\n");
    }
};

void modif2() {
    Thread* a1 = new ThreadA2(1);
    Thread* b1 = new ThreadB2(1);
    Thread* a2 = new ThreadA2(2);
    Thread* b2 = new ThreadB2(2);

    /*Thread::pair(a1, b1);
    Thread::pair(a2, b2);

    a1->start();
    b1->start();
    a2->start();
    b2->start();

    for (int i = 0; i < 30; i++) {
        Thread::dispatch();
    }

    printString("=== Pairing Test Finished ===\n");

    delete a1;
    delete b1;
    delete a2;
    delete b2;
}

//===============MAXIMUM_THR======================
static volatile bool finished[20] = {false};

void some_func(void *i) {
    int id = ::getThreadId();
    for (int j = 0; j < 5; j++) {
        printString("HELLO! Thread: \t");
        printInt(id);
        printString("\n");
    }
    for ( int j = 0; j < 1000 * id; j++) {}
    Thread::dispatch();
    printString("Thread end: "); printInt(id); printString("\n");
    finished[(long)i] = true;
}

void modif3() {
    //Thread::setMaximumThreads(3);
    Thread* ts[20];
    for (long i = 0; i < 20; i++) {
        ts[i] = new Thread(some_func, (void*)i);
    }
    while (!(finished[0] && finished[1] && finished[2] && finished[3] && finished[4] &&
            finished[5] && finished[6] && finished[7] && finished[8] && finished[9] &&
            finished[10] && finished[11] && finished[12] && finished[13] && finished[14] &&
            finished[15] && finished[16] && finished[17] && finished[18] && finished[19])) {
        Thread::dispatch();
            }

    for (auto thread: ts) { delete thread; }
}

//====================JOIN=====================
class SimpleWorker : public Thread {
private:
    int id;
public:
    SimpleWorker(int id) : id(id) {}

    void run() override {
        for (int i = 0; i < 3; i++) {
            printString("Worker "); printInt(id);
            printString(" working... "); printInt(i); printString("\n");

            for (volatile int j = 0; j < 10000; j++) {}
            Thread::dispatch();
        }
        printString("Worker "); printInt(id); printString(" finished!\n");
    }
};

void modif4() {
    printString("=== Simple Join Test ===\n");

    Thread* worker1 = new SimpleWorker(1);
    Thread* worker2 = new SimpleWorker(2);

    worker1->start();
    worker2->start();

    printString("Main waiting for worker 1...\n");
    worker1->join();
    printString("Main: worker 1 joined!\n");

    printString("Main waiting for worker 2...\n");
    worker2->join();
    printString("Main: worker 2 joined!\n");

    printString("All workers completed!\n");

    delete worker1;
    delete worker2;
}

//==================SEND_RECEIVE===================
class ThreadA : public Thread {
private:
    Thread* threadB;
    Thread* threadC;
public:
    ThreadA(Thread* b, Thread* c) : threadB(b), threadC(c) {}

    void run() override {
        printString("Thread A: started\n");

        // Šalje B i C
        //char msg1[] = "A to B: Hello!";
        //char msg2[] = "A to C: Hello!";

        //threadB->send(msg1);
        //threadC->send(msg2);

        // Prima od C
        //char* response = Thread::receive();
        //printString("Thread A: received: "); printString(response); printString("\n");

        printString("Thread A: finished\n");
    }
};

class ThreadB : public Thread {
private:
    Thread* threadC;
public:
    ThreadB(Thread* c) : threadC(c) {}

    void run() override {
        printString("Thread B: started\n");

        // Prima od A
        //char* msg = Thread::receive();
        //printString("Thread B: received: "); printString(msg); printString("\n");

        // Šalje C 2 poruke
       // char msg1[] = "B to C: First!";
        //char msg2[] = "B to C: Second!";
        //threadC->send(msg1);
       // threadC->send(msg2);

        printString("Thread B: finished\n");
    }
};

class ThreadC : public Thread {
public:
    Thread* threadA;
    ThreadC(Thread* a) : threadA(a) {}

    void run() override {
        printString("Thread C: started\n");

        // Prima 3 poruke
        for (int i = 0; i < 3; i++) {
            //char* msg = Thread::receive();
            printString("Thread C: received "); printInt(i + 1);
            //printString(": "); printString(msg); printString("\n");
        }

        // Šalje odgovor A
        //char response[] = "C to A: Response!";
        //threadA->send(response);

        printString("Thread C: finished\n");
    }
};

void modif5() {
    printString("=== Clean Three-Class Test ===\n");

    ThreadC* threadC = new ThreadC(nullptr);
    ThreadB* threadB = new ThreadB(threadC);
    ThreadA* threadA = new ThreadA(threadB, threadC);

    // Postavi referencu
    threadC->threadA = threadA;

    printString("Starting A, B, C...\n");

    threadA->start();
    threadB->start();
    threadC->start();

    threadA->join();
    threadB->join();
    threadC->join();

    printString("All threads completed!\n");

    delete threadA;
    delete threadB;
    delete threadC;
}

//================HISTOGRAM==========================
static int next = 17;
constexpr long mulpl = 53*61*7*113;
constexpr int modl = 17*13*11*31;
constexpr long incr = 5654465;

static void srand(int seed) {
    next = seed;
}

static int rand() {
    next = (mulpl * next + incr) % modl;
    return next;
}

static int histogram[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

struct Args {
    int idex;
    int len;
    int* own_column;
    int* own_histogram;
};

void workerBody(void* args) {
    auto my_args = (Args*) args;
    int my_idex = my_args->idex;
    int my_len = my_args->len;
    int* my_own_column = my_args->own_column;
    int* my_own_histogram = my_args->own_histogram;
    printString("\nWorker ");
    printInt(my_idex);
    printString(" start\n");
    for (int i = 0; i < my_len; i++) {
//        printString("Column: ");
//        printInt(my_own_column[0]);
//        printString("\n");
//        printString("Histo: ");
//        printInt(my_own_histogram[0]);
//        printString("\n");
        my_own_histogram[my_own_column[i] % 10]++;
        if (i % 10 == 0) thread_dispatch();
    }
//    printString("\nWorker ");
//    printInt(my_idex);
//    printString(" end\n");
}




void modif6() {
    char inputM[30];
    char inputN[30];
    int m, n;

    printString("Unesite broj redova?\n");
    getString(inputM, 30);
    m = stringToInt(inputM);
    printString("Unesite broj kolona?\n");
    getString(inputN, 30);
    n = stringToInt(inputN);

    srand(17);

    int** mat = (int**) mem_alloc(m * sizeof(int*));
    if(!mat) {
        printString("No memory\n");
        return;
    }
    for (int i = 0; i < m; i++) {
        mat[i] = (int*) mem_alloc(n * sizeof(int));
        if(!mat[i]) {
            printString("No memory\n");
            return;
        }
    }

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            mat[i][j] = rand();
        }
    }

    printString("\nPRAZAN HISTORGAM:\n");
    for (int i = 0; i < 10; i++) {
        printInt(histogram[i]);
        printString("\t");
    }
    printString("\n");

    printString("\nMATRICA:\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            printInt(mat[i][j]);
            printString("\t");
        }
        printString("\n");
    }
    printString("\n");

    thread_t* thrs = (thread_t*) mem_alloc(m * sizeof(thread_t));
    if(!thrs) {
        printString("No memory\n");
        return;
    }
    Args* args = (Args*) mem_alloc(m * sizeof(Args));
    if(!args) {
        printString("No memory\n");
        return;
    }
    int ** histograms = (int**) mem_alloc(m * sizeof(int*));
    if(!histograms) {
        printString("No memory\n");
        return;
    }
    for (int i = 0; i < m; i++) {
        histograms[i] = (int*) mem_alloc(10 * sizeof(int));
        if(!histograms[i]) {
            printString("No memory\n");
            return;
        }
        for (int j = 0; j < 10; j++) {
            histograms[i][j] = 0;
        }
    }

    for (int i = 0; i < m; i++) {
        args[i].len = n;
        args[i].idex = i;
        args[i].own_column = mat[i];
        args[i].own_histogram = histograms[i];
        thread_create(&thrs[i], workerBody, &args[i]);
    }

    while (true) {
        for (int i = 0; i < m; i++) {
            if (!thrs[i]->isFinished()) goto dsptch;
        }
        break;
        dsptch:
            thread_dispatch();
    }

    // Gather computed values:
    for (int i = 0; i < m; i++) {
        for(int j = 0; j < 10; j++) {
            histogram[j]+=histograms[i][j];
        }
    }
    printString("\nHISTORGAM:\n");
    for (int i = 0; i < 10; i++) {
        printInt(histogram[i]);
        printString("\t");
    }
    printString("\n");

    rand();
}

//============PING===============

class PingTestThread : public Thread {
private:
    int id;
public:
    PingTestThread(int id) : id(id) {}
    
    void run() override {
        printString("Thread "); printInt(id); printString(" started\n");
        
        // Alociraj nešto memorije
        if (id == 1) {
            printString("Thread 1 allocating memory...\n");
            int* arr1 = new int[10];
            char* str1 = new char[20];
            
            // Simuliraj rad
            for (int i = 0; i < 3; i++) {
                printString("Thread 1 working... "); printInt(i); printString("\n");
                for (volatile int j = 0; j < 10000; j++) {}
                Thread::dispatch();
            }
            
            delete[] arr1;
            delete[] str1;
        }
        else if (id == 2) {
            // Thread 2 će ping-ovati Thread 1
            printString("Thread 2 waiting then will ping Thread 1\n");
            
            for (volatile int j = 0; j < 20000; j++) {}  // Mali delay
            
            printString("Thread 2 pinging Thread 1!\n");
            // Ping Thread 1 - trebalo bi da vidi alokacije
        }
        
        printString("Thread "); printInt(id); printString(" finished\n");
    }
};

void ping_test() {
    printString("=== Ping Memory Tracking Test ===\n");
    
    PingTestThread* thread1 = new PingTestThread(1);
    PingTestThread* thread2 = new PingTestThread(2);
    
    thread1->start();
    thread2->start();
    
    // Main nit može da ping-uje bilo koju nit
    printString("Main thread will ping Thread 1\n");
    thread1->ping();
    
    // Daj vremena za izvršavanje
    for (int i = 0; i < 50; i++) {
        Thread::dispatch();
    }
    
    // Ping opet da vidimo promene
    printString("Main thread pinging Thread 1 again\n");
    thread1->ping();
    
    for (int i = 0; i < 20; i++) {
        Thread::dispatch();
    }
    
    delete thread1;
    delete thread2;
    printString("=== Ping Test Completed ===\n");
}

=======================JOIN_ALL=================================
class TCB {
public:
    using Body = void (*)(void*);
    static long gPid;
    static long tcbCount;

    ~TCB() { MemoryAllocator::mem_free(stack); }

    bool isFinished() const { return finished; }
    void setFinished(const bool value){finished = value;}

    void block() { blocked = true; }
    void unblock() { blocked = false; }
    bool isBlocked() const { return blocked; }

    bool isMain() const { return main; }
    long getPid() const { return myPid; }

    static TCB *createThread(TCB::Body body, void* arg);
    static void yield();
    static void dispatch();
    static int exit();

    int joinAll();
    void addChild(TCB* child);
    void notifyParent();

    static TCB* running;

private:
    TCB(Body body, void* args):
        body(body),
        stack(new uint64[DEFAULT_STACK_SIZE]),
        context({(uint64) &threadWrapper,
                 stack != nullptr ? (uint64) &stack[DEFAULT_STACK_SIZE] : 0}),
        finished(false),
        blocked(false),
        args(args),
        main(body==nullptr),
        myPid(gPid++)
    {tcbCount++;}

    struct Context {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    uint64 *stack;
    Context context;

    bool finished;
    bool blocked;
    void *args;
    bool main;
    long myPid;

    
    TCB* parent = nullptr;
    List<TCB> children;
    int finishedChildren = 0;
    bool waitingForJoinAll = false;

    friend class Riscv;

    static void threadWrapper();
    static void contextSwitch(Context* oldContext, Context* runningContext);
};

tcb.cpp - implementacija joinAll:
cpp

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"

TCB* TCB::running = nullptr;
long TCB::gPid = 0;
long TCB::tcbCount = 0;

TCB *TCB::createThread(TCB::Body body, void* arg) {
    TCB* thr = new TCB(body, arg);
    
    if(!thr->isMain()) {
        if (running) {
            running->addChild(thr);
        }
        Scheduler::put(thr);
    }
    return thr;
}

void TCB::addChild(TCB* child) {
    child->parent = this;
    children.addLast(child);
}

void TCB::notifyParent() {
    if (parent) {
        parent->finishedChildren++;
        
        if (parent->waitingForJoinAll && parent->finishedChildren >= parent->children.getSize()) {
            parent->unblock();
            parent->waitingForJoinAll = false;
            Scheduler::put(parent);
        }
    }
}

int TCB::joinAll() {
    TCB* current = running;

    if (current->children.isEmpty()) {
        return 0;  // Nema dece, odmah vraća uspeh
    }
    

    if (current->finishedChildren >= current->children.getSize()) {
        return 0; 
    }
    
    current->waitingForJoinAll = true;
    current->block();
    dispatch();
    
    return 0;
}

void TCB::yield() {
    uint64 op = 0x13;
    __asm__ volatile ("mv a0, %[x]" : : [x] "r" (op));
    __asm__ volatile ("ecall");
}

void TCB::dispatch() {
    TCB *old = running;
    if (!old->isFinished() && !old->isBlocked()) { 
        Scheduler::put(old); 
    }
    running = Scheduler::get();
    if (running){
        running->isMain() ? Riscv::ms_sstatus(Riscv::SSTATUS_SPP) : Riscv::mc_sstatus(Riscv::SSTATUS_SPP);
        TCB::contextSwitch(&old->context, &running->context);
    }
}

int TCB::exit() {
    if(running->isFinished()) return -1;
    running->setFinished(true);
    
    running->notifyParent();
    
    dispatch();
    return 0;
}

void TCB::threadWrapper(){
    Riscv::popSppSpie();
    running->isMain() ? running->cppThread->run() : running->body(running->args);
    running->setFinished(true);
    
    running->notifyParent();
    
    TCB::yield();
}

*/

