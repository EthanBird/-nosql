#include "mytest.h"
clock_t start,end,step;
void insert(RedisHandler* r);
int main(){
    RedisHandler* r = new RedisHandler("127.0.0.1",6379);
    r->del("zset1");
    r->del("zset2");
    dotest(r);
    return 0;
}
void dotest(RedisHandler* r){
    start = clock();
    step = clock();
    for(int i = 0; i < 20000; i++){
        int s = rand() % 65535;
        r->zadd("zset2", std::to_string(s).c_str(), std::to_string(i+1).c_str());
    }
    std::cout << "(非管道模式) 插入20k个数据，耗时 : " << (double)(clock() - step)/CLOCKS_PER_SEC * 1000<<"ms\n";

    step = clock();
    std::cout<<"Max(score) = "<< r->zmaxScore("zset2")
             <<" (Step Time cost : "<< (double)(clock() - step)/CLOCKS_PER_SEC * 1000 <<"ms)\n";
    step = clock();
    std::cout<<"Min(score) = "<< r->zminScore("zset2")
                 <<" (Step Time cost : "<< (double)(clock() - step)/CLOCKS_PER_SEC * 1000 <<"ms)\n";
    step = clock();
    std::cout<<"Avg(score) = "<< r->zavg("zset2")
                 <<" (Step Time cost : "<< (double)(clock() - step)/CLOCKS_PER_SEC * 1000 <<"ms)\n";
    step = clock();
    std::cout<<"Std(score) = "<< r->zstd("zset2")
                 <<" (Step Time cost : "<< (double)(clock() - step)/CLOCKS_PER_SEC * 1000 <<"ms)\n";
    step = clock();
    std::cout<<"Sum(score) = "<< r->zsum("zset2")
                 <<" (Step Time cost : "<< (double)(clock() - step)/CLOCKS_PER_SEC * 1000 <<"ms)\n";
    end = clock();
    std::cout << "(非管道模式) 总耗时 : " << (double)(end - start)/CLOCKS_PER_SEC * 1000<<"ms\n";

    RedisHandler*p = r->pipeline();
    start = clock();
    step = clock();
    for(int i = 0; i < 20000; i++){
        int s = rand() % 65535;
        p->zadd("zset1", std::to_string(s).c_str(), std::to_string(i+1).c_str());
    }
    for(int i = 0; i < 20000; i++){
        p->pipelineSubmit();
    }
    std::cout << "(管道模式) 插入20k个数据，耗时 : " << (double)(clock() - step)/CLOCKS_PER_SEC * 1000<<"ms\n";

    step = clock();
    std::cout<<"Max(score) = "<< p->zmaxScore("zset1")
             <<" (Step Time cost : "<< (double)(clock() - step)/CLOCKS_PER_SEC * 1000 <<"ms)\n";
    step = clock();
    std::cout<<"Min(score) = "<< p->zminScore("zset1")
                 <<" (Step Time cost : "<< (double)(clock() - step)/CLOCKS_PER_SEC * 1000 <<"ms)\n";
    step = clock();
    std::cout<<"Avg(score) = "<< p->zavg("zset1")
                 <<" (Step Time cost : "<< (double)(clock() - step)/CLOCKS_PER_SEC * 1000 <<"ms)\n";
    step = clock();
    std::cout<<"Std(score) = "<< p->zstd("zset1")
                 <<" (Step Time cost : "<< (double)(clock() - step)/CLOCKS_PER_SEC * 1000 <<"ms)\n";
    step = clock();
    std::cout<<"Sum(score) = "<< p->zsum("zset1")
                 <<" (Step Time cost : "<< (double)(clock() - step)/CLOCKS_PER_SEC * 1000 <<"ms)\n";
    end = clock();
    std::cout << "(管道模式) 总耗时 : " << (double)(end - start)/CLOCKS_PER_SEC * 1000<<"ms\n";
}
