#include<stdio.h>
#include<stdlib.h>
#include<stddef.h>
#include<stdarg.h>
#include<string>
#include<strings.h>
#include<assert.h>
#include "hiredis.h"
#include<cstdlib>
#include<iostream>
#include<ctime>
clock_t start,end;
double pipeline(){
    //start = clock();
    redisContext* c = redisConnect("127.0.0.1",6379);
    if(c->err){
        redisFree(c);
        return 0;
    }
   start = clock();
   std::string zset1 = "zset1 ";
   redisReply * reply = NULL;
   for(int i = 0; i < 20000; i++){
        std::string command = "zadd ";
        int s = rand() % 100;
        std::string value = std::to_string(i+1);
        std::string score = std::to_string(s);
        command.append(zset1);
        command.append(score);
        command.append(" ");
        command.append(value);
        if(REDIS_OK != redisAppendCommand(c,command.c_str())){
            std::cout<<"[END] END WITH i="<<i<<"\n";
            redisFree(c);
            return 0;
        }
    }
    for(int i = 0; i < 20000; i++){
        if(REDIS_OK != redisGetReply(c,(void**)&reply)){
           std::cout<<"[ERROR] REIDS_ERR WITH i = " << i <<" : FAILED!\n";
           freeReplyObject(reply);
           redisFree(c);
           return 0;
        }
        // else std::cout<<"[INFO] i = " << i << " : SUCCESSFUL!\n";
    }
    end = clock();
    double atime = (double)(end - start)/CLOCKS_PER_SEC;
    // std::cout << "(PipeLine Mode) Time Cost : " << atime * 1000<<"ms\n";
    redisFree(c);
    return atime * 1000;
}
double singleCommand(){
    //start = clock();
    redisContext* c = redisConnect("127.0.0.1",6379);
    if(c->err){
        redisFree(c);
        return 0;
    }
    start = clock();
    std::string zset1 = "zset2 ";
    for(int i = 0; i < 20000; i++){
        std::string command = "zadd ";

        int s = rand()  % 1000;
        //std::string value = std::to_string(i+1);
        //std::string score = std::to_string(s);
        command.append(zset1);
        command.append(std::to_string(s));  // score
        command.append(" ");
        command.append(std::to_string(i+1));// value
        redisReply *r = (redisReply*) redisCommand(c,command.c_str());
        if(NULL == r){
            std::cout<<"[END] END WITH i="<<i<<"\n";
            redisFree(c);
            return 0;
        }
        if(!(r->type == REDIS_REPLY_STATUS && strcasecmp(r->str,"OK")==0)){
           // std::cout<<"[ERROR] " << command <<" : FAILED!\n";
        }
        freeReplyObject(r);
	    // std::cout<<"[INFO] " << command << " : SUCCESSFUL!\n";
    }
    end = clock();
    double atime = (double)(end - start)/CLOCKS_PER_SEC;
    // std::cout << "(SingleCommand Mode) Time Cost : " << atime * 1000<<"ms\n";
    redisFree(c);
    return atime * 1000;
}
void execWithoutResult(std::string command){
    redisContext* c = redisConnect("127.0.0.1",6379);
    if(c->err){
        redisFree(c);
        return;
    }
    redisReply *r = (redisReply*) redisCommand(c,command.c_str());
    if(NULL == r){
        std::cout<<"[END] redisReply object is NULL\n";
        redisFree(c);
        return;
    }
    freeReplyObject(r);
    redisFree(c);
}

void execResult(std::string command){
    redisContext* c = redisConnect("127.0.0.1",6379);
    if(c->err){
        redisFree(c);
        return;
    }
    redisReply *r = (redisReply*) redisCommand(c,command.c_str());
    if(NULL == r){
        std::cout<<"[END] redisReply object is NULL\n";
        redisFree(c);
        return;
    }
    std::cout<<r->integer;
    freeReplyObject(r);
    redisFree(c);
}
int testFunction(int iter, std::string key, double (*pf)(), std::string title = ""){
    double sum = 0;
    for(int i = 0; i < iter;i++){
        std::cout<<"[Testing] iter = "<<i<<" : ";
        double tmp = (*pf)();
        if(tmp == 0){
            i--;iter--; // 轮次不变，次数变少
        } else {
            sum += tmp;
            std::cout<<"total time = "<< sum <<" ms; step = " << tmp << " ms";
        }
        std::string cmd = "del ";
        execWithoutResult(cmd.append(key));
        std::cout<<"; "<<title<<"\n";
    }
    return sum/iter;
}
int main(){
    double pipelineAvg= 0, singleCommandAvg = 0;
    int iter = 10;
    execResult("zcard zset1");
    //pipeline();
    //pipelineAvg = testFunction(iter,"zset1",pipeline,"pipeline");
    //singleCommandAvg = testFunction(iter,"zset2",singleCommand,"singleCommand");
    std::cout << "(pipeline Mode) Time Cost Avg: " << pipelineAvg <<"ms\n";
    std::cout << "(SingleCommand Mode) Time Cost Avg : " << singleCommandAvg <<"ms\n";
    return 0;
}
