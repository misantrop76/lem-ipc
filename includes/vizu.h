#ifndef VIZU_H
# define VIZU_H
# include "../mlx/mlx.h"
# include "lem-ipc.h"
# include <sys/time.h>
#include <unistd.h>

///////// IMAGE MLX ///////
typedef struct s_img
{
	void				*img_ptr;
	char				*data;
	int					size_l;
	int					bpp;
	int					endian;
}						t_img;

typedef struct s_vizu
{
    void    *mlx_ptr;
    void   *win_ptr;
	int		winSize;
	t_img	img;
	t_lem_ipc lemIpc;
}				t_vizu;

#endif