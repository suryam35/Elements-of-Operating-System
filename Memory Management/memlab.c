// Group No- 53
// Suryam Arnav Kalra - 19CS30050
// Kunal Singh - 19CS30025

#include "memlab.h"

unsigned char *memPtr;
int *pagetable;
int localCounter;
int totalMemory;
// map<string, pair<int,int>> varToPage;  // name -> (type, localcounter)
// map<int,string> pageToVar;
// map<string,int> arrSz;
// set<int> freeFrames;
// map<string, bool> mark;

typedef struct varToPage {
    char name[MAX_CHAR];
    int type;
    int localCounter;
} varToPage;

typedef struct pageToVar {
    char name[MAX_CHAR];
} pageToVar;

typedef struct arrSz {
    char name[MAX_CHAR];
    int sz;
} arrSz;

typedef struct mark {
    char name[MAX_CHAR];
    int is_mark;
} mark;

typedef struct freeFrame {
    int a[REQD];
    int cnt;
} freeFrame;

// int *freeFrames;
varToPage *vP;
pageToVar *pV;
arrSz *aS;
mark *mk;
freeFrame *freeFrames;

pthread_t gcid;
pthread_mutex_t mutex_lock;

int min(int a, int b) {
    return (a < b) ? a : b;
}

void push(int val) {
    freeFrames->a[freeFrames->cnt] = val;
    freeFrames->cnt = freeFrames->cnt+1;
}

void init_stack(int sz) {
    freeFrames = (freeFrame *)malloc(sizeof(freeFrame));
    freeFrames->cnt = 0;
    for(int i = 0; i < min(REQD,sz/4); i++) {
        push(i);
    }
}

int top() {
    return freeFrames->a[freeFrames->cnt-1];
}


void pop() {
    freeFrames->cnt = freeFrames->cnt -1;
}


int search_varToPage(char *name) {
    int found = -1;
    for(int i = 0; i < MAX_SIZE; i++) {
        if(strcmp(vP[i].name, name) == 0) {
            found = i;
            break;
        }
    }
    return found;
}

void insert_varToPage(char *name, int type, int lc) {
    for(int i = 0; i < MAX_SIZE; i++) {
        if(vP[i].type == -1) {
            strcpy(vP[i].name, name);
            vP[i].type = type;
            vP[i].localCounter = lc;
            break;
        }
    }
}

int getFreeFrame() {
    // for(int i = 0; i < totalMemory/4; i++) {
    //     if(freeFrames[i] == 1) {
    //         return i;
    //     }
    // }
    return top();
}

void removeFreeFrame(int id) {
    // freeFrames[id] = 0;
    pop();
}

void insert_pageToVar(char *name, int lc) {
    for(int i = 0; i < MAX_CHAR; i++) {
        (pV[lc].name)[i] = '\0';
    }
    strcpy(pV[lc].name, name);
}

void remove_varToPage(char *name) {
    for(int i = 0; i < MAX_SIZE; i++) {
        if(strcmp(vP[i].name, name) == 0) {
            vP[i].type = -1;
            vP[i].localCounter = -1;
            for(int j = 0; j < MAX_CHAR; j++) {
                (vP[i].name)[j] = '\0';
            }
            break;
        }
    }
}

void remove_pageToVar(int lc) {
    for(int i = 0; i < MAX_CHAR; i++) {
        (pV[lc].name)[i] = '\0';
    }
}

int search_arrSz(char *name) {
    int found = -1;
    for(int i = 0; i < MAX_SIZE; i++) {
        if(strcmp(aS[i].name, name) == 0) {
            found = i;
            break;
        }
    }
    return found;
}

void insert_freeFrame(int lc) {
    // freeFrames[lc] = 1;
    push(lc);
}

void remove_arrSz(int id) {
    for(int i = 0; i < MAX_CHAR; i++) {
        (aS[id].name)[i] = '\0';
    }
    aS[id].sz = -1;
}

void remove_mark(char *name) {
    for(int i = 0; i < MAX_SIZE; i++) {
        if(strcmp(mk[i].name, name) == 0) {
            for(int j = 0; j < MAX_CHAR; j++) {
                (mk[i].name)[j] = '\0';
            }
            mk[i].is_mark = 0;
            break;
        }
    }
}

