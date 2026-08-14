#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <json/json.h>
#include <fcntl.h>

void parse_json(const char *json, char *content)
{
	//字符串转换成json对象
	struct json_object *obj = json_tokener_parse(json);
	if (NULL == obj)
	{
		fprintf(stderr, "不是一个json对象\n");
		return;
	}

	//1.根据 choice 解析得到数组
	struct json_object *arr;
	arr = json_object_object_get(obj, "choices");
	if (NULL == arr || json_object_get_type(arr) != json_type_array)
	{
		fprintf(stderr, "不是一个数组对象\n");
		return;
	}

	//获取数组的第一个元素
	struct json_object *first_choice;
	first_choice = json_object_array_get_idx(arr, 0);
	
	struct json_object *message;
	message = json_object_object_get(first_choice, "message");

	struct json_object *cont;
	cont = json_object_object_get(message, "content");

	strcpy(content, json_object_get_string(cont));

	json_object_put(obj);
}

int main(int argc, char *argv[])
{
	if (argc != 2)
		return -1;

	char command[1024] = {0};
	sprintf(command, "/home/qwen/qwen.sh %s 2>/dev/null", argv[1]);

	FILE *fp = popen(command, "r");
	if (NULL == fp)
	{
		perror("popen");
		return -1;
	}

	char buf[2048] = {0};
	fgets(buf, sizeof(buf), fp);

	//printf("--> %s\n", buf);

	pclose(fp);

	char content[1024] = {0};
	parse_json(buf, content);

	if (strlen(content) > 0)
	{
		printf("-->%s\n", content);

		//打开管道
		int tts_fd = open("/home/fifo/tts_fifo", O_WRONLY);
		if (-1 == tts_fd)
		{
			perror("open");
			return -1;
		}

		//写入数据
		if (write(tts_fd, content, strlen(content)) == -1)
		{
			perror("write");
		}

		close(tts_fd);
	}

	return 0;
}
