#include "../includes/vizu.h"

int	free_all(t_vizu *all)
{
	mlx_destroy_image(all->mlx_ptr, all->img.img_ptr);
	mlx_clear_window(all->mlx_ptr, all->win_ptr);
	mlx_destroy_window(all->mlx_ptr, all->win_ptr);
	mlx_destroy_display(all->mlx_ptr);
	free(all->mlx_ptr);
	exit (0);
	return (0);
}

int	my_mlx_get_color(t_img *data, int x, int y)
{
	char	*dst;

	dst = data->data + (y * data->size_l + x * (data->bpp / 8));
	return (*(unsigned int *)dst);
}

int		GetGameState(t_vizu *all)
{
	int lastTeam = 0;

	for (int x = 0; x < all->winSize; x++)
	{
		for (int y = 0; y < all->winSize; y++)
		{
			int color = my_mlx_get_color(&all->img, x, y);
			if (color)
			{
				if (lastTeam && lastTeam != color)
					return (0);
				lastTeam = color;
			}
		}
	}
	return (lastTeam);
}

int		getShm(t_lem_ipc *all)
{
	all->semId = shmget(all->semKey, sizeof(sem_t), 0666);
	all->mapId = shmget(all->mapKey, (MAP_SIZE + 1) * sizeof(int), 0666);
	all->msgId = msgget(all->msgKey, 0666);
	if (all->mapId == -1 || all->semId == -1 || all->msgId == -1)
		exit (1);
	all->semaphore = (sem_t *) shmat(all->semId, NULL, 0);
	all->map = (int *)shmat(all->mapId, NULL, 0);
	if (all->semaphore == (sem_t *)(-1) || all->map == (int *)(-1))
		exit (1);
	return (0);
}

int	key_press(int key, t_vizu *all)
{
	if (key == 65307)
		free_all(all);
	return (0);
}

void	updateImg(t_vizu *all)
{

	sem_wait(all->lemIpc.semaphore);
	for (int x = 0; x < all->winSize; x++)
	{
		for (int y = 0; y < all->winSize; y++)
		{
			int dx = (int)((float)x * ((float)MAP_WIDTH) / all->winSize);
			int dy = (int)((float)y * ((float)MAP_WIDTH) / all->winSize);

			my_mlx_pixel_put(&all->img, x, y, all->lemIpc.map[(dy * MAP_WIDTH) + dx] * (__INT_MAX__ / 1000000));
		}
	}
	sem_post(all->lemIpc.semaphore);
}

void	winnerImage(t_vizu *all)
{
	int center = all->winSize / 2;
	float maxDist = sqrtf(2) * center;
	srand(time(NULL));

	for (int x = 0; x < all->winSize; x++)
	{
		for (int y = 0; y < all->winSize; y++)
		{
			int dx = x - center;
			int dy = y - center;
			float dist = sqrtf(dx * dx + dy * dy);

			float fade = 1.0f - (dist / maxDist);
			if (fade < 0) fade = 0;
			if (fade > 1) fade = 1;

			unsigned int base = all->winnerColor;
			unsigned char r = ((base >> 16) & 0xFF) * fade;
			unsigned char g = ((base >> 8) & 0xFF) * fade;
			unsigned char b = (base & 0xFF) * fade;

			if (rand() % 200 == 0)
			{
				r = (r + rand() % 100) % 256;
				g = (g + rand() % 100) % 256;
				b = (b + rand() % 100) % 256;
			}

			int finalColor = (r << 16) | (g << 8) | b;
			my_mlx_pixel_put(&all->img, x, y, finalColor);
		}
	}

	int scale = all->winSize / 70;
	draw_big_text(all, all->winSize / 8, all->winSize / 2.5 , "GAME OVER", all->winnerColor, scale);
}

int	vizu_loop(t_vizu *all)
{

	struct timeval time;

	gettimeofday(&time, NULL);
	unsigned long now_ms = (time.tv_sec * 1000) + (time.tv_usec / 1000);
	if (!all->winnerColor && all->gameStatus == 0)
	{
		if (now_ms - all->start_ms > 2000)
		{
			//msgctl(all->lemIpc.msgId, IPC_RMID, NULL);
			sem_wait(all->lemIpc.semaphore);
			all->lemIpc.map[MAP_SIZE] = 1;
			sem_post(all->lemIpc.semaphore);
		}
	}
	if (!all->winnerColor && (now_ms - all->start_ms) % 60 == 0)
	{
		updateImg(all);
		if (GetGameState(all))
		{
			all->winnerColor = GetGameState(all);
			winnerImage(all);
			shmdt(all->lemIpc.map);
			shmdt(all->lemIpc.semaphore);
		}
		mlx_put_image_to_window(all->mlx_ptr, all->win_ptr, all->img.img_ptr, 0, 0);
	}
	return (0);
}

int	mouse_click(int button, int x, int y, t_vizu *all)
{	
	(void)x;
	(void)y;
	if (!all->winnerColor && button == 1)
	{
		t_msg message;
		message.command = 1;
		message.ctype = 1;
		all->gameStatus = 1;
		if (msgsnd(all->lemIpc.msgId, &message, sizeof(message.command), 0) == -1)
			exit (1);
		// sem_wait(all->lemIpc.semaphore);
		// if (all->lemIpc.map[MAP_SIZE])
		// 	all->lemIpc.map[MAP_SIZE] = 0;
		// else
		// 	all->lemIpc.map[MAP_SIZE] = 1;
		// sem_post(all->lemIpc.semaphore);
	}
	return (0);
}

int main()
{
	t_vizu vizu;
	struct timeval time;

	vizu.lemIpc.mapKey = ftok(SHM_KEY_PATH, SHM_KEY_ID);
	vizu.lemIpc.semKey = ftok(SHM_KEY_PATH, SHM_KEY_ID + 1);
	vizu.lemIpc.msgKey = ftok(SHM_KEY_PATH, SHM_KEY_ID + 2);
	getShm(&vizu.lemIpc);
	t_msg message;
	message.command = 0;
	message.ctype = 1;
	if (msgsnd(vizu.lemIpc.msgId, &message, sizeof(message.command), 0) == -1)
	{
		exit (1);
	}
	vizu.gameStatus = 0;
	vizu.winnerColor = 0;
	gettimeofday(&time, NULL);
	vizu.start_ms = (time.tv_sec * 1000) + (time.tv_usec / 1000);
	int x;
	vizu.mlx_ptr = mlx_init();
	mlx_get_screen_size(vizu.mlx_ptr, &x, &vizu.winSize);
	vizu.winSize -= 100;
	vizu.win_ptr = mlx_new_window(vizu.mlx_ptr, vizu.winSize, vizu.winSize, "LEM-IPC VISUALIZER");
	vizu.img.img_ptr = NULL;
	vizu.img.img_ptr = mlx_new_image(vizu.mlx_ptr, vizu.winSize, vizu.winSize);
	vizu.img.data = mlx_get_data_addr(vizu.img.img_ptr, &vizu.img.bpp, &vizu.img.size_l, &vizu.img.endian);
	mlx_hook(vizu.win_ptr, 17, 0, free_all, &vizu);
	mlx_hook(vizu.win_ptr, 2, (1L << 0), key_press, &vizu);
	mlx_hook(vizu.win_ptr, 4, (1L << 2), mouse_click, &vizu);
	mlx_loop_hook(vizu.mlx_ptr, vizu_loop, &vizu);
	mlx_loop(vizu.mlx_ptr);
}
