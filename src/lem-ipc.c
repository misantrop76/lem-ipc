#include "../includes/lem-ipc.h"

int main()
{
    key_t key = ftok(SHM_KEY_PATH, SHM_KEY_ID);
	int shmid;
	int is_creator = 0;

    shmid = shmget(key, 1024, 0666 | IPC_CREAT | IPC_EXCL);
	if (shmid == -1)
	{
		printf("c'est pas le premier !\n");
		if (errno == EEXIST)
		{
			shmid = shmget(key, 1024, 0666);
			if (shmid == -1)
			{
				printf("shmget error\n");
				return (1);
			}
		}
		else
		{
			printf("shmget error\n");
			return (1);
		}
	}
	else
	{
		is_creator = 1;
	}

	char *str = (char*) shmat(shmid, NULL, 0);
	if (str == (char *)(-1))
	{
		perror("shmat");
		return (1);
	}
	if (is_creator)
	{
		printf("Je suis le premier !\n");
		strcpy(str, "Hello from creator process");
		printf("Donnee ecrite\n");
		sleep(10);
		shmdt(str);
		shmctl(shmid, IPC_RMID, NULL);
		printf("memoire supprimer !\n");
	}
	else
	{
		sleep (1);
		printf("[Lecteur] Donnée lue depuis la mémoire : %s\n", str);
		shmdt(str);
	}
    return 0;
}
