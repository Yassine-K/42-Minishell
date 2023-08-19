/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykhayri <ykhayri@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/19 10:11:21 by ykhayri           #+#    #+#             */
/*   Updated: 2023/08/19 10:11:39 by ykhayri          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	ft_strrevcmp(char *s1, char *s2)
{
	int	i;
	int	j;

	j = 0;
	i = ft_strlen(s1) - ft_strlen(s2);
	if (i < 0)
		return (1);
	while (s1[i] && s2[j] && s1[i] == s2[j])
	{
		i++;
		j++;
	}
	return (s1[i] - s2[j]);
}
