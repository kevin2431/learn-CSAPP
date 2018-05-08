#include "cachelab.h"
#include<stdio.h>
#include <stdlib.h>
#include<string.h>
#include <time.h>

typedef struct cache_line 
{
	unsigned valid;
	time_t counter;		//获取时钟信息，LRU所用
	unsigned long tag;
	
}cache_line;

int main(int argc,char *argv[])
{
	//获取输入信息
	unsigned size_s=(*argv[2]-'0');
	unsigned size_e=(*argv[4]-'0');
	unsigned size_b=(*argv[6]-'0');
	//printf("%d %d %d \n",size_s,size_e,size_b ); correct
	FILE *fp;
	fp=fopen(argv[8],"r");
	char access_type;
	unsigned long address;
	int size;
	//计数
	int miss_count=0,hit_count=0,eviction_count=0;
	cache_line* cache=(cache_line*)malloc((2<<size_s)*size_e*sizeof(cache_line));	//malloc cache
	//初始化
	for (int i = 0; i < (2<<size_s)*size_e; ++i)
	{
		cache[i].valid=0;
		cache[i].tag=0;
	}
	//后续取地址所用，位运算
	unsigned long s=0,b=0;
	for (int i = 0; i < size_b; ++i)
	{
		b=(b<<1)+1;
	}
	for (int i = 0; i < size_s; ++i)
	{
		s=(s<<1)+1;
	}
	s=s<<size_b;
	//printf("%lx %lx\n",b,s);
	/*
	这里显然block大小没有用到，在极端情况下，会需要两个cache块
	*/
	while(fscanf(fp," %c %lx,%d",&access_type,&address,&size)>0){
		int flag=0;
		if(access_type=='I') continue;
		unsigned address_s=(address&s)>>size_b;		//
		unsigned long tag=address>>(size_b+size_s);
		unsigned now=address_s*size_e;
		// hit 1 ;miss 2 ;evic 3;
		for (; now < address_s*size_e+size_e; ++now)
		{
			if(cache[now].valid==0){	//miss ,have place
				flag=2;
				cache[now].valid=1;
				cache[now].tag=tag;
				cache[now].counter=clock();
				break;
			}else {
			if(cache[now].tag==tag){	//hit
				flag=1;
				cache[now].counter=clock();
				break;
			}
		}
		}
		if(now==(address_s*size_e+size_e)&&flag==0) {	//miss have no place, so evic
			flag=3;										//if flag!=0, then just miss or hit
			now=address_s*size_e;
			for (int i = 0; i < size_e; ++i)
			{
				if(cache[address_s*size_e+i].counter<cache[now].counter)
					now=address_s*size_e+i;
			}
			cache[now].tag=tag;
			cache[now].counter=clock();
		}
		if(access_type=='L'||access_type=='S')
		{
			switch(flag){
			case 1:	hit_count++;
					printf("%lx %x hit\n",address,address_s);
					break;
			case 2: miss_count++;
			printf("%lx %x miss\n",address,address_s);
					break;
			case 3:	miss_count++;
					eviction_count++;
					printf("%lx %x miss ecic\n",address,address_s);
			default:break;
			}
		}else {
			switch(flag){
			case 1:	hit_count+=2;
			printf("%lx %x hit hit\n",address,address_s);
					break;
			case 2: miss_count++;
					hit_count++;
					printf("%lx %x miss hit\n",address,address_s);
					break;
			case 3:	miss_count++;
					eviction_count++;
					hit_count++;
					printf("%lx %x miss evic hit\n",address,address_s);
			default:break;
			}
		}


	}
	fclose(fp);
    printSummary(hit_count, miss_count, eviction_count);
    free(cache);
    return 0;
}
