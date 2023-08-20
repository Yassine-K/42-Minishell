/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   help.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykhayri <ykhayri@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/06 19:52:24 by ykhayri           #+#    #+#             */
/*   Updated: 2023/08/20 18:35:11 by ykhayri          ###   ########.fr       */
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

int	skip_opertor(char **arr, int n[SUB], int check, char *s)
{
	while (arr[n[i]] && ((par_coount(arr[n[i]]) == 1 && arr[n[i]][0] == '('
			&& arr[n[i]][ft_strlen(arr[n[i]]) - 1] == ')')
			|| !ft_strnstr(arr[n[i]], s, -1)))
	{
		if (arr[++n[i]])
			arr[n[i]] = ft_strtrim(arr[n[i]], " \t");
	}
	if (arr[n[i]])
	{
		if (ft_strlen(arr[n[i]]) == 2)
			n[i]++;
		if (!par_coount(arr[n[i]]) && arr[n[i]][0] != '('
			&& arr[n[i]][ft_strlen(arr[n[i]]) - 1] != ')')
			return (nested_par(&arr[n[i]], check, n[ind] + 1));
	}
	return (-2);
}

int	subshell_op_help(int n[SUB], int check, char **arr)
{
	if (g_vars->ex_status != 0
		&& g_vars->op[ft_strlen(g_vars->op) - 1] == '&')
	{
		n[nest_return] = skip_opertor(arr, n, check, "||");
		if (n[nest_return] != -2)
			return (n[nest_return]);
	}
	if (g_vars->ex_status == 0
		&& g_vars->op[ft_strlen(g_vars->op) - 1] == '|')
	{
		n[nest_return] = skip_opertor(arr, n, check, "&&");
		if (n[nest_return] != -2)
			return (n[nest_return]);
	}
	return (-2);
}

int	subshell_op(int n[SUB], int check, char **arr)
{
	if (n[par_count] == 1 && arr[n[i]][0] == '('
		&& arr[n[i]][ft_strlen(arr[n[i]]) - 1] == ')')
	{
		if (check && arr[n[i] -1] && (arr[n[i] -1][ft_strlen(
					arr[n[i] - 1]) - 1] == '&' || (arr[n[i] - 1][ft_strlen(
					arr[n[i] - 1]) - 1] == '|' && arr[n[i] - 1][ft_strlen(
				arr[n[i] - 1]) - 2] && arr[n[i] - 1][ft_strlen(
						arr[n[i] - 1]) - 2] == '|')))
		{
			if (subshell_op_help(n, check, arr) != -2)
				return (n[nest_return]);
		}
		if (!arr[n[i]])
			return (0);
		arr[n[i]] = ft_substr(arr[n[i]], 1, ft_strlen(arr[n[i]]) - 2);
		if (!arr[n[i]][0])
		{
			ft_dprintf(2, "minishell: syntax error\n");
			g_vars->ex_status = 2;
			return (0);
		}
	}
	return (-2);
}

int	subshell_errors2_help(char **op_err, int y)
{
	int		w;
	int		op_c;

	w = -1;
	op_c = 0;
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
	return (1);
}

int	subshell_errors4(char **kobi, int x)
{
	if (kobi[x + 1])
	{
		kobi[x + 1] = ft_strtrim(kobi[x + 1], " \t");
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
		if (kobi[x -1][ft_strlen(kobi[x - 1]) - 1] != '&'
			&& kobi[x -1][ft_strlen(kobi[x - 1]) - 1] != '|')
		{
			ft_dprintf(2, "minishell: syntax error\n");
			g_vars->ex_status = 2;
			return (0);
		}
	}
	return (1);
}

int	subshell_errors3(char **kobi, int x)
{
	if (kobi[x][0] == '&' || kobi[x][0] == '|')
	{
		if (!x || (x - 1 >= 0 && par_coount(kobi[x -1]) == 0))
		{
			ft_dprintf(2, "minishell: syntax error\n");
			g_vars->ex_status = 2;
			return (0);
		}
	}
	if (kobi[x][ft_strlen(kobi[x]) - 1] == '&'
		|| kobi[x][ft_strlen(kobi[x]) - 1] == '|')
	{
		if (!kobi[x + 1] || (kobi[x + 1] && par_coount(kobi[x + 1]) == 0))
		{
			ft_dprintf(2, "minishell: syntax error\n");
			g_vars->ex_status = 2;
			return (0);
		}
	}
	return (1);
}

int	subshell_errors2(char **kobi, int x)
{
	char	**op_err;
	int		y;

	y = -1;
	op_err = ft_split_charset(kobi[x], " \t");
	while (op_err[++y])
	{
		if (!subshell_errors2_help(op_err, y))
			return (0);
		if (op_err[y + 1] && (op_err[y][ft_strlen(op_err[y]) - 1] == '&'
			|| op_err[y][ft_strlen(op_err[y]) - 1] == '|')
		&& (op_err[y + 1][0] == '&' || op_err[y +1][0] == '|'))
		{
			ft_dprintf(2, "minishell: syntax error\n");
			g_vars->ex_status = 2;
			return (0);
		}
	}
	return (1);
}

