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
#include <queue>


using namespace std;

ifstream file;
class IORequest{
public:
    int id;
    int issued;
    int startTime;
    int endTime;
    int track;
    int movesLeft;
    
    void setValues(int id1, int issued1, int tr){
        id=id1;
        issued=issued1;
        track=tr;
    }
    
    int getId(){ return id; }
    int getIssueTime(){ return issued; }
    int getTrackNumber(){ return track; }

};

IORequest* CURRENT_RUNNING_IO;
int currentTrack=-1;
int direction=+1;

vector<IORequest> createdRequests;
deque<IORequest> insertQ;
deque<IORequest> IOQueue;

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
        
        IORequest ioReq;
        ioReq.id=createdRequests.size();
        ioReq.issued=issued1;
        ioReq.track=trackNum;
        createdRequests.push_back(ioReq);
        insertQ.push_back(ioReq);
    }
        
}

void printStats(){}

class IOScheduler{
    public:
    virtual void getStrategyVictim() = 0;
};
IOScheduler *sch;


class FIFO:public IOScheduler{
public:
    void getStrategyVictim(){
        
        CURRENT_RUNNING_IO=&IOQueue.front();
        if(currentTrack!=-1){
            if(CURRENT_RUNNING_IO->track > currentTrack) direction=1;
            else direction=-1;
        }
        
        IOQueue.pop_front();
        
    }
};

void simulation(){
    int time = 0;
    /*while (true)
    if a new I/O arrived at the system at this current time
     → add request to IO-queue
    if an IO is active and completed at this time
     → Compute relevant info and store in IO request for final summary
    if no IO request active now
     if requests are pending
     → Fetch the next request from IO-queue and start the new IO.
     else if all IO from input file processed
     → exit simulation
    if an IO is active
     → Move the head by one unit in the direction its going (to simulate seek)
    Increment time by 1*/
    
    
    
    while(true){
        
        if(insertQ.front().getIssueTime()==time){
            IOQueue.push_back(insertQ.front());
            insertQ.pop_front();
        }
        
        if(CURRENT_RUNNING_IO!=NULL && CURRENT_RUNNING_IO->endTime==time){
            cout<<CURRENT_RUNNING_IO->getId()<<":\t"<<CURRENT_RUNNING_IO->getIssueTime()<<"\t"<<CURRENT_RUNNING_IO->startTime<<"\t"<<CURRENT_RUNNING_IO->endTime<<endl;
            currentTrack=CURRENT_RUNNING_IO->track;
            CURRENT_RUNNING_IO=NULL;
        }
        if(CURRENT_RUNNING_IO==NULL){
            if(!IOQueue.empty()){
                sch->getStrategyVictim();
                CURRENT_RUNNING_IO->startTime=time;
                if(currentTrack==-1){CURRENT_RUNNING_IO->endTime=time+CURRENT_RUNNING_IO->track;} // first req
                else if(direction==1){
                    CURRENT_RUNNING_IO->endTime=CURRENT_RUNNING_IO->track-currentTrack+time;
                }
                else {
                    CURRENT_RUNNING_IO->endTime=currentTrack-CURRENT_RUNNING_IO->track+time;
                }
                
//                cout<<endl<<CURRENT_RUNNING_IO->getId()<<" "<<CURRENT_RUNNING_IO->startTime<<" "<<CURRENT_RUNNING_IO->endTime<<endl;
            }
            else if(insertQ.empty()){
                printStats();
                break;
            }
        }
        time++;
        
    }
}
int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    string inputFile="/Users/asmitamitra/Desktop/Spring2023/OS/Lab4/lab4_assign/input9";
    initialize(inputFile);
    sch=new FIFO();
    simulation();
    return 0;
}
