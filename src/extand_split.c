/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extand_split.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykhayri <ykhayri@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/19 16:23:08 by ykhayri           #+#    #+#             */
/*   Updated: 2023/08/19 16:23:19 by ykhayri          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	does_redirection_exist(char *str)
{
	int	i;
	int	count;

	i = -1;
	count = 0;
	while (str[++i])
	{
		if (str[i] == '<' && str[i + 1] && str[i + 1] == '<')
		{
			count++;
			i++;
		}
		else if (str[i] == '>' && str[i + 1] && str[i + 1] == '>')
		{
			count++;
			i++;
		}
		else if ((str[i] == '<' && !str[i + 1]) || (str[i] == '<' && str[i + 1]
				&& str[i + 1] != '<'))
			count++;
		else if ((str[i] == '>' && !str[i + 1]) || (str[i] == '>' && str[i + 1]
				&& str[i + 1] != '>'))
			count++;
	}
	return (count);
}

char	**extended_splitting(char **phrases)
{
	int		i;
	int		count;
	char	**arr;
	int		j;
	int		k;
	int		x;

	i = -1;
	count = 0;
	while (phrases[++i])
	{
		if (phrases[i][0] != '"' && phrases[i][0] != '\''
			&& !ft_strchr(phrases[i], '='))
		{
			count += split_charset_word_count(phrases[i], "><");
			count += does_redirection_exist(phrases[i]);
		}
		if (ft_strchr(phrases[i], '=') || phrases[i][0] == '"'
			|| phrases[i][0] == '\'')
			count++;
	}
	arr = my_alloc(sizeof(char *) * (count + 1));
	i = 0;
	j = 0;
	k = 0;
	while (phrases[i])
	{
		if (phrases[i][0] == '"' || phrases[i][0] == '\''
			|| ft_strchr(phrases[i], '='))
		{
			arr[j++] = phrases[i];
			i++;
		}
		if (!phrases[i])
			break ;
		if (!does_redirection_exist(phrases[i]))
		{
			arr[j] = phrases[i];
			j++;
		}
		else
		{
			k = 0;
			while (phrases[i][k])
			{
				if (does_redirection_exist(&phrases[i][k]))
				{
					x = k;
					while (phrases[i][x] && phrases[i][x] != '<'
						&& phrases[i][x] != '>')
						x++;
					if (phrases[i][x] && x - k > 0)
					{
						arr[j++] = ft_substr(phrases[i], k, x - k);
						k += x - k;
					}
					if ((phrases[i][k] == '<' && phrases[i][k + 1]
							&& phrases[i][k + 1] == '<')
						|| (phrases[i][k] == '>' && phrases[i][k + 1]
							&& phrases[i][k + 1] == '>'))
					{
						arr[j] = ft_substr(phrases[i], k, 2);
						k++;
						j++;
					}
					else if ((phrases[i][k] == '<' && !phrases[i][k + 1])
						|| (phrases[i][k] == '<' && phrases[i][k + 1]
							&& phrases[i][k + 1] != '<'))
					{
						arr[j] = ft_substr(phrases[i], k, 1);
						j++;
					}
					else if ((phrases[i][k] == '>' && !phrases[i][k + 1])
						|| (phrases[i][k] == '>' && phrases[i][k + 1]
							&& phrases[i][k + 1] != '>'))
					{
						arr[j] = ft_substr(phrases[i], k, 1);
						j++;
					}
				}
				else
				{
					arr[j++] = ft_substr(phrases[i], k, ft_strlen(phrases[i])
						- k);
					k += ft_strlen(phrases[i]) - k;
				}
				k++;
			}
		}
		i++;
	}
	arr[j] = NULL;
	return (arr);
}