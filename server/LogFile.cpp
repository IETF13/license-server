#include <iostream>
#include <stdio.h>
#include <memory.h>;
#pragma warning(disable:4996)
#define USERNUM 10
#define PASSWORDLEN 10
using namespace std;
void LogToFile(char* password, bool ticket[])
{
	FILE* fp;
	if ((fp = fopen("Log.txt", "w")) != NULL)
	{
		for (unsigned int i = 0; i < PASSWORDLEN; i++)
			fprintf(fp, "%c", password[i]);
		fprintf(fp, "\n");
		for (int i = 0; i < USERNUM; i++)
			fprintf(fp, "%d", ticket[i]);
		fclose(fp);
	}
	else
	{
		printf("open file error");
	}

}
void ReadLogFile(char* password, bool ticket[])
{
	FILE* fp;
	if ((fp = fopen("Log.txt", "r")) != NULL)
	{
		if (!feof(fp))
		{
			fgets(password, PASSWORDLEN + 1, fp);
		}
		int i = 0;
		while (!feof(fp))
		{
			char tmp = fgetc(fp);
			if (tmp == '\n')
			{
				continue;
			}
			ticket[i] = tmp - '0';
			i++;
			if (i >= USERNUM)
			{
				break;
			}
		}
		fclose(fp);
	}
	else
	{
		printf("open file error");
	}

}