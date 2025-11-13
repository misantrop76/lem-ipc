#include "../includes/lem-ipc.h"

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
void 	addNeightborsTiny(t_list **lst, int pos, t_room *mapRoom)
{
	int x;
	int y;

	for (int a = 0; a < 4; a++)
	{			
		x = pos % MAP_WIDTH;
		y = pos / MAP_WIDTH;
		if (a == 0)
			x--;
		else if (a == 1)
			x++;
		else if (a == 2)
			y--;
		else
			y++;
		if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
			continue;
		int checkPos = (y * MAP_WIDTH) + x;
		if (mapRoom[checkPos].isVisited == 1)
			continue;
		mapRoom[checkPos].prevPos = pos;
		mapRoom[checkPos].isVisited = 1;
		ft_lstadd_back(lst, ft_lstnew(&mapRoom[checkPos]));
	}

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

	for (int dx = x - 8; dx <= x + 8; dx++)
	{
		for (int dy = y - 8; dy <= y + 8; dy++)
		{
			if (dx < 0 || dy < 0 || dx > MAP_WIDTH || dy > MAP_HEIGHT || (dx == x && dy == y))
				continue;
			if (map[dx + (MAP_WIDTH * dy)] == teamId)
				return (1);
		}
	}
	return (0);
}

int		myBfs(int *map, int pos, int teamId, int random)
{
	t_list	*tmp;
	t_list	*to_check = NULL;
	t_list	*tmp_toCheck = NULL;
	t_room 	*mapRoom = GetNewMapRoom(pos);
	int		hunting = isHunting(map, pos, teamId);

	if (random)
		addNeightborsToCheck(&to_check, pos, mapRoom);
	else
		addNeightborsTiny(&to_check, pos, mapRoom);
	while (ft_lstsize(to_check))
	{
		tmp = to_check;
		while (tmp)
		{
			t_room *room = (t_room *)tmp->content;
			if (map[room->pos] != 0)
			{
				if ((hunting == 1 && map[room->pos] != teamId) 
					|| (hunting == 0 && map[room->pos] == teamId)
					|| (hunting == 0 && isInContact(pos, room->pos, 3)))
				{
					int newPos = getFirstPos(room->pos, pos, mapRoom);
					if (hunting == 0 && teamId != map[room->pos])
						newPos = reverse(newPos, pos);
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

int		isMovePossible(int *map, int pos)
{
	int x = pos % MAP_WIDTH;
	int y = pos / MAP_WIDTH;

	for (int dx = x - 1; dx <= x + 1; dx++)
	{
		for (int dy = y - 1; dy <= y + 1; dy++)
		{
			if (dx > 0 && dy > 0 && dx < MAP_WIDTH && dy < MAP_HEIGHT 
				&& map[dx + (MAP_WIDTH * dy)] == 0)
				return (1);
		}
	}
	return (0);
}

void	move(t_lem_ipc *all, int move_pattern)
{
	int mapSnap[MAP_SIZE];
	int newPos;

	sem_wait(all->semaphore);
	for (int a = 0; a < MAP_SIZE; a++)
		mapSnap[a] = all->map[a];
	sem_post(all->semaphore);
	if (!isMovePossible(mapSnap, all->pos))
		return;
	newPos = myBfs(mapSnap, all->pos, all->teamId, move_pattern ? rand() % 2: 0);
	sem_wait(all->semaphore);
	if (newPos != -1 && all->map[newPos] == 0)
	{
		all->map[all->pos] = 0;
		all->map[newPos] = all->teamId;
		all->pos = newPos;
	}
	sem_post(all->semaphore);
}

