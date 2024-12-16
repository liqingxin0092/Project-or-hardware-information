
#include "uart.h"

void delay(int d)
{
	while(d--);
}

int main()
{
	char c;
	
	uart_init();
	
	putchar('1');
	putchar('0');
	putchar('0');
	putchar('a');
	putchar('s');
	putchar('k');
	putchar('\n');
	putchar('\r');
	
	while (1)
	{
		c = getchar();
		putchar(c);
		putchar(c+1);
	}
	
	return 0;
}
