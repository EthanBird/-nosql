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
#include <cmath>
const int PIPELINE = 1;
const int NON_PIPELINE = 2;
class RedisHandler{
    public:
        redisContext* context;
        redisReply* reply = NULL;
        int mode;
        RedisHandler(const char * ip="127.0.0.1",int port=6379, int mode = NON_PIPELINE){
            this->mode = mode;
            this->context = redisConnect(ip,port);
            if(this->context == NULL || context->err){
                if(context){
                    printf("Error: %s\n",context->errstr);
                    exit(-1);
                }else{
                    printf("can not allocate redis context\n");
                    exit(-1);
                }
            }
        }
        long long zstd(const char* zkey){
            bool isStd = true;
            return zavg(zkey, isStd);
        }
        long long zavg(const char* zkey, bool isStd = false){
            bool isAvg = true;
            if(isStd){
                long long avg = zsum(zkey, isAvg);
                return zsum(zkey, false, isStd, avg);
            }
            return zsum(zkey, isAvg);
        }
        long long zsum(const char* zkey, bool isAvg = false, bool isStd = false, long long avg = 0){
            long long sum = 0;
            int len = 0;
            long long squareSum = 0;
            redisReply* reply = NULL;
            reply = (redisReply*) redisCommand(this->context, "zcard %s",zkey);
            if(REDIS_REPLY_INTEGER == reply->type){
                len = (int) reply->integer;
            }
            reply = (redisReply*) redisCommand(this->context, "zrange %s 0 -1 withscores",zkey);
            for(int i = 0; i < len; i++){
                long long xi = atoll(reply->element[2*i+1]->str);
                if(isStd){
                    long long m = (xi - avg);
                    squareSum += m * m;
                } else {
                    sum += xi;
                }
            }
            if(isAvg){
                if(len == 0) return 0xFFFFFFFFFFFFFFFF;
                else return sum/len;
            }
            if(isStd){
                if(len == 0) return 0xFFFFFFFFFFFFFFFF;
                else return sqrt(squareSum/len);
            }
            return sum;

        }
        long long zmaxScore(const char* zkey){
            redisReply* reply = NULL;
            reply = (redisReply*) redisCommand(this->context, "zrange %s -1 -1 withscores",zkey);
            if(reply->type == REDIS_REPLY_ARRAY ){
                long long l = atoll(reply->element[1]->str);
                freeReplyObject(reply);
                return l;
            }
            std::cout<<"[ERROR] reply->type = "<<reply->type<<"\n";
            if(reply->type == 6) std::cout<<reply->str<<"\n";
            freeReplyObject(reply);
            return -1;

        }
        long long zminScore(const char* zkey){
            redisReply* reply = NULL;
            reply = (redisReply*) redisCommand(this->context, "zrevrange %s -1 -1 withscores",zkey);
            if(reply->type == REDIS_REPLY_ARRAY ){
                long long l = atoll(reply->element[1]->str);
                freeReplyObject(reply);
                return l;
            }
            std::cout<<"[ERROR] reply->type = "<<reply->type<<"\n";
            if(reply->type == 6) std::cout<<reply->str<<"\n";
            freeReplyObject(reply);
            return -1;
        }
        int zadd(const char* zkey, const char* score, const char* value){
            if(this->mode == NON_PIPELINE){
                redisReply* reply = (redisReply*) redisCommand(this->context, "ZADD %s %s %s",zkey , score, value);
                freeReplyObject(reply);

            } else {
                if(REDIS_OK != redisAppendCommand(this->context, "ZADD %s %s %s",zkey , score, value)){
                    printf("Failed to execute command : ZADD %s %s %s",zkey, score, value);
                    redisFree(this->context);
                    return -1;
                }
            }
            return 0;
        }
        RedisHandler* pipeline(){
            RedisHandler* r = new RedisHandler();
            r->mode = PIPELINE;
            return r;
        }
        RedisHandler* toNonPipeline(){
            this->mode = NON_PIPELINE;
            return this;
        }
        redisReply* pipelineSubmit(){
            if(this->mode != PIPELINE){
                std::cout << "Now mode is not pipeline\n";
                return NULL;
            }

            int status = redisGetReply(this->context,(void**)&(this->reply));
            if(REDIS_OK != status){
                printf("Failed to execute command with Pipeline.");
                freeReplyObject(reply);
                return NULL;
            }
            if(REDIS_REPLY_NIL == reply->type){
                return NULL;
            }
            return reply;
        }
        /**delete key*/
        int del(const char* key){
            int res = 0;
            redisReply* reply = (redisReply*)redisCommand(this->context, "DEL %s", key);
            if(reply->type == REDIS_REPLY_INTEGER){
                if(reply->integer == 1L)
                    res = 1;
            }
            freeReplyObject(reply);
            return res;
        }

        /*if Key ID exists*/
        int existsKey(const char* ID){
            redisReply * reply = (redisReply*)redisCommand(this->context,"exists %s",ID);
            int res = 0;
            if(reply->type == REDIS_REPLY_INTEGER){
                if(reply->integer == 1L)
                    res  = 1;
            }
            freeReplyObject(reply);
            return res;
        }
        virtual ~RedisHandler(){
            freeReplyObject(this->reply);
            redisFree(this->context);

        }
    protected:
    private:
};
