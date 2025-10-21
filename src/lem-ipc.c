#include "../includes/lem-ipc.h"

void	printMap(int *map)
{
	int x = 0;
	int y = 0;

	while (x < MAP_HEIGHT)
	{
		while (y < MAP_WIDTH)
		{
			if (y != 0)
				printf("%2d", map[x * MAP_WIDTH + y]);
			else
				printf("%2d", map[x * MAP_WIDTH + y]);
			y++;
		}
		printf("\n");
		y = 0;
		x++;
	}
}

int		shm_init(t_lem_ipc *all)
{
	all->semId = shmget(all->semKey, sizeof(sem_t *), 0666);
	all->semaphore = (sem_t *) shmat(all->semId, NULL, 0);
	if (!all->semaphore)
	{
		perror("Error semaphore\n");
		return (1);
	}
	//sem_wait(all->semaphore);
	all->mapId = shmget(all->mapKey, MAP_SIZE * sizeof(int), 0666);
	if (all->mapId == -1)
	{
		perror("Error shmget\n");
		return (1);
	}
	all->map = (int *)shmat(all->mapId, NULL, 0);
	if (all->map == (int *)(-1))
	{
		perror("shmat");
		shmdt(all->map);
		shmctl(all->mapId, IPC_RMID, NULL);
		return (1);
	}
	//printMap(all->map);
	//sem_post(all->semaphore);
	return (0);
}

int		first_init(t_lem_ipc *all)
{
	all->semId = shmget(all->semKey, sizeof(sem_t), 0666 | IPC_CREAT | IPC_EXCL);
	all->mapId = shmget(all->mapKey, MAP_SIZE * sizeof(int), 0666 | IPC_CREAT | IPC_EXCL);
	if (all->mapId == -1)
		printf("%d\n", errno);
	all->is_creator = 1;
	all->semaphore = (sem_t *)shmat(all->semId, NULL, 0);
	if (!all->semaphore)
	{
		perror("Error semaphore\n");
		return (1);
	}
	//sem_init(all->semaphore, 1, 1);
	//sem_wait(all->semaphore);
	if (all->mapId == -1)
	{
		perror("Error shmget\n");
		return (1);
	}
	all->map = (int *)shmat(all->mapId, NULL, 0);
	if (all->map == (int *)(-1))
	{
		perror("shmat");
		shmdt(all->map);
		shmctl(all->mapId, IPC_RMID, NULL);
		return (1);
	}
	for (int a = 0; a < MAP_SIZE; a++)
		all->map[a] = 0;
	printMap(all->map);
	//sem_post(all->semaphore);
	return (0);
}

int main()
{
	t_lem_ipc	all;

	all.mapKey = ftok(SHM_KEY_PATH_MAP, SHM_KEY_ID);
	all.semKey = ftok(SHM_KEY_PATH_SEM, SHM_KEY_ID + 1);
	all.is_creator = 0;
	all.semId = shmget(all.semKey, sizeof(sem_t), 0666 | IPC_CREAT | IPC_EXCL);
	all.mapId = shmget(all.mapKey, MAP_SIZE * sizeof(int), 0666 | IPC_CREAT | IPC_EXCL);
	if (all.semId != -1 || all.mapId != -1)
	{
		if (all.mapId != -1)
			shmctl(all.mapId, IPC_RMID, NULL);
		if (all.semId != -1)
			shmctl(all.semId, IPC_RMID, NULL);
		first_init(&all);
	}
	else
		shm_init(&all);
	sleep(5);
	shmctl(all.mapId, IPC_RMID, NULL);
	shmctl(all.semId, IPC_RMID, NULL);
}
