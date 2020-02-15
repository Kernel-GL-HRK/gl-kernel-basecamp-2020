#include <stdio.h>
#include <time.h>

#define T_BUF_LEN 26

int main(void)
{
	time_t t;
	char t_buf[T_BUF_LEN];
	struct tm *t_info;

	t = time(0);
	t_info = localtime(&t);

	strftime(t_buf, T_BUF_LEN, "%Y-%m-%d %H:%M:%S", t_info);
	printf("%s\n", t_buf);

	return 0;
}
