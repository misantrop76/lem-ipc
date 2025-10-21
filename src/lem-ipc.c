#include "../includes/lem-ipc.h"

void	printMap(int *map, sem_t *sem)
{
	int x = 0;
	int y = 0;

	sem_wait(sem);
    printf("\033[2J\033[H");
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
	fflush(stdout);
	sem_post(sem);
}

int		is_last(int *map, sem_t *sem)
{
	int check = 0;

	sem_wait(sem);
	for (int a = 0; a < MAP_SIZE; a++)
	{
		if (map[a] != 0)
		{
			check++;
			if (check > 1)
			{
				sem_post(sem);
				return (0);
			}
		}
	}
	sem_post(sem);
	return (1);
}

void	last_exit(t_lem_ipc *all)
{
	printf("Last exit call\n");
	shmdt(all->map);
	sem_close(all->semaphore);
	shmdt(all->semaphore);
	shmctl(all->mapId, IPC_RMID, NULL);
	shmctl(all->semId, IPC_RMID, NULL);
	exit(0);
}

int		getPos(int *map, sem_t *sem)
{
	int random;

	sem_wait(sem);
	do
	{
		random = rand() % (MAP_SIZE - 1);

	} while (map[random] != 0);
	sem_post(sem);
	return (random);
}

int		getShm(t_lem_ipc *all)
{
	all->semId = shmget(all->semKey, sizeof(sem_t *), 0666);
	all->mapId = shmget(all->mapKey, MAP_SIZE * sizeof(int), 0666);
	if (all->mapId == -1 || all->semId == -1)
		exit (1);

	all->semaphore = (sem_t *) shmat(all->semId, NULL, 0);
	all->map = (int *)shmat(all->mapId, NULL, 0);
	if (all->semaphore == (sem_t *)(-1) || all->map == (int *)(-1))
		exit (1);
	return (0);
}

int		shm_init(t_lem_ipc *all)
{
	shmctl(all->mapId, IPC_RMID, NULL);
	shmctl(all->semId, IPC_RMID, NULL);
	all->semId = shmget(all->semKey, sizeof(sem_t), 0666 | IPC_CREAT | IPC_EXCL);
	all->mapId = shmget(all->mapKey, MAP_SIZE * sizeof(int), 0666 | IPC_CREAT | IPC_EXCL);
	if (all->semId == -1 || all->mapId == -1)
		exit(1);

	all->semaphore = (sem_t *)shmat(all->semId, NULL, 0);
	all->map = (int *)shmat(all->mapId, NULL, 0);
	if (all->semaphore == (sem_t *)(-1) || all->map == (int *)(-1))
		exit (1);

	sem_init(all->semaphore, 1, 1);
	sem_wait(all->semaphore);
	for (int a = 0; a < MAP_SIZE; a++)
		all->map[a] = 0;
	sem_post(all->semaphore);
	return (0);
}

// int randomPos(int x, int y, int *map, sem_t *sem)
// {
// 	int a;
// 	int random;
// 	do
// 	{
// 		random = rand() % 3;
// 		int dx = 
// 		a = 
// 		/* code */
// 	} while (map[a] != 0);
	
// }

int main()
{
	t_lem_ipc	all;

	srand(time(NULL) ^ getpid());
	all.mapKey = ftok(SHM_KEY_PATH_MAP, SHM_KEY_ID);
	all.semKey = ftok(SHM_KEY_PATH_SEM, SHM_KEY_ID + 1);
	all.semId = shmget(all.semKey, sizeof(sem_t), 0666);
	all.mapId = shmget(all.mapKey, MAP_SIZE * sizeof(int), 0666);
	if (all.semId == -1 || all.mapId == -1)
		shm_init(&all);
	else
		getShm(&all);

	int pos = getPos(all.map, all.semaphore);
	sem_wait(all.semaphore);
	all.map[pos] = 44;
	sem_post(all.semaphore);
	// for (int a = 0; a < 20; a++)
	// {
	// 	pos = randomPos();
	// 	printMap(all.map, all.semaphore);
	// 	sleep(1);
	// }
	if (is_last(all.map, all.semaphore))
		last_exit(&all);
	sem_wait(all.semaphore);
	all.map[pos] = 0;
	sem_post(all.semaphore);
	return (0);
}
