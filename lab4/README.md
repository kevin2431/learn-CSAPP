# Lab4 cache lab
## Part A: Writing a Cache Simulator
使用C语言写一个Cache模拟器
1. 信息由`main(argc,*argv[])`输入获取
2. 定义结构体 malloc_line
3. 使用malloc 分配cache空间，free回收
4. 若想使用命令行参数提示等，可以利用getopt函数，`#include<unistd.h>`中
### 获取命令行参数
系统输入如下
```
linux> ./csim -s 1 -E 1 -b 1 -t traces/yi2.trace
linux> ./csim -s 4 -E 2 -b 4 -t traces/yi.trace
linux> ./csim -s 2 -E 1 -b 4 -t traces/dave.trace
linux> ./csim -s 2 -E 1 -b 3 -t traces/trans.trace
linux> ./csim -s 2 -E 2 -b 3 -t traces/trans.trace
```
将字符变为数字，其实要是两位数以上，应该写一个函数将字符串转化为相应的整数，但题目中应该都是各位数。
```
unsigned size_s=(*argv[2]-'0');
unsigned size_e=(*argv[4]-'0');
unsigned size_b=(*argv[6]-'0');
```
### cache 结构和分配
```
typedef struct cache_line 
{
	unsigned valid;		//有效位，判断是否使用
	time_t counter;		//获取时钟信息，LRU所用
	unsigned long tag;	//标记位
	
}cache_line;

cache_line* cache=(cache_line*)malloc((2<<size_s)*size_e*sizeof(cache_line));	//malloc cache
```
### 类型判断
利用一个整形flag 记录`hit=1;miss=2;eviction=3`，判断代码如下
```
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
```
根据不同类型，计算次数
```
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
```

结果如下
```
kevin@kevin:~/csapp/lab4/cachelab-handout$ ./test-csim
                        Your simulator     Reference simulator
Points (s,E,b)    Hits  Misses  Evicts    Hits  Misses  Evicts
     3 (1,1,1)       9       8       6       9       8       6  traces/yi2.trace
     3 (4,2,4)       4       5       2       4       5       2  traces/yi.trace
     3 (2,1,4)       2       3       1       2       3       1  traces/dave.trace
     3 (2,1,3)     167      71      67     167      71      67  traces/trans.trace
     3 (2,2,3)     201      37      29     201      37      29  traces/trans.trace
     3 (2,4,3)     212      26      10     212      26      10  traces/trans.trace
     3 (5,1,5)     231       7       0     231       7       0  traces/trans.trace
     6 (5,1,5)  265189   21775   21743  265189   21775   21743  traces/long.trace
    27

TEST_CSIM_RESULTS=27
```
***
##  Part B: Optimizing Matrix Transpose
根据提供的数据，一个cache块中可有8个int。采用分块的思想
```
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int size=8;
    int n=N/size;
    int m=M/size;
    int temp;
    for (int a = 0; a < n; ++a)
        for (int b = 0; b < m; ++b)
            for (int c = 0; c < size; ++c)
                for (int d = 0; d < size; ++d)
                {
                    temp=A[a*size+c][b*size+d];
                    B[b*size+d][a*size+c]=temp;
                }
    int n_r=N%size;
    int m_r=M%size;
    for (int a = 0; a < n; ++a)
        for (int c = 0; c < size; ++c)
            for (int d = 0; d < m_r; ++d)
            {
                temp=A[a*size+c][m*size+d];
                B[m*size+d][a*size+c]=temp;
            }

    for (int b = 0; b < m; ++b)
        for (int c = 0; c < n_r; ++c)
            for (int d = 0; d < size; ++d)
            {
                temp=A[n*size+c][b*size+d];
                B[b*size+d][n*size+c]=temp;
            }

    for (int c = 0; c < n_r; ++c)
        for (int d = 0; d < m_r; ++d)
        {
            temp=A[n*size+c][m*size+d];
            B[m*size+d][n*size+c]=temp;
        }
}
```
测试结果如下
```
Part B: Testing transpose function
Running ./test-trans -M 32 -N 32
Running ./test-trans -M 64 -N 64
Running ./test-trans -M 61 -N 67

Cache Lab summary:
                        Points   Max pts      Misses
Csim correctness          27.0        27
Trans perf 32x32           6.9         8         343
Trans perf 64x64           0.0         8        4723
Trans perf 61x67           8.8        10        2123
          Total points    42.7        53
```
有待更好的算法来改进，做毕设去了，来不及了，这先放一下。
