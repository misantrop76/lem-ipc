#ifndef LEM_IPC_H
#define LEM_IPC_H

#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <semaphore.h>

# define MAP_WIDTH 20
# define MAP_HEIGHT 20
# define MAP_SIZE MAP_HEIGHT * MAP_WIDTH
# define SHM_KEY_PATH_MAP "/tmp"
# define SHM_KEY_PATH_SEM "/tmp"
# define SHM_KEY_ID 65

typedef struct	s_lem_ipc
{

	sem_t		*semaphore;
	int			semId;
	key_t		semKey;
	int			*map;
	int			mapId;
	key_t		mapKey;
	int			is_creator;
}				t_lem_ipc;


#endif
