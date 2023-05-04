//
//  main.cpp
//  IOScheduler
//
//  Created by Arunima Mitra on 4/27/23.
//  Copyright © 2023 Arunima. All rights reserved.
//

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <istream>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <cstdlib>
#include <iomanip>
#include <math.h>
#include <algorithm>
#include <unistd.h>
#include <queue>


using namespace std;

ifstream file;

enum sums {TURNAROUND=0, WAITING=1, MAXWAITING=2};
unsigned long movementTracker = 0;

class IORequest{
    int id;
    int issued;
    int startTime;
    int endTime;
    int track;
    int movesLeft;
public:
    
    IORequest(int id1, int issued1, int tr){
        id=id1;
        issued=issued1;
        track=tr;
    }
    
    int getId(){ return id; }
    int getIssueTime(){ return issued; }
    int getTrackNumber(){ return track; }
    int getStartTime() { return startTime; }
    int getEndTime(){ return endTime; }
    
    void setStartTime(int s){ startTime = s; }
    void setEndTime(int e){ endTime=e; }

};

IORequest* CURRENT_RUNNING_IO=nullptr;
int currentTrack=0;
int direction=1;

vector<IORequest*> createdRequests;
list<IORequest> insertQ;


void initialize(string fileName){
    file.open(fileName);
    string lineNow;
     
     if (file.is_open()==false) {
         cout << "The gates of hell will open if I continue to parse with a non-existent file <%s>. Hence I decide to exit this program at this point avoiding insufferable pain\n" << fileName;
         exit(1);
     }
    
    while (getline(file, lineNow)) {
        if(lineNow[0]=='#') continue;
        if(lineNow.length()==0) continue;
        istringstream str(lineNow);
        int issued1,trackNum;
        str >> issued1 >> trackNum;
        
        IORequest *ioReq= new IORequest(createdRequests.size(), issued1, trackNum);
//        ioReq->id=createdRequests.size();
//        ioReq->issued=issued1;
//        ioReq->track=trackNum;
        createdRequests.push_back(ioReq);
        
    }
        
}

void printStats(){}

class IOScheduler{
    public:
    list<IORequest*> IOQueue;
    virtual IORequest* getStrategyVictim() = 0;
    virtual void addRequest(IORequest*) = 0;
    virtual bool empty(){
        return IOQueue.empty();
    }
};
IOScheduler *sch;


class FIFO:public IOScheduler{
public:
    void addRequest(IORequest* req){
        IOQueue.push_back(req);
    }
    
    IORequest* getStrategyVictim(){
        
        IORequest* Req=IOQueue.front();
        IOQueue.pop_front();
        return Req;
    }
};


class SSTF:public IOScheduler{
public:
    void addRequest(IORequest* req){
        IOQueue.push_back(req);
    }


    IORequest* getStrategyVictim(){
        auto start=IOQueue.begin();
        for(auto i = IOQueue.begin();i!=IOQueue.end(); i++)
        {
            unsigned long curr_dist =
                (*i)->getTrackNumber() > ::currentTrack ?
                (*i)->getTrackNumber() - ::currentTrack :
                ::currentTrack - (*i)->getTrackNumber();
            unsigned long shortest_dist =
                (*start)->getTrackNumber() > ::currentTrack ?
                (*start)->getTrackNumber() - ::currentTrack :
                ::currentTrack - (*start)->getTrackNumber();
            if(curr_dist<shortest_dist) start=i;
            
        }
        
        IORequest* ioReq = *start;
        IOQueue.erase(start);
        return ioReq;
    }
    
    bool empty(){
        return IOQueue.empty();
    }
};
std::map<int, std::string> mapper;
double summary[3];

void mappingOutput(IORequest* proc){
    int id=proc->getId();
    int issueTime=CURRENT_RUNNING_IO->getIssueTime();
    int startTime =CURRENT_RUNNING_IO->getStartTime();
    int endTime =CURRENT_RUNNING_IO->getEndTime();
    
    string str = to_string(CURRENT_RUNNING_IO->getId())+":\t"+to_string(CURRENT_RUNNING_IO->getIssueTime())+"\t"+to_string(CURRENT_RUNNING_IO->getStartTime())+"\t"+to_string(CURRENT_RUNNING_IO->getEndTime());
    
    mapper[id]=str;
    
    summary[TURNAROUND]+= endTime - issueTime;
    summary[WAITING]+= startTime - issueTime;
    if(summary[MAXWAITING] < startTime-issueTime)
        summary[MAXWAITING] = startTime-issueTime;
}

int currentTime = 0;

void printOutputs(){
    for(int i=0;i<createdRequests.size();i++){
        cout<<mapper[i]<<endl;
    }
    
    double avgTurnaround=summary[TURNAROUND]/createdRequests.size();
    double avgWaiting=summary[WAITING]/createdRequests.size();
    double avgBusy=(double)(movementTracker)/(double)(currentTime);
    cout<<"SUM: "<<currentTime<<" "<<movementTracker<<" "<<setprecision(4)<<avgBusy<<" "<<
    fixed << setprecision(2) << avgTurnaround << " "
    << fixed << setprecision(2) << avgWaiting <<" "
    << (long)(summary[MAXWAITING]) << endl;
}
void simulation(){
    
    auto next=createdRequests.begin();
    while(true){
        
        if(next!=createdRequests.end() && (*next)->getIssueTime()==::currentTime){
            sch->addRequest(*next);
            next++;
        }
        if(::CURRENT_RUNNING_IO!=nullptr){
            if(::CURRENT_RUNNING_IO->getTrackNumber()==::currentTrack){
                CURRENT_RUNNING_IO->setEndTime(currentTime);
            mappingOutput(CURRENT_RUNNING_IO);
            ::CURRENT_RUNNING_IO=nullptr;
            continue; }
        else{
            currentTrack+=direction;
            movementTracker++;
        }
        }
        else if(!::sch->empty()){
                ::CURRENT_RUNNING_IO = sch->getStrategyVictim();
            ::CURRENT_RUNNING_IO->setStartTime(currentTime);
                if (currentTrack!= CURRENT_RUNNING_IO->getTrackNumber()) {
                    if(currentTrack<CURRENT_RUNNING_IO->getTrackNumber()) direction=1;
                    else direction=-1;
                }
            continue;
            }
        
        if(!::CURRENT_RUNNING_IO && ::sch->empty() && next==createdRequests.end()){
                break;
            }
        currentTime++;
    }
    
}
int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    string inputFile="/Users/asmitamitra/Desktop/Spring2023/OS/Lab4/lab4_assign/input9";
    sch=new SSTF();

    initialize(inputFile);
    simulation();
    printOutputs();
    return 0;
}
