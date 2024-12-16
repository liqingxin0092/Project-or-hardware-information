#include <stdio.h>
extern void func_b();
extern void func_c();
int main()
{
    func_b();
    func_c();
    while (1)
        ;
    printf("这个函数执行了");
    return 0;
}