int	subshell_errors(char **kobi, int check)
{
	int	x;

	x = -1;
	while (!check && kobi[++x])
	{
		kobi[x] = ft_strtrim(kobi[x], " \t");
		if (!subshell_errors2(kobi, x))
			return (0);
		if (!subshell_errors3(kobi, x))
			return (0);
		if (par_coount(kobi[x]) == 1 && kobi[x][0] == '('
			&& kobi[x][ft_strlen(kobi[x]) - 1] == ')')
		{
			if (!subshell_errors4(kobi, x))
				return (0);
		}
	}
	return (-2);
}

void	start_pipe(int n[SUB], char **arr)
{
	if (arr[n[i] + 1] && arr[n[i] + 1][0] && arr[n[i] + 1][0] == '|'
		&& arr[n[i] + 1][1] && arr[n[i] + 1][1] != '|')
		pipe(g_vars->next_pipefd);
	if (g_vars->prev_pipefd[1] != 0)
		close(g_vars->prev_pipefd[1]);
}

void	sub_piping(int n[SUB], char **arr)
{
	if (arr[n[i] + 1] && arr[n[i] + 1][0] && arr[n[i] + 1][0] == '|'
		&& arr[n[i] + 1][1] && arr[n[i] + 1][1] != '|')
	{
		dup2(g_vars->next_pipefd[1], 1);
		close(g_vars->next_pipefd[1]);
		close(g_vars->next_pipefd[0]);
	}
	if (n[i] > 0 && arr[n[i] - 1] && arr[n[i] - 1][ft_strlen(arr[n[i] - 1]) - 1]
		&& arr[n[i] - 1][ft_strlen(arr[n[i] - 1]) - 1] == '|'
		&& (!arr[n[i] - 1][ft_strlen(arr[n[i] - 1]) - 2]
		|| (arr[n[i] - 1][ft_strlen(arr[n[i] - 1]) - 2]
		&& arr[n[i] - 1][ft_strlen(arr[n[i] - 1]) - 2] != '|')))
	{
		dup2(g_vars->prev_pipefd[0], 0);
		close(g_vars->prev_pipefd[1]);
		close(g_vars->prev_pipefd[0]);
	}
}

void	sub_pipe(int n[SUB], char **arr, char **kobi, int check)
{
	int	pid;

	start_pipe(n, arr);
	pid = fork();
	if (pid == 0)
	{
		sub_piping(n, arr);
		nested_par(kobi, check, n[ind] + 1);
		exit(g_vars->ex_status);
	}
	else
	{
		if (waitpid(pid, &n[stat], 0) != -1)
			(g_vars->ex_status) = n[stat] >> 8;
		if (g_vars->prev_pipefd[0] != 0)
			close(g_vars->prev_pipefd[0]);
	}
}

void	not_subshell_pieps2(int n[SUB], char **arr, int check)
{
	if ((check && n[i] - 1 >= 0 && arr[n[i]][0] && arr[n[i]][0] == '|'
		&& (!arr[n[i]][1] || (arr[n[i]][1] && arr[n[i]][1] != '|')))
		&& (check && arr[n[i] + 1] && par_coount(arr[n[i] + 1]) == 1
		&& arr[n[i] + 1][0] == '(' && arr[n[i] + 1][ft_strlen(
			arr[n[i] + 1]) - 1] == ')' && arr[n[i]][ft_strlen(arr[n[i]]) - 1]
			&& arr[n[i]][ft_strlen(arr[n[i]]) - 1] == '|'
			&& (!arr[n[i]][ft_strlen(arr[n[i]]) - 2]
			|| (arr[n[i]][ft_strlen(arr[n[i]]) - 2]
			&& arr[n[i]][ft_strlen(arr[n[i]]) - 2] != '|')))
	)
	{
		g_vars->pipe = 3;
		if (ft_strlen(arr[n[i]]) < 2)
			g_vars->pipe = 0;
		g_vars->prev_pipefd[0] = g_vars->next_pipefd[0];
		g_vars->prev_pipefd[1] = g_vars->next_pipefd[1];
	}
}

void	not_subshell_pieps(int n[SUB], char **arr, int check)
{
	if (check && n[i] - 1 >= 0 && arr[n[i]][0] && arr[n[i]][0] == '|'
		&& (!arr[n[i]][1] || (arr[n[i]][1] && arr[n[i]][1] != '|')))
	{
		g_vars->pipe = 2;
		g_vars->prev_pipefd[0] = g_vars->next_pipefd[0];
		g_vars->prev_pipefd[1] = g_vars->next_pipefd[1];
	}
	if (check && arr[n[i] + 1] && par_coount(arr[n[i] + 1]) == 1
		&& arr[n[i] + 1][0] == '(' && arr[n[i] + 1][ft_strlen(
			arr[n[i] + 1]) - 1] == ')' && arr[n[i]][ft_strlen(arr[n[i]]) - 1]
			&& arr[n[i]][ft_strlen(arr[n[i]]) - 1] == '|'
			&& (!arr[n[i]][ft_strlen(arr[n[i]]) - 2]
			|| (arr[n[i]][ft_strlen(arr[n[i]]) - 2] && arr[n[i]][ft_strlen(
			arr[n[i]]) - 2] != '|')))
		g_vars->pipe = 1;
	not_subshell_pieps2(n, arr, check);
}

