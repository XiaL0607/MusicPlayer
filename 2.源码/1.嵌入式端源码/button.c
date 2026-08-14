#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

int main()
{
	int fd = open("/dev/input/event1", O_RDONLY);
	if (-1 == fd)
	{
		perror("open");
		return -1;
	}

	struct input_event ev;

	while (1)
	{
		int ret = read(fd, &ev, sizeof(ev));
		if (-1 == ret)
		{
			perror("read");
			continue;
		}

		if (ev.type != EV_KEY)
		{
			continue;
		}

		if (ev.value == 1)
		{
			printf("按键被按下\n");
		}
		else if (ev.value == 0)
		{
			printf("按键松开\n");
		}
	}

	close(0);

	return 0;
}
