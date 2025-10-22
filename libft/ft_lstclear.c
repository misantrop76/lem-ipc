/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mminet <mminet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 13:34:54 by mminet            #+#    #+#             */
/*   Updated: 2025/10/22 21:44:43 by mminet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void*))
{
	t_list	*elm;
	t_list	*next;

	elm = *lst;
	next = NULL;
	while (elm != NULL)
	{
		next = elm->next;
		del(&elm->content);
		free(elm);
		elm = next;
	}
	*lst = NULL;
}