void sweepPhase(char *name) {
    int found = search_varToPage(name);
    // pair<int,int> p = varToPage[name];
    int type = vP[found].type;
    int logicalAddress = vP[found].localCounter;
    // varToPage.erase(name);
    // pageToVar.erase(p.second);

    remove_varToPage(name);
    remove_pageToVar(logicalAddress);

    // int is_array = (arrSz.count(name) > 0 ? 1: 0);
    int is_array = search_arrSz(name);
    if(is_array != -1) {
        int sz = aS[is_array].sz;
        remove_arrSz(is_array);
        // arrSz.erase(name);
        // int logicalAddress = p.second;
        for(int i = 0; i < sz; i++) {
            // freeFrames.insert(pagetable[logicalAddress+i]);
            insert_freeFrame(pagetable[logicalAddress+i]);
            pagetable[logicalAddress+i] = -1;
        }
    }
    else {
        // int logicalAddress = p.second;
        // freeFrames.insert(pagetable[logicalAddress]);
        insert_freeFrame(pagetable[logicalAddress]);
        pagetable[logicalAddress] = -1;
    }
    // mark.erase(name);
    remove_mark(name);
}

void compactPhase() {
    printf("Compact phase started\n");
    int firstFound = -1;
    int holes = 0;
    for(int i = 0; i < localCounter/4; i++) {
        if(pagetable[i] == -1) {
            if(firstFound == -1)
                firstFound = i;
            holes++;
        }
    }
    if(firstFound == -1) {
        return;
    }
    int start = firstFound;
    for(int i = firstFound+1; i < localCounter/4; i++) {
        if(pagetable[i] == -1) {
            continue;
        }
        char *varName = pV[i].name;
        // pair<int,int> p = varToPage[varName];
        int found = search_varToPage(varName);
        // p.second = i;
        vP[found].localCounter = start;

        // varToPage[varName] = p;
        pagetable[start] = pagetable[i];
        // pageToVar[start] = varName;
        insert_pageToVar(varName, start);
        pagetable[i] = -1;
        start++;
        // bool is_array = (arrSz[varName] > 0 ? 1: 0);
        int is_array = search_arrSz(varName);

        if(is_array != -1) {
            i++;
            // int sz = arrSz[varName];
            int sz = aS[is_array].sz;
            for(int j = 0; j < sz-1; j++) {
                pagetable[start] = pagetable[i];
                pagetable[i] = -1;
                start++;
                i++;
            }
            i--;
        }
    }

    localCounter -= 4*holes;
    printf("Compact phase ended after merging %d holes\n", holes);

}

void clear_mark() {
    for(int i = 0; i < MAX_SIZE; i++) {
        for(int j = 0; j < MAX_CHAR; j++) {
            (mk[i].name)[j] = '\0';
        }
        mk[i].is_mark = 0;
    }
}

void gc_run() {
    pthread_mutex_lock(&mutex_lock);
    printf("Garbage collector started\n");
    // for(pair<string,bool> p: mark) {
    //     sweepPhase(p.first);
    // }
    printf("Sweep phase started\n");
    for(int i = 0; i < MAX_SIZE; i++) {
        // printf("From Mark: %s\n", mk[i].name);
        if(mk[i].is_mark == 1) {
            sweepPhase(mk[i].name);
        }
    }
    printf("Sweep phase ended\n");
    clear_mark();
    // mark.clear();
    compactPhase();
    printf("Garbage collector ended\n");
    pthread_mutex_unlock(&mutex_lock);
}

void *gc_help(void *param) {
    // compaction
    while(1) {
        sleep(0.001);
        gc_run();
    }
}

void gc_initialize() {
    pthread_mutex_init(&mutex_lock,NULL);
    pthread_attr_t attr; //Set of thread attributes
    pthread_attr_init(&attr);
    int GC = pthread_create(&gcid, &attr, gc_help, NULL);

    if(GC < 0) {
        exit(1);
    }
}

void initialize_pageTable(int sz) {
    pagetable = (int *)malloc(sizeof(int)*sz/4); 
    for(int i = 0; i < min(REQD, sz/4); i++) {
        pagetable[i] = -1;
    }
}

// void initialize_freeFrames(int sz) {
//     freeFrames = (int *)malloc(sizeof(int)*min(4000000, sz/4));
//     for(int i = 0; i < min(4000000, sz/4); i++) {
//         freeFrames[i] = 1;
//     }
// }

void initialize_varToPage() {
    vP = (varToPage *)malloc(sizeof(varToPage)*MAX_SIZE);
    for(int i = 0; i < MAX_SIZE; i++) {
        vP[i].type = -1;
        vP[i].localCounter = -1;
        for(int j = 0; j < MAX_CHAR; j++) {
            (vP[i].name)[j] = '\0';
        }
    }
}

void initialize_pageToVar(int sz) {
    pV = (pageToVar *)malloc(sizeof(pageToVar)*sz/4);
    for(int i = 0; i < min(REQD, sz/4); i++) {
        for(int j = 0; j < MAX_CHAR; j++) {
            (pV[i].name)[j] = '\0';
        }
    }
}

