#include "../includes/lem-ipc.h"

void	last_exit(t_lem_ipc *all)
{
	printf("Last exit call\n");
	sem_destroy(all->semaphore);
	shmdt(all->map);
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
		random = rand() % (MAP_SIZE - 1);
	while (map[random] != 0);
	sem_post(sem);
	return (random);
}

int	moveRandom(int *map, int pos)
{
	int dx = (pos % MAP_WIDTH) + ((rand() % 3) - 1);
	int dy = (pos / MAP_WIDTH) + ((rand() % 3) - 1);
	int newpos = (MAP_WIDTH * dy) + dx;

	while (dx < 0 || dx >= MAP_WIDTH || dy < 0 || dy >= MAP_HEIGHT || map[newpos] != 0)
	{
		dx = (pos % MAP_WIDTH) + ((rand() % 3) - 1);
		dy = (pos / MAP_WIDTH) + ((rand() % 3) - 1);
		newpos = (MAP_WIDTH * dy) + dx;
	}
	return (newpos);
}

int		getShm(t_lem_ipc *all)
{
	all->semId = shmget(all->semKey, sizeof(sem_t), 0666);
	all->mapId = shmget(all->mapKey, (MAP_SIZE + 1) * sizeof(int), 0666);
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
	all->mapId = shmget(all->mapKey, (MAP_SIZE + 1) * sizeof(int), 0666 | IPC_CREAT | IPC_EXCL);
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
	all->map[MAP_SIZE] = 0;
	sem_post(all->semaphore);
	return (0);
}

int		isInContact(int pos, int checkPos, int deg)
{
	int x = pos % MAP_WIDTH;
	int y = pos / MAP_WIDTH;
	int checkX = checkPos % MAP_WIDTH;
	int checkY = checkPos / MAP_WIDTH;

	if (checkX < x - deg || checkX > x + deg || checkY < y - deg || checkY > y + deg)
		return (0);
	return (1);
}

int		isInToCheck(int pos, t_list *to_check)
{
	t_list *tmp;

	tmp = to_check;
	while (tmp)
	{
		t_room *room = (t_room *)tmp->content;
		if (room->pos == pos)
			return (1);
		tmp = tmp->next;
	}
	return (0);
}

t_room	*GetNewMapRoom(int pos)
{
	t_room *newMapRoom;

	newMapRoom = malloc(sizeof(t_room) * MAP_SIZE);
	for (int a = 0; a < MAP_SIZE; a++)
	{
		newMapRoom[a].isVisited = 0;
		newMapRoom[a].pos = a;
		newMapRoom[a].prevPos = -1;
	}
	newMapRoom[pos].isVisited = 1;
	newMapRoom[pos].prevPos = pos;
	return (newMapRoom);
}

int		getFirstPos(int TargetPos, int pos, t_room *mapRoom)
{
	int newPos = TargetPos;

	while (mapRoom[newPos].prevPos != pos)
		newPos = mapRoom[newPos].prevPos;
	return (newPos);
}

void	addNeightborsToCheck(t_list **lst, int pos, t_room *mapRoom)
{
	int x = pos % MAP_WIDTH;
	int y = pos / MAP_WIDTH;

	for (int dx = -1; dx <=1; dx++)
	{
		for (int dy = -1; dy <=1; dy++)
		{
			int nx = dx + x;
			int ny = dy + y;
			if (nx < 0 || nx >= MAP_WIDTH || ny < 0 || ny >= MAP_HEIGHT)
				continue;
			int checkPos = (ny * MAP_WIDTH) + nx;
			if (checkPos == pos || mapRoom[checkPos].isVisited == 1)
				continue;
			mapRoom[checkPos].prevPos = pos;
			mapRoom[checkPos].isVisited = 1;
			ft_lstadd_back(lst, ft_lstnew(&mapRoom[checkPos]));
		}
	}
}

void	del(void *content)
{
	(void)content;
}

int reverse(int opp, int pos)
{
	int x = pos % MAP_WIDTH;
	int y = pos / MAP_WIDTH;
	int ox = opp % MAP_WIDTH;
	int oy = opp / MAP_WIDTH;

	if (ox < x && x + 1 < MAP_WIDTH)
		x += 1;
	else if (ox > x && x - 1 >= 0)
		x -= 1;
	if (oy < y && y + 1 < MAP_HEIGHT)
		y += 1;
	else if (oy > y && y - 1 >= 0)
		y -= 1;
	return (x + (y * MAP_WIDTH));
}

