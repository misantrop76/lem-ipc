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
	//last_exit(all);
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

// int		myBfs(int *map, int pos)
// {
// 	int	backPos[MAP_SIZE] = {-1};
// 	t_list	*to_check;

// 	backPos[pos] = pos;

// 	ft_lstadd_back(&to_check, ft_lstnew(pos))
// 	while (ft_lstsize(to_check))
// 	{

// 	}

// }

// void	move(t_lem_ipc *all)
// {
// 	int *mapSnap[MAP_SIZE];

// 	sem_wait(all->semaphore);
// 	ft_memcpy(mapSnap, all->map, MAP_SIZE);
// 	sem_post(all->semaphore);
	
// }

void	move(t_lem_ipc *all)
{
	int dx = (all->pos % MAP_WIDTH) + ((rand() % 3) - 1);
	int dy = (all->pos / MAP_WIDTH) + ((rand() % 3) - 1);
	int newpos = (MAP_WIDTH * dy) + dx;

	sem_wait(all->semaphore);
	while (dx < 0 || dx >= MAP_WIDTH || dy < 0 || dy >= MAP_HEIGHT || all->map[newpos] != 0)
	{
		dx = (all->pos % MAP_WIDTH) + ((rand() % 3) - 1);
		dy = (all->pos / MAP_WIDTH) + ((rand() % 3) - 1);
		newpos = (MAP_WIDTH * dy) + dx;
	}
	all->map[all->pos] = 0;
	all->pos = newpos;
	all->map[newpos] = all->teamId;
	sem_post(all->semaphore);
}

void	my_sleep(unsigned long ms)
{
	struct timeval time;
	gettimeofday(&time, NULL);
	unsigned long start_ms = (time.tv_sec * 1000) + (time.tv_usec / 1000);
	unsigned long now_ms = start_ms;
	while (now_ms - start_ms < ms)
	{
		gettimeofday(&time, NULL);
		now_ms = (time.tv_sec * 1000) + (time.tv_usec / 1000);
	}
}

void	error_help()
{
	ft_putstr_fd("Error with arguments\n", 1);
	ft_putstr_fd("Usage:\n", 1);
	ft_putstr_fd("./lem-ipc team_ID\n", 1);
	exit (1);
}

int is_last_team(t_lem_ipc *all)
{
	int firstVal = 0;

	sem_wait(all->semaphore);
	for (int i = 0; i < MAP_SIZE; i++)
	{
		if (firstVal == 0 && all->map[i] != 0)
			firstVal = all->map[i];
		if (firstVal != 0 && all->map[i] != firstVal && all->map[i] != 0)
		{
			sem_post(all->semaphore);
			return (0);
		}
	}
	sem_post(all->semaphore);
	printMap(all->map, all->semaphore);
	return (1);
}

int	is_dead(t_lem_ipc *all)
{
	int neightbors[8] = {0};
	int x = all->pos % MAP_WIDTH;
	int y = all->pos / MAP_WIDTH;
	int posCheck;

	sem_wait(all->semaphore);
	for (int dx = -1; dx <= 1; dx++)
	{
		for (int dy = -1; dy <= 1; dy++)
		{
			if ((dx == 0 && dy == 0) || (dx + x) < 0 || (dy + y) < 0 || (dx + x) >= MAP_WIDTH || (dy + y) >= MAP_HEIGHT)
				continue;
			posCheck = ((dy + y) * MAP_WIDTH) + (dx + x);
			if (all->map[posCheck] != 0 && all->map[posCheck] != all->teamId)
			{
				int i = 0;
				while (i < 8 && neightbors[i] != 0)
				{
					if (neightbors[i] == all->map[posCheck])
					{
						sem_post(all->semaphore);
						return (1);
					}
					i++;
				}
				if (i < 8 && neightbors[i] == 0)
					neightbors[i] = all->map[posCheck];
			}
		}
	}
	sem_post(all->semaphore);
	printMap(all->map, all->semaphore);
	return (0);
}

int main(int ac, char **av)
{
	t_lem_ipc	all;

	srand(time(NULL) ^ getpid());
	if (ac != 2 || ft_atoi(av[1]) < 1)
		all.teamId = 42;
	else
		all.teamId = ft_atoi(av[1]);
	all.mapKey = ftok(SHM_KEY_PATH_MAP, SHM_KEY_ID);
	all.semKey = ftok(SHM_KEY_PATH_SEM, SHM_KEY_ID + 1);
	all.semId = shmget(all.semKey, sizeof(sem_t), 0666);
	all.mapId = shmget(all.mapKey, MAP_SIZE * sizeof(int), 0666);
	if (all.semId == -1 || all.mapId == -1)
		shm_init(&all);
	else
		getShm(&all);

	all.pos = getPos(all.map, all.semaphore);
	sem_wait(all.semaphore);
	all.map[all.pos] = all.teamId;
	sem_post(all.semaphore);
	while (is_last_team(&all))
		my_sleep(100);
	while (!is_dead(&all) && !is_last_team(&all))
	{
		my_sleep(100);
		move(&all);
		printMap(all.map, all.semaphore);
		my_sleep(100);
	}
	if (is_last(all.map, all.semaphore))
		last_exit(&all);
	sem_wait(all.semaphore);
	all.map[all.pos] = 0;
	sem_post(all.semaphore);
	return (0);
}
