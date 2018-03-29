#CSAPP 深入理解计算机系统

to be a really CS zjuer

希望在大四毕业前学完这本书，边做毕设边学代码的机械狗

```
printf("hello world\n");

```
以下实验基于Carnegie Mellon 15-213/18-213/15-513: [Introduction to Computer Systems (ICS)](http://www.cs.cmu.edu/afs/cs/academic/class/15213-f17/www/index.html)

***

###LAB0
一些陷阱

- 单向链表的头插法和尾插法（是否带头结点，写法会有不同）
- malloc无法分配地址时，返回NULL。且不可free
- You dereferenced a NULL or invalid pointer。对空链表或空节点（NULL)进行free操作或引用便会报错。尤其下面这个函数最为典型
```
bool q_remove_head(queue_t *q, int *vp)
...
if(q==NULL||q->head==NULL||vp==NULL) return 0;
...
```
- 链表进行reverse时，防止断链，否则结果有问题


***