void	wait_pid(int n[SUB], char *op)
{
	int	j;

	j = -1;
	*op = 0;
	while (g_vars->iter_else_count > 0 && ++j < g_vars->iter_else_count)
	{
		if (waitpid(-1, &n[stat], 0) != -1)
			(g_vars->ex_status) = n[stat] >> 8;
	}
}

void	skip_sub_array(char op, int n[SUB], char **arr)
{
	while (arr[n[i]][++n[s]])
	{
		if (arr[n[i]][n[s] + 1] && arr[n[i]][n[s]] == op
			&& arr[n[i]][n[s] + 1] == op)
		{
			if (arr[n[i]][n[s] + 2])
				n[s] += 2;
			break ;
		}
	}
}

int	not_subshell_skip_help(char **arr, int check, int n[SUB], char op)
{
	if (n[s] == -1)
	{
		n[s] = 2;
		n[success] = 1;
	}
	arr[n[i]] = ft_substr(arr[n[i]], n[s], -1);
	if (!arr[n[i]][0])
	{
		if (n[success] && arr[n[i] + 1])
			return (nested_par(&arr[n[i] + 1], check, n[ind] +1));
		else if (!n[success])
		{
			while (arr[n[i]] && ft_strtrim(arr[n[i]], " \t")[0] != op)
				n[i]++;
			if (arr[n[i]])
			{
				if (ft_strtrim(arr[n[i]], " \t")[0] == op
					&& ft_strlen(ft_strtrim(arr[n[i]], " \t")) == 2)
					n[i]++;
				return (nested_par(split_par(arr[n[i]]), check, n[ind] + 1));
			}
		}
	}
	return (-2);
}

int	not_subshell_skip(int n[SUB], char **arr, int check)
{
	char	op;
	int		return_val;

	wait_pid(n, &op);
	arr[n[i]] = ft_strtrim(arr[n[i]], " \t");
	if (arr[n[i]][0] == '&' || (arr[n[i]][0] == '|'
		&& arr[n[i]][1] && arr[n[i]][1] == '|'))
	{
		n[s] = -1;
		if (g_vars->ex_status == 0 && arr[n[i]][0] == '|')
		{
			op = '&';
			skip_sub_array(op, n, arr);
		}
		else if (g_vars->ex_status != 0 && arr[n[i]][0] == '&')
		{
			op = '|';
			skip_sub_array(op, n, arr);
		}
		n[success] = 0;
		return_val = not_subshell_skip_help(arr, check, n, op);
		if (return_val != -2)
			return (return_val);
	}
	return (-2);
}

int	nested_par(char **arr, int check, int index)
{
	int		n[SUB];
	char	*save;
	char	**kobi;

	n[i] = -1;
	n[ind] = index;
	while (arr[++n[i]])
	{
		arr[n[i]] = ft_strtrim(arr[n[i]], " \t");
		if (has_char(arr[n[i]], '(') || has_char(arr[n[i]], ')'))
		{
			n[par_count] = par_coount(arr[n[i]]);
			if (!check && n[par_count] == -1)
			{
				ft_dprintf(2, "minishell: syntax error\n");
				g_vars->ex_status = 2;
				return (0);
			}
			save = ft_strdup (arr[n[i]]);
			n[subshell_op_ret] = subshell_op(n, check, arr);
			if (n[subshell_op_ret] != -2)
				return (n[subshell_op_ret]);
			kobi = split_par(arr[n[i]]);
			n[subshell_err] = subshell_errors(kobi, check);
			if (n[subshell_err] != -2)
				return (n[subshell_err]);
			if (check && ( n[ind] > 0 || (n[ind] == 0 && save[0] == '(' && save[ft_strlen(save) - 1] == ')')))
				sub_pipe(n, arr, kobi, check);
			else
				return (nested_par(kobi, check, n[ind] + 1));
		}
		else
		{
			not_subshell_pieps(n, arr, check);
			if (check)
			{
				n[skip_op] = not_subshell_skip(n, arr, check);
				if (n[skip_op] != -2)
					return (n[skip_op]);
				if (!arr[n[i]])
					return (0);
			}
			if (!g_vars->op)
				g_vars->op = ft_strdup("");
			if (arr[n[i] + 1] || (!arr[n[i] + 1]  && (arr[n[i]][0] == '&' || arr[n[i]][0] == '|') && n[ind] > 0))
				g_vars->initial_commands = initial_split( arr[n[i]], 1);
			else
				g_vars->initial_commands = initial_split( arr[n[i]], 0);
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
//ls && (ls && gg) && (ls && kk) || (ls | wc) | (cat -e)
