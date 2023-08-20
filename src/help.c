/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   help.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykhayri <ykhayri@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/06 19:52:24 by ykhayri           #+#    #+#             */
/*   Updated: 2023/08/20 12:15:52 by ykhayri          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	checker(char **commands, int i)
{
	int		j;
	char	*str;

	j = i;
	if (!ft_strncmp(commands[i], ">", -1))
		str = ft_strdup(">>");
	if (!ft_strncmp(commands[i], ">>", -1))
		str = ft_strdup(">");
	if (!ft_strncmp(commands[i], "<", -1))
		str = ft_strdup("<<");
	if (!ft_strncmp(commands[i], "<<", -1))
		str = ft_strdup("<");
	while (commands[++j])
	{
		if (!ft_strncmp(commands[j], commands[i], -1)
			|| !ft_strncmp(commands[j], str, -1))
			return (0);
	}
	return (1);
}

int	rederiction_error2(char **commands, int i)
{
	if (commands[i + 1] && (!ft_strncmp(commands[i], ">", -1)
			|| !ft_strncmp(commands[i], "<", -1) || !ft_strncmp(commands[i],
				">>", -1) || !ft_strncmp(commands[i], "<<", -1)) && (
			!ft_strncmp(commands[i + 1], ">", -1)
			|| !ft_strncmp(commands[i + 1], "<", -1)
			|| !ft_strncmp(commands[i + 1], ">>", -1)
			|| !ft_strncmp(commands[i + 1], "<<", -1)))
	{
		ft_dprintf(2, "minishell: syntax error near unexpected token `%s'\n",
			commands[i + 1]);
		g_vars->ex_status = 2;
		return (0);
	}
	return (1);
}

int	rederiction_error(char **commands, int i)
{
	if (ft_strnstr(commands[i], "<<<", -1) || ft_strnstr(commands[i], ">>>", -1)
		|| ft_strnstr(commands[i], "=>", -1) || (!ft_strncmp(commands[i], ">",
				-1) && commands[i + 1] && !ft_strncmp(commands[i + 1], "<", -1))
		|| (!ft_strncmp(commands[i], "<", -1) && commands[i + 1] && !ft_strncmp(
				commands[i + 1], ">", -1)))
	{
		ft_dprintf(2, "minishell: syntax error\n");
		g_vars->ex_status = 2;
		return (0);
	}
	if ((!ft_strncmp(commands[i], ">", -1) || !ft_strncmp(commands[i],
				"<", -1) || !ft_strncmp(commands[i], ">>", -1)
			|| !ft_strncmp(commands[i], "<<", -1)) && !commands[i + 1])
	{
		ft_dprintf(2,
			"minishell: syntax error near unexpected token `newline'\n");
		g_vars->ex_status = 2;
		return (0);
	}
	if (!rederiction_error2(commands, i))
		return (0);
	return (1);
}

static int	par_coount(char *s)
{
	int	i;
	int	active;
	int	par_c;

	i = 0;
	active = 0;
	par_c = 0;
	while (s[i])
	{
		if (s[i] == '(')
			active++;
		if (s[i] == ')')
		{
			active--;
			if (!active)
				par_c++;
		}
		i++;
	}
	if (active)
		return (-1);
	return (par_c);
}

int	nested_par(char **arr, int check, int index)
{
	int		i;
	i = -1;
	int status;
	while (arr[++i])
	{
		arr[i] = ft_strtrim(arr[i], " \t");
		if (has_char(arr[i], '(') || has_char(arr[i], ')'))
		{
			int parentheses_c = par_coount(arr[i]);
			if (!check && parentheses_c == -1)
			{
				ft_dprintf(2, "minishell: syntax error\n");
				g_vars->ex_status = 2;
				return (0);
			}
			char *save = ft_strdup (arr[i]);
			if (parentheses_c == 1 && arr[i][0] == '(' && arr[i][ft_strlen(arr[i]) - 1] == ')')
			{
				if (check && arr[i -1] && (arr[i -1][ft_strlen(arr[i - 1]) - 1] == '&' || (arr[i - 1][ft_strlen(arr[i - 1]) - 1] == '|' && arr[i - 1][ft_strlen(arr[i - 1]) - 2] && arr[i - 1][ft_strlen(arr[i - 1]) - 2] == '|')))
				{
					if (g_vars->ex_status != 0 && g_vars->op[ft_strlen(g_vars->op) -1] == '&')
					{
						while (arr[i] && ((par_coount(arr[i]) == 1 && arr[i][0] == '(' && arr[i][ft_strlen(arr[i]) - 1] == ')' )|| !ft_strnstr(arr[i], "||", -1)))
						{
							if (arr[++i])
								arr[i] = ft_strtrim(arr[i], " \t");
						}
						if (arr[i])
						{
							if (ft_strlen(arr[i]) == 2)
								i++;
							if (!par_coount(arr[i]) && arr[i][0] != '(' && arr[i][ft_strlen(arr[i]) - 1] != ')')
								return nested_par(&arr[i], check, index +1);
						}
					}
					if (g_vars->ex_status == 0 && g_vars->op[ft_strlen(g_vars->op) -1] == '|')
					{

						while (arr[i] && ((par_coount(arr[i]) == 1 && arr[i][0] == '(' && arr[i][ft_strlen(arr[i]) - 1] == ')' )|| !ft_strnstr(arr[i], "&&", -1)))
						{
							if (arr[++i])
								arr[i] = ft_strtrim(arr[i], " \t");
						}
						if (arr[i])
						{
							if (ft_strlen(arr[i]) == 2)
								i++;
							if (!par_coount(arr[i]) && arr[i][0] != '(' && arr[i][ft_strlen(arr[i]) - 1] != ')')
								return nested_par(&arr[i], check, index +1);
						}
					}
				}
				if (!arr[i])
					return (0);
				arr[i] = ft_substr(arr[i], 1, ft_strlen(arr[i]) - 2);
				if (!arr[i][0])
				{
					ft_dprintf(2, "minishell: syntax error\n");
					g_vars->ex_status = 2;
					return (0);
				}
			}
			char **kobi = split_par(arr[i]);
			int x = -1;
			while (!check && kobi[++x])
			{
				kobi[x] = ft_strtrim(kobi[x], " \t");
				char **op_err = ft_split_charset(kobi[x], " \t");
				int y = -1;
				while (op_err[++y])
				{
					int w = -1;
					int op_c = 0;
					while (op_err[y][++w])
					{
						if (op_err[y][w] == '|' || op_err[y][w] == '&')
						{
							op_c++;
						}
						if (op_c > 2)
						{
							ft_dprintf(2, "minishell: syntax error\n");
							g_vars->ex_status = 2;	
							return (0);
						}
					}
					if (op_err[y + 1] && (op_err[y][ft_strlen(op_err[y]) - 1] == '&' || op_err[y][ft_strlen(op_err[y]) - 1] == '|')
					&& (op_err[y + 1][0] == '&' || op_err[y +1][0] == '|'))
					{
						ft_dprintf(2, "minishell: syntax error\n");
						g_vars->ex_status = 2;
						return (0);
					}
				}
				if (kobi[x][0] == '&' || kobi[x][0] == '|')
				{
					if (!x || (x - 1 >= 0 && par_coount(kobi[x -1]) == 0))
					{
						ft_dprintf(2, "minishell: syntax error\n");
						g_vars->ex_status = 2;
						return (0);
					}
				}
				if (kobi[x][ft_strlen(kobi[x]) - 1] == '&' || kobi[x][ft_strlen(kobi[x]) - 1] == '|')
				{
					if (!kobi[x + 1] || (kobi[x + 1] && par_coount(kobi[x + 1]) == 0))
					{
						ft_dprintf(2, "minishell: syntax error\n");
						g_vars->ex_status = 2;
						return (0);
					}
				}
				if (par_coount(kobi[x]) == 1 && kobi[x][0] == '(' && kobi[x][ft_strlen(kobi[x]) - 1] == ')')
				{
					if (kobi[x + 1])
					{
						kobi[x + 1] = ft_strtrim(kobi[x+1], " \t");
						if (kobi[x + 1][0] != '&' && kobi[x + 1][0] != '|')
						{
							ft_dprintf(2, "minishell: syntax error\n");
							g_vars->ex_status = 2;
							return (0);
						}
					}
					if (x > 0)
					{
						kobi[x -1] = ft_strtrim(kobi[x - 1], " \t");
						if (kobi[x -1][ft_strlen(kobi[x - 1]) - 1] != '&' && kobi[x -1][ft_strlen(kobi[x - 1]) - 1] != '|')
						{
							ft_dprintf(2, "minishell: syntax error\n");
							g_vars->ex_status = 2;
							return (0);
						}
					}
				}
			}
			if (check && ( index > 0 || (index == 0 && save[0] == '(' && save[ft_strlen(save) - 1] == ')')))
			{
				if (arr[i+1] && arr[i+1][0] && arr[i+1][0] == '|' && arr[i+1][1] && arr[i+1][1] != '|')
					pipe(g_vars->next_pipefd);
				if (g_vars->prev_pipefd[1] != 0)
					close(g_vars->prev_pipefd[1]);
				int pid = fork();
				if (pid == 0)
				{
					if (arr[i+1] && arr[i+1][0] && arr[i+1][0] == '|' && arr[i+1][1] && arr[i+1][1] != '|')
					{
						dup2(g_vars->next_pipefd[1], 1);
						close(g_vars->next_pipefd[1]);
						close(g_vars->next_pipefd[0]);
					}
					if (i>0 && arr[i-1] && arr[i-1][ft_strlen(arr[i-1]) -1] && arr[i-1][ft_strlen(arr[i-1]) -1] == '|' && (!arr[i-1][ft_strlen(arr[i-1]) -2] || (arr[i-1][ft_strlen(arr[i-1]) -2] && arr[i-1][ft_strlen(arr[i-1]) -2] != '|')))
					{
						dup2(g_vars->prev_pipefd[0], 0);
						close(g_vars->prev_pipefd[1]);
						close(g_vars->prev_pipefd[0]);
					}
					nested_par(kobi, check, index + 1);
					exit(g_vars->ex_status);
				}
				else
				{
					if (waitpid(pid, &status, 0) != -1)
						(g_vars->ex_status) = WEXITSTATUS(status);
					if (g_vars->prev_pipefd[0] != 0)
						close(g_vars->prev_pipefd[0]);
				}
			}
			else
			{
				return nested_par(kobi, check, index + 1);
			}
		}
		else
		{
			if (check && i-1>=0 && arr[i][0] && arr[i][0] == '|' && (!arr[i][1] || (arr[i][1] && arr[i][1] != '|')))
			{
				g_vars->pipe=2;
				g_vars->prev_pipefd[0] = g_vars->next_pipefd[0];
				g_vars->prev_pipefd[1] = g_vars->next_pipefd[1];

			}
			if (check && arr[i+1] && par_coount(arr[i+1]) == 1 && arr[i+1][0] == '(' && arr[i + 1][ft_strlen(arr[i + 1]) - 1] == ')'
				&& arr[i][ft_strlen(arr[i]) -1] && arr[i][ft_strlen(arr[i]) -1] == '|' &&(!arr[i][ft_strlen(arr[i]) - 2] || (arr[i][ft_strlen(arr[i]) - 2] && arr[i][ft_strlen(arr[i]) - 2] != '|')))
				g_vars->pipe = 1;
			if ((check && i-1>=0 && arr[i][0] && arr[i][0] == '|' && (!arr[i][1] || (arr[i][1] && arr[i][1] != '|')))
			&& (check && arr[i+1] && par_coount(arr[i+1]) == 1 && arr[i+1][0] == '(' && arr[i + 1][ft_strlen(arr[i + 1]) - 1] == ')'
				&& arr[i][ft_strlen(arr[i]) -1] && arr[i][ft_strlen(arr[i]) -1] == '|' &&(!arr[i][ft_strlen(arr[i]) - 2] || (arr[i][ft_strlen(arr[i]) - 2] && arr[i][ft_strlen(arr[i]) - 2] != '|')))
			){
				g_vars->pipe = 3;
				if(ft_strlen(arr[i]) < 2)
					g_vars->pipe = 0;
				g_vars->prev_pipefd[0] = g_vars->next_pipefd[0];
				g_vars->prev_pipefd[1] = g_vars->next_pipefd[1];				
			}
			if (check)
			{
				int j = -1;
				while (g_vars->iter_else_count > 0 && ++j < g_vars->iter_else_count)
				{
					if (waitpid(-1, &status, 0) != -1)
						(g_vars->ex_status) = WEXITSTATUS(status);
				}

				arr[i] = ft_strtrim(arr[i], " \t");

				if (arr[i][0] == '&' || (arr[i][0] == '|' && arr[i][1] && arr[i][1] == '|'))
				{
					int s;
					char op;

					s = -1;
					if (g_vars->ex_status == 0 && arr[i][0] == '|')
					{
						op = '&';
						while (arr[i][++s])
						{
							if (arr[i][s+1] && arr[i][s] == op && arr[i][s+1] == op)
							{
								if (arr[i][s+2])
									s += 2;
								break ;
							}
						}
					}
					else if (g_vars->ex_status != 0 && arr[i][0] == '&')
					{
						op = '|';
						while (arr[i][++s])
						{
							if (arr[i][s+1] && arr[i][s] == op && arr[i][s+1] == op)
							{
								if (arr[i][s+2])
									s += 2;
								break ;
							}
						}
					}
					
					int valid = 0;
					if (s == -1)
					{
						s = 2;
						valid = 1;
					}
					arr[i] = ft_substr(arr[i], s, -1);
					if (!arr[i][0])
					{
						if (valid && arr[i + 1])
							return nested_par(&arr[i + 1], check, index +1);
						else if (!valid)
						{
							while (arr[i] && ft_strtrim(arr[i], " \t")[0] != op)
								i++;
							if (arr[i])
							{
								if (ft_strtrim(arr[i], " \t")[0] == op && ft_strlen(ft_strtrim(arr[i], " \t")) == 2)
									i++;
								return nested_par(split_par(arr[i]), check, index +1);
							}
						}
					}
				}
				if (!arr[i])
					return (0);
			}
			if (!g_vars->op)
				g_vars->op = ft_strdup("");
			if (arr[i + 1] || (!arr[i+1]  && (arr[i][0] == '&' || arr[i][0] == '|') && index > 0))
				g_vars->initial_commands = initial_split( arr[i], 1);
			else
				g_vars->initial_commands = initial_split( arr[i], 0);
			g_vars->iter_else_count++;
			if (!g_vars->initial_commands)
				return (0);
			if (check)
			{
				remove_spaces_in_between();
				if (!parsing_commands( g_vars->initial_commands))
					return (0);
				execution_phase();
				g_vars->command_head = NULL;
			}
		}
	}
	return (1);
}

void	split_char_init(int n[4])
{
	n[i] = -1;
	n[j] = -1;
	n[ac] = 0;
}
