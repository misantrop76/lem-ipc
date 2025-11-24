#ifndef LEM_IPC_H
#define LEM_IPC_H

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <semaphore.h>
#include "../libft/libft.h"
#include "../mlx/mlx.h"
#include "math.h"
#include <signal.h>


# define MAP_WIDTH 100
# define MAP_HEIGHT MAP_WIDTH
# define MAP_SIZE MAP_HEIGHT * MAP_WIDTH
# define SHM_KEY_PATH "/tmp"
# define SHM_KEY_ID 65

typedef struct s_room
{
	int		pos;
	int		prevPos;
	int		isVisited;
}				t_room;

typedef struct s_msg {
	long ctype;
    int command;      // 0 = pause, 1 = resume, 2 = shutdown
} t_msg;


typedef struct	t_lem_ipc
{

	sem_t		*semaphore;
	int			semId;
	key_t		semKey;
	int			*map;
	int			mapId;
	key_t		mapKey;
	int			msgId;
	key_t		msgKey;
	int			pos;
	int			teamId;
}				t_lem_ipc;

void	move(t_lem_ipc *all, int move_pattern);
int		is_last(int *map, sem_t *sem);

#endif
