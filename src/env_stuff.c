/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_stuff.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykhayri <ykhayri@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/21 11:33:03 by abouabra          #+#    #+#             */
/*   Updated: 2023/08/19 09:42:37 by ykhayri          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*get_env_data(char *id)
{
	t_env	*env;

	env = g_vars->env_head;
	while (env)
	{
		if (!ft_strncmp(id, env->env_id, -1))
			return (env->env_data);
		env = env->next;
	}
	return (NULL);
}

void	set_env()
{
	t_env	*env;
	char	**arr;
	int		i;

	i = -1;
	while (g_vars->ev[++i])
	{
		arr = split_arg(g_vars->ev[i]);
		env = ft_new_env_node(arr[0], arr[1]);
		add_env_in_back(&g_vars->env_head, env);
	}	
}

int	ft_env_list_size(t_env **head)
{
	t_env	*tmp;
	int		i;

	i = 0;
	tmp = *head;
	while (tmp)
	{
		i++;
		tmp = tmp->next;
	}
	return (i);
}
