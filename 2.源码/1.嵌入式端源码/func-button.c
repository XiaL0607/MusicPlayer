#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <signal.h>
#include <sys/time.h>

typedef enum{
	STATE_IDLE,
	STATE_FIRST_PRESS,
	STATE_FIRST_RELEASE,
	STATE_SECOND_PRESS
}BUTTON_STATE;

BUTTON_STATE state = STATE_IDLE;

struct itimerval tv;

void button_handler(int sig)
{
	printf("短按\n");

	state = STATE_IDLE;
}

int main()
{
	signal(SIGALRM, button_handler);

	int fd = open("/dev/input/event1", O_RDONLY);
	if (-1 == fd)
	{
		perror("open");
		return -1;
	}

	struct input_event ev;
	struct timeval old, new;

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
			if (state == STATE_IDLE)
			{
				gettimeofday(&old, NULL);

				state = STATE_FIRST_PRESS;
			}
			else if (state == STATE_FIRST_RELEASE)
			{
				printf("双击\n");

				state = STATE_IDLE;

				tv.it_value.tv_sec = 0;
				tv.it_value.tv_usec = 0;

				tv.it_interval.tv_sec = 0;
				tv.it_interval.tv_usec = 0;

				//取消定时器
				setitimer(ITIMER_REAL, &tv, NULL);

			}
		}
		else if (ev.value == 0)
		{
			if (state == STATE_FIRST_PRESS)
			{
				gettimeofday(&new, NULL);

				if ((new.tv_sec * 1000 + new.tv_usec / 1000) - (old.tv_sec * 1000 + old.tv_usec / 1000) > 300)
				{
					printf("长按\n");

					state = STATE_IDLE;
				}
				else 
				{
					state = STATE_FIRST_RELEASE;

					tv.it_value.tv_sec = 0;
					tv.it_value.tv_usec = 300 * 1000;

					tv.it_interval.tv_sec = 0;
					tv.it_interval.tv_usec = 0;

					//启动定时器
					setitimer(ITIMER_REAL, &tv, NULL);
				}
			}
		}
	}

	close(0);

	return 0;
}
