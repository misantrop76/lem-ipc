#ifndef LEM_IPC_H
#define LEM_IPC_H

#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <semaphore.h>
#include "../libft/libft.h"
#include "../mlx/mlx.h"
#include "math.h"

# define MAP_WIDTH 100
# define MAP_HEIGHT 100
# define MAP_SIZE MAP_HEIGHT * MAP_WIDTH
# define SHM_KEY_PATH_MAP "/tmp"
# define SHM_KEY_PATH_SEM "/tmp"
# define SHM_KEY_ID 65

typedef struct s_room
{
	int		pos;
	int		prevPos;
	int		isVisited;
}				t_room;

typedef struct	t_lem_ipc
{

	sem_t		*semaphore;
	int			semId;
	key_t		semKey;
	int			*map;
	int			mapId;
	key_t		mapKey;
	int			pos;
	int			teamId;
}				t_lem_ipc;

void	move(t_lem_ipc *all);
int		is_last(int *map);

#endif
