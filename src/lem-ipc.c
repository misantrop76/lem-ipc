#include "../includes/lem-ipc.h"
#include "../includes/global.h"
#include <stdio.h>

extern int g_is_signal;

void	last_exit(t_lem_ipc *all)
{
	ft_putstr_fd("Mémoire partagée supprimer !\n", 1);
	sem_destroy(all->semaphore);
	shmdt(all->map);
	shmdt(all->semaphore);
	shmctl(all->mapId, IPC_RMID, NULL);
	shmctl(all->semId, IPC_RMID, NULL);
	msgctl(all->msgId, IPC_RMID, NULL);
	exit(0);
}

int	getPos(int *map, sem_t *sem, int teamId)
{
	int random = MAP_SIZE - 1;
	int safe = 0;

	sem_wait(sem);
	do
	{
		random = rand() % (MAP_SIZE - 1);
		safe++;
	}
	while (map[random] != 0 && safe < 1000);
	if (map[random] != 0)
		for (int a = 0; a < MAP_SIZE; a++)
			if (map[a] == 0)
				random = a;
	if (map[random] != 0 || map[MAP_SIZE] == 1)
	{
		sem_post(sem);
		return (-1);
	}
	map[random] = teamId;
	sem_post(sem);
	return (random);
}

int		getShm(t_lem_ipc *all)
{
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
	msgctl(all->msgId, IPC_RMID, NULL);
	all->semId = shmget(all->semKey, sizeof(sem_t), 0666 | IPC_CREAT | IPC_EXCL);
	all->mapId = shmget(all->mapKey, (MAP_SIZE + 1) * sizeof(int), 0666 | IPC_CREAT | IPC_EXCL);
	all->msgId = msgget(all->msgKey, 0666 | IPC_CREAT);
	if (all->semId == -1 || all->mapId == -1 || all->msgId == -1)
		exit(1);
	all->semaphore = (sem_t *)shmat(all->semId, NULL, 0);
	all->map = (int *)shmat(all->mapId, NULL, 0);
	if (all->semaphore == (sem_t *)(-1) || all->map == (int *)(-1))
		exit (1);
	sem_init(all->semaphore, 1, 1);
	sem_wait(all->semaphore);
	for (int a = 0; a <= MAP_SIZE; a++)
		all->map[a] = 0;
	sem_post(all->semaphore);
	return (0);
}

void	error_help()
{
	ft_putstr_fd("Error with arguments\n", 1);
	ft_putstr_fd("Usage:\n", 1);
	ft_putstr_fd("./lem-ipc team_ID\n", 1);
	ft_putstr_fd("with 0 < team_ID < 10000\n", 1);
	exit (1);
}

int		isInTeamId(int *teamId, int check)
{
	for (int a = 0; a < MAP_SIZE; a++)
	{
		if (teamId[a] == check)
			return (1);
	}
	return (0);
}

int is_last_team(t_lem_ipc *all)
{
	int firstVal = 0;
	int val = 1;

	sem_wait(all->semaphore);
	for (int i = 0; i < MAP_SIZE; i++)
	{
		if (all->map[i] != 0)
		{
			if (firstVal != 0 && all->map[i] != firstVal)
				val = 0;
			firstVal = all->map[i];
		}
	}
	sem_post(all->semaphore);
	return (val);
}

int checkGameStatus(t_lem_ipc *all)
{
	int status;
	sem_wait(all->semaphore);
	status = all->map[MAP_SIZE];
	sem_post(all->semaphore);
	return (status);
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
	return (0);
}

int	checkExitMsg(t_lem_ipc *all)
{
	t_msg	message;

	message.ctype = 1;
	message.command = 1;
	if (msgrcv(all->msgId, &message, sizeof(message.command), 0, IPC_NOWAIT) != -1)
	{
		msgsnd(all->msgId, &message, sizeof(message.command), 0);
		return (1);
	}
	return (0);
}

void	game(t_lem_ipc *all, int move_pattern)
{
	all->pos = getPos(all->map, all->semaphore, all->teamId);
	if (all->pos == -1)
	{
		ft_putstr_fd("Error: trop de bots sur la map\n", 1);
		return;
	}
	while (!checkGameStatus(all) && !checkExitMsg(all) && g_is_signal == 0)
		usleep(100000);
	while (!is_dead(all) && !is_last_team(all) && !checkExitMsg(all) && g_is_signal == 0)
	{
		if (checkGameStatus(all))
			move(all, move_pattern);
		usleep(5500000 / MAP_WIDTH);
	}
	if (is_last(all->map, all->semaphore) == 1)
		last_exit(all);
	sem_wait(all->semaphore);
	all->map[all->pos] = 0;
	sem_post(all->semaphore);
}

void signal_handler(int sig)
{
    (void)sig;
    ft_putstr_fd("signal recu !\n", 1);
	g_is_signal = 1;
}

int main(int ac, char **av)
{
	t_lem_ipc	all;
	int			move_pattern = 0;

	srand(time(NULL) ^ getpid());
	g_is_signal = 0;
	if (ac < 2 || ft_atoi(av[1]) < 1 || ft_atoi(av[1]) > 10000 || MAP_HEIGHT < 5 ||
	MAP_HEIGHT > 100 || MAP_HEIGHT != MAP_WIDTH)
		error_help();
	else
		all.teamId = ft_atoi(av[1]);
	if (ac > 2)
		move_pattern = 1;
	all.mapKey = ftok(SHM_KEY_PATH, SHM_KEY_ID);
	all.semKey = ftok(SHM_KEY_PATH, SHM_KEY_ID + 1);
	all.msgKey = ftok(SHM_KEY_PATH, SHM_KEY_ID + 2);
	all.msgId = msgget(all.msgKey, 0666);
	all.semId = shmget(all.semKey, sizeof(sem_t), 0666);
	all.mapId = shmget(all.mapKey, (MAP_SIZE + 1) * sizeof(int), 0666);
	if (all.semId == -1 || all.mapId == -1 || all.msgId == -1)
		shm_init(&all);
	else
		getShm(&all);
    signal(SIGINT, signal_handler);
	signal(SIGABRT, signal_handler);
	game(&all, move_pattern);
	shmdt(all.map);
	shmdt(all.semaphore);
	return (0);
}