void initialize_arrSz() {
    aS = (arrSz *)malloc(sizeof(arrSz)*MAX_SIZE);
    for(int i = 0; i < MAX_SIZE; i++) {
        aS[i].sz = -1;
        for(int j = 0; j < MAX_CHAR; j++) {
            (aS[i].name)[j] = '\0';
        }
    }
}

void initialize_mark() {
    mk = (mark *)malloc(sizeof(mark)*MAX_SIZE);
    for(int i = 0; i < MAX_SIZE; i++) {
        mk[i].is_mark = 0;
        for(int j = 0; j < MAX_CHAR; j++) {
            (mk[i].name)[j] = '\0';
        }
    }
}

void createMem(int sz) {
	totalMemory = sz;
	memPtr = (unsigned char *)malloc(sz);

    printf("%d bytes memory created\n", sz);
    // initialize_freeFrames(sz);
    printf("Creation of book-keeping data structures started\n");
    initialize_pageTable(sz);
    initialize_varToPage();
    initialize_pageToVar(sz);
    initialize_arrSz();
    initialize_mark();
    init_stack(sz);
    gc_initialize();
    printf("Creation of book-keeping data structures completed\n");
}

void createVar(char *name, int type) {

    int found = search_varToPage(name);
	
    // if(varToPage.count(name) != 0) {
    //     cout << "Variable with same name exists\n";
    //     return;
    // }

    if(found != -1) {
        printf("Variable with same name exists\n");
        return;
    }

    if(localCounter >= totalMemory) {
        printf("Memory exceeded\n");
        return;
    }

    if(type != INT) {
        printf("Word alignment for variable: %s is done\n", name);
    }
    printf("Variable with name: %s created\n", name);
    insert_varToPage(name, type, localCounter/4);

    // varToPage[name] = {type, localCounter/4};

    int freeF = getFreeFrame();

    pagetable[localCounter/4] = freeF;

    insert_pageToVar(name, localCounter/4);

    // pageToVar[localCounter/4] = name;


    removeFreeFrame(freeF);

    // freeFrames.erase(freeFrames.begin());
    localCounter += 4;

}

void assignVar(char *name, int type, int val) {
    int found = search_varToPage(name);

    // if(varToPage.count(name) == 0) {
    //     cout << "No such variable exists\n";
    //     return;
    // }

    if(found == -1) {
        printf("No such variable exists\n");
        return;
    }

    int my_type = vP[found].type;
    // pair<int,int> p = varToPage[name];
    // if(p.first != type) {
    //     cout << "Variable types do not match\n";
    //     return;
    // }
    if(my_type != type) {
        printf("Variable types do not match\n");
        return;
    }
    if(type == MEDINT && val >= (1<<23)) {
        printf("MEDINT value exceeded\n");
        return;
    }
    // printf("Variable with name: %s assigned value\n", name);
    // int logicalAddress = p.second;
    // int frameAddress = pagetable[logicalAddress];

    int logicalAddress = vP[found].localCounter;
    int frameAddress = pagetable[logicalAddress];

    if(type == INT) {
        for(int i = 0; i < 4; i++) {
            *(memPtr+frameAddress*4+3-i) = val & 255;
            val >>= 8; 
        }
    }
    else if(type == CHAR) {
        *(memPtr+frameAddress*4) = val;
    }
    else if(type == MEDINT) {
        
        for(int i = 0; i < 3; i++) {
            *(memPtr+frameAddress*4+2-i) = val & 255;
            val >>= 8; 
        }
    }
    else {
        if(val != 0) {
            val = 1;
        }
        *(memPtr+frameAddress*4) = val;
    }

}

int getVar(char *name) {
    int found = search_varToPage(name);

    // if(varToPage.count(name) == 0) {
    //     cout << "No such variable exists\n";
    //     return -1;
    // }
    if(found == -1) {
        printf("No such variable exists\n");
        return -1;
    }
    // pair<int,int> p = varToPage[name];
    // int type = p.first;
    // int logicalAddress = p.second;

    int type = vP[found].type;
    int logicalAddress = vP[found].localCounter;
    int frameAddress = pagetable[logicalAddress];

    int nval = 0;
    if(type == INT) {
        for(int i = 0; i < 4; i++) {
            nval <<= 8;
            nval += *(memPtr+frameAddress*4+i);
        } 
    }
    else if(type == CHAR) {
        nval = *(memPtr+frameAddress*4);
    }
    else if(type == MEDINT) {
        for(int i = 0; i < 3; i++) {
            nval <<= 8;
            nval += *(memPtr+frameAddress*4+i); 
        }
    }
    else {
        nval = *(memPtr+frameAddress*4);
    }

    return nval;
}