int		isHunting(int *map, int pos, int teamId)
{
	int x = pos % MAP_WIDTH;
	int y = pos / MAP_WIDTH;

	for (int dx = x - 10; dx <= x + 10; dx++)
	{
		for (int dy = y - 10; dy <= y + 10; dy++)
		{
			if (dx < 0 || dy < 0 || dx > MAP_WIDTH || dy > MAP_HEIGHT || (dx == x && dy == y))
				continue;
			if (map[dx + (MAP_WIDTH * dy)] == teamId)
				return (1);
		}
	}
	return (0);
}

int		myBfs(int *map, int pos, int teamId)
{
	t_list	*tmp;
	t_list	*to_check = NULL;
	t_list	*tmp_toCheck = NULL;
	t_room 	*mapRoom = GetNewMapRoom(pos);
	int		hunting = isHunting(map, pos, teamId);

	addNeightborsToCheck(&to_check, pos, mapRoom);
	while (ft_lstsize(to_check))
	{
		tmp = to_check;
		while (tmp)
		{
			t_room *room = (t_room *)tmp->content;
			if (map[room->pos] != 0)
			{
				int newPos = getFirstPos(room->pos, pos, mapRoom);
				if (hunting == 0 && teamId != map[room->pos] && isInContact(pos, room->pos, 4))
				{
					newPos = reverse(newPos, pos);
					ft_lstclear(&to_check, del);
					ft_lstclear(&tmp_toCheck, del);
					free(mapRoom);
					while (newPos == pos || map[newPos] != 0)
						newPos = moveRandom(map, pos);
					return (newPos);
				}
				if ((hunting == 0 && teamId == map[room->pos]) || (teamId != map[room->pos] && hunting == 1))
				{
					newPos = getFirstPos(room->pos, pos, mapRoom);
					ft_lstclear(&to_check, del);
					ft_lstclear(&tmp_toCheck, del);
					free(mapRoom);
					while (newPos == pos || map[newPos] != 0)
						newPos = moveRandom(map, pos);
					return (newPos);
				}
			}
			else
				addNeightborsToCheck(&tmp_toCheck, room->pos, mapRoom);
			tmp = tmp->next;
		}
		ft_lstclear(&to_check, del);
		to_check = tmp_toCheck;
		tmp_toCheck = NULL;
	}
	ft_lstclear(&to_check, del);
	free(mapRoom);
	return (moveRandom(map, pos));
}

void	move(t_lem_ipc *all)
{
	int mapSnap[MAP_SIZE + 1];
	int newPos;

	sem_wait(all->semaphore);
	for (int a = 0; a < MAP_SIZE; a++)
		mapSnap[a] = all->map[a];
	sem_post(all->semaphore);
	newPos = myBfs(mapSnap, all->pos, all->teamId);
	sem_wait(all->semaphore);
	if (newPos != -1 && all->map[newPos] == 0)
	{
		all->map[all->pos] = 0;
		all->map[newPos] = all->teamId;
		all->pos = newPos;
	}
	sem_post(all->semaphore);
}

void	error_help()
{
	ft_putstr_fd("Error with arguments\n", 1);
	ft_putstr_fd("Usage:\n", 1);
	ft_putstr_fd("./lem-ipc team_ID\n", 1);
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
	return (1);
}

int checkGameStatus(int *map, sem_t *sem)
{
	int status;
	sem_wait(sem);
	status = map[MAP_SIZE];
	sem_post(sem);
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

void	game(t_lem_ipc *all)
{
	all->pos = getPos(all->map, all->semaphore);
	sem_wait(all->semaphore);
	all->map[all->pos] = all->teamId;
	sem_post(all->semaphore);
	while (!checkGameStatus(all->map, all->semaphore))
		usleep(100000);
	while (!is_dead(all) && !is_last_team(all))
	{
		move(all);
		usleep(20000);
	}
	if (is_last(all->map, all->semaphore))
		last_exit(all);
	sem_wait(all->semaphore);
	all->map[all->pos] = 0;
	sem_post(all->semaphore);
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
	all.mapId = shmget(all.mapKey, (MAP_SIZE + 1) * sizeof(int), 0666);
	if (all.semId == -1 || all.mapId == -1)
		shm_init(&all);
	else
		getShm(&all);
	game(&all);
	return (0);
}
