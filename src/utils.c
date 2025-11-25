#include "../includes/lem-ipc.h"
#include "../includes/vizu.h"

int		is_last(int *map, sem_t *sem)
{
	int check = 0;
	sem_wait(sem);
	for (int a = 0; a < MAP_SIZE; a++)
	{
		if (map[a] != 0)
			check++;
	}
	sem_post(sem);
	return (check);
}

int		isInLst(t_list *lst, int color)
{
	t_list *tmp = lst;

	while (tmp)
	{
		t_team *team = (t_team *)tmp->content;
		if (team->color == color)
			return (1);
		tmp = tmp->next;
	}
	return (0);
}

void	my_mlx_pixel_put(t_img *data, int x, int y, int color)
{
	char	*dst;

	dst = data->data + (y * data->size_l + x * (data->bpp / 8));
	*(unsigned int *)dst = color;
}

void	draw_big_char(t_vizu *all, int x, int y, char c, int color, int scale)
{
	static const char *letters[26] = {
		"0110"
		"1001"
		"1111"
		"1001"
		"1001", // A

		"1110"
		"1001"
		"1110"
		"1001"
		"1110", // B

		"0111"
		"1000"
		"1000"
		"1000"
		"0111", // C

		"1110"
		"1001"
		"1001"
		"1001"
		"1110", // D

		"1111"
		"1000"
		"1110"
		"1000"
		"1111", // E

		"1111"
		"1000"
		"1110"
		"1000"
		"1000", // F

		"0111"
		"1000"
		"1011"
		"1001"
		"0111", // G

		"1001"
		"1001"
		"1111"
		"1001"
		"1001", // H

		"111"
		"010"
		"010"
		"010"
		"111", // I

		"0011"
		"0001"
		"0001"
		"1001"
		"0110", // J

		"1001"
		"1010"
		"1100"
		"1010"
		"1001", // K

		"1000"
		"1000"
		"1000"
		"1000"
		"1111", // L

		"10001"
		"11011"
		"10101"
		"10001"
		"10001", // M

		"1001"
		"1101"
		"1011"
		"1001"
		"1001", // N

		"0110"
		"1001"
		"1001"
		"1001"
		"0110", // O

		"1110"
		"1001"
		"1110"
		"1000"
		"1000", // P

		"0110"
		"1001"
		"1001"
		"1011"
		"0111", // Q

		"1110"
		"1001"
		"1110"
		"1010"
		"1001", // R

		"0111"
		"1000"
		"0110"
		"0001"
		"1110", // S

		"11111"
		"00100"
		"00100"
		"00100"
		"00100", // T

		"1001"
		"1001"
		"1001"
		"1001"
		"0110", // U

		"10001"
		"10001"
		"01010"
		"01010"
		"00100", // V

		"10001"
		"10001"
		"10101"
		"11011"
		"10001", // W

		"1001"
		"0110"
		"0110"
		"1001"
		"1001",

		"1001"
		"0101"
		"0010"
		"0010"
		"0010",

		"1111"
		"0001"
		"0010"
		"0100"
		"1111",
		
	};
	static const char *digits[10] = {
		"0110"
		"1001"
		"1001"
		"1001"
		"0110",   // 0

		"0010"
		"0110"
		"0010"
		"0010"
		"0111",   // 1

		"0110"
		"1001"
		"0010"
		"0100"
		"1111",   // 2

		"1110"
		"0001"
		"0110"
		"0001"
		"1110",   // 3

		"1001"
		"1001"
		"1111"
		"0001"
		"0001",   // 4

		"1111"
		"1000"
		"1110"
		"0001"
		"1110",   // 5

		"0111"
		"1000"
		"1110"
		"1001"
		"0110",   // 6

		"1111"
		"0001"
		"0010"
		"0100"
		"0100",   // 7

		"0110"
		"1001"
		"0110"
		"1001"
		"0110",   // 8

		"0110"
		"1001"
		"0111"
		"0001"
		"1110"    // 9
	};

	// Letters
	if (c >= 'A' && c <= 'Z')
	{
		const char *glyph = letters[c - 'A'];
		int width = strlen(glyph) / 5;

		for (int row = 0; row < 5; row++)
			for (int col = 0; col < width; col++)
				if (glyph[row * width + col] == '1')
					for (int i = 0; i < scale; i++)
						for (int j = 0; j < scale; j++)
							my_mlx_pixel_put(&all->img, x + col * scale + i,
								y + row * scale + j, color);
		return;
	}

	// Digits
	if (c >= '0' && c <= '9')
	{
		const char *glyph = digits[c - '0'];
		int width = strlen(glyph) / 5;

		for (int row = 0; row < 5; row++)
			for (int col = 0; col < width; col++)
				if (glyph[row * width + col] == '1')
					for (int i = 0; i < scale; i++)
						for (int j = 0; j < scale; j++)
							my_mlx_pixel_put(&all->img, x + col * scale + i,
								y + row * scale + j, color);
		return;
	}
}

void	draw_big_text(t_vizu *all, int start_x, int start_y, char *text, int color, int scale)
{
	int x = start_x;
	for (int i = 0; text[i]; i++)
	{
		if (text[i] != ' ')
			draw_big_char(all, x, start_y, text[i], color, scale);
		x += scale * 6;
	}
}