void insert_arrSz(char *name, int sz) {
    for(int i = 0; i < MAX_SIZE; i++) {
        if(aS[i].sz == -1) {
            aS[i].sz = sz;
            strcpy(aS[i].name, name);
            break;
        }
    }
}

void createArr(char *name, int type, int sz) {
    int found = search_varToPage(name);

    if(found != -1) {
        printf("Variable name exists\n");
        return;
    }

    // if(varToPage.count(name) != 0) {
    //     cout << "Variable name exists\n";
    //     return;
    // }
    if(localCounter + (sz-1)*4 >= totalMemory) {
        printf("Memory exceeded\n");
        return;
    }

    if(type != INT) {
        printf("Word alignment for array: %s is done\n", name);
    }
    printf("Array with name: %s created\n", name);
    // arrSz[name] = sz;
    insert_arrSz(name, sz);
    // varToPage[name] = {type, localCounter/4};
    insert_varToPage(name, type, localCounter/4);
    // pageToVar[localCounter/4] = name;
    insert_pageToVar(name, localCounter/4);
    for(int i = 0; i < sz; i++) {
        int freeF = getFreeFrame();
        pagetable[localCounter/4] = freeF;
        localCounter += 4;
        // freeFrames.erase(freeFrames.begin());
        removeFreeFrame(freeF);
    }

}

void assignArr(char *name, int type, int val, int offset) {
    int found = search_varToPage(name);

    if(found == -1) {
        printf("No such variable exists\n");
        return;
    }

    // if(varToPage.count(name) == 0) {
    //     cout << "No such variable name exists\n";
    //     return;
    // }

    // pair<int,int> p = varToPage[name];

    int my_type = vP[found].type;

    // if(p.first != type) {
    //     cout << "Variable types do not match\n";
    //     return;
    // }

    if(my_type != type) {
        printf("Variable types do not match\n");
        return;
    }

    if(type == MEDINT && val >= (1<<23)) {
        printf("MEDINT value exceeded\n");
        return;
    }
    // int logicalAddress = p.second + offset;
    int logicalAddress = vP[found].localCounter + offset;
    int frameAddress = pagetable[logicalAddress];

    if(type == INT) {
        for(int i = 0; i < 4; i++) {
            *(memPtr+frameAddress*4+3-i) = val & 255;
            val >>= 8; 
        }
    }
    else if(type == CHAR) {
        *(memPtr+frameAddress*4) = val;
    }
    else if(type == MEDINT) {
        
        for(int i = 0; i < 3; i++) {
            *(memPtr+frameAddress*4+2-i) = val & 255;
            val >>= 8; 
        }
    }
    else {
        if(val != 0) {
            val = 1;
        }
        *(memPtr+frameAddress*4) = val;
    }

}

int getArrElem(char *name, int offset) {
    int found = search_varToPage(name);
    if(found == -1) {
        printf("No such variable name exists\n");
        return -1;
    }
    // if(varToPage.count(name) == 0) {
    //     cout << "No such variable exists\n";
    //     return -1;
    // }
    // pair<int,int> p = varToPage[name];
    // int type = p.first;
    // int logicalAddress = p.second + offset;
    int type = vP[found].type;
    int logicalAddress = vP[found].localCounter + offset;
    int frameAddress = pagetable[logicalAddress];

    int nval = 0;
    if(type == INT) {
        for(int i = 0; i < 4; i++) {
            nval <<= 8;
            nval += *(memPtr+frameAddress*4+i);
        }
    }
    else if(type == CHAR) {
        nval = *(memPtr+frameAddress*4);
    }
    else if(type == MEDINT) {
        for(int i = 0; i < 3; i++) {
            nval <<= 8;
            nval += *(memPtr+frameAddress*4+i); 
        }
    }
    else {
        nval = *(memPtr+frameAddress*4);
    }

    return nval;
}

void insert_mark(char *name) {
    for(int i = 0; i < MAX_SIZE; i++) {
        if(mk[i].is_mark == 0) {
            strcpy(mk[i].name, name);
            mk[i].is_mark = 1;
            break;
        }
    }
}

void freeElem(char *name) {
    int found = search_varToPage(name);
    if(found == -1) {
        printf("No such variable exists\n");
        return;
    }
    // if(varToPage.count(name) == 0) {
    //     cout << "No such variable exists\n";
    //     return;
    // }
    pthread_mutex_lock(&mutex_lock);
    insert_mark(name);
    sweepPhase(name);
    pthread_mutex_unlock(&mutex_lock);
    // mark[name] = 1;
    
    printf("FreeElem called for variable name: %s\n", name);
}