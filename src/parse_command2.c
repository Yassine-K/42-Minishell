/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_command2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykhayri <ykhayri@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/18 16:27:14 by abouabra          #+#    #+#             */
/*   Updated: 2023/08/19 20:52:42 by ykhayri          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void	rm_qts_help(int *num, char **arr, char *q, t_fill_info *info)
{
	char	*new;
	char	**tmp;

	*num = ft_strchr_num(arr[0], q[0]);
	if (arr[0][*num - 1] == '=')
	{
		tmp = ft_split(arr[0], '=');
		new = ft_strtrim(tmp[1], q);
		arr[0] = ft_strjoin(tmp[0], "=");
		arr[0] = ft_strjoin(arr[0], new);
	}
	else
	{
		if (q[0] == '\'')
			info->quote_type = 1;
		else
			info->quote_type = 2;
	}
}

void	fix_string2(t_fill_info *info, char **dest, char **src)
{
	char	c;

	c = *(*src);
	if (c == '\'')
		info->quote_type = 1;
	else if (c == '\"')
		info->quote_type = 2;
	(*src)++;
	while (*(*src) && *(*src) != c)
	{
		*(*dest) = *(*src);
		(*dest)++;
		(*src)++;
	}
	if (*(*src) == c)
		(*src)++;
}

void	fix_string(t_fill_info *info, char *dest, char *src)
{
	if (src[0] && src[1] && !src[2] && src[0] == src[1])
		return ;
	if (ft_strchr(src, '=') || ft_strchr(src, '$'))
		return ;
	while (*src)
	{
		if (*src == '"' || *src == '\'')
			fix_string2(info, &dest, &src);
		else
		{
			*dest = *src;
			dest++;
			src++;
		}
	}
	*dest = '\0';
}

void	remove_quotes_help(int quote[2], char **arr)
{
	int	j;
	int	i;

	quote[sin] = 0;
	quote[doub] = 0;
	i = -1;
	while (arr[++i])
	{
		j = -1;
		while (arr[i][++j])
		{
			if (arr[i][j] == '\'')
				quote[sin]++;
			else if (arr[i][j] == '\"')
				quote[doub]++;
		}
	}
}

int	remove_quotes(t_fill_info *info, char **arr)
{
	int	i;
	int	num_1;
	int	num_2;
	int	quote[2];

	remove_quotes_help(quote, arr);
	if (quote[sin] % 2 != 0 || quote[doub] % 2 != 0)
	{
		ft_dprintf(2, "minishell: unexpected EOF while looking for matching\n");
		g_vars->ex_status = 2;
		return (0);
	}
	i = -1;
	while (arr[++i])
		fix_string(info, arr[i], arr[i]);
	i = -1;
	while (arr[++i])
	{
		if (arr[i][0] == '\'' && arr[i][1] && arr[i][1] != '\'')
			rm_qts_help(&num_1, &arr[i], "\'", info);
		else if (arr[i][0] == '\"' && arr[i][1] && arr[i][1] != '\"')
			rm_qts_help(&num_2, &arr[i], "\"", info);
	}
	return (1);
}

void	expand_herd_help(char *original_string, char **final, int *i)
{
	char	*data;
	int		j;
	char	*id;

	if (original_string[(*i)] == '?')
	{
		data = ft_itoa(g_vars->ex_status);
		*final = ft_strjoin((*final), data);
	}
	else
	{
		j = -1;
		while (original_string[++j + (*i)])
		{
			if (ft_strchr(" \"'\n", original_string[j + (*i)]))
				break ;
		}
		id = ft_substr(original_string, (*i), j);
		data = get_env_data(id);
		if (!data)
			data = "";
		(*final) = ft_strjoin((*final), data);
		(*i) += j - 1;
	}
}

char	*expand_herd(char *original_string, int should_expand)
{
	int		i;
	char	*final;
	char	tmp[2];

	final = "";
	i = -1;
	while (original_string[++i])
	{
		if (original_string[i] == '$' && should_expand)
		{
			i++;
			expand_herd_help(original_string, &final, &i);
		}
		else
		{
			tmp[0] = original_string[i];
			tmp[1] = '\0';
			final = ft_strjoin(final, tmp);
		}
	}
	return (final);
}

char	*expand_env(t_fill_info *info, char *str)
{
	int		should_expand;
	char	*tt;

	if (!str || !ft_strchr(str, '$'))
		return (str);
	should_expand = 1;
	if (info->quote_type == 1 || info->quote_type == 2)
		should_expand = 0;
	tt = expand_herd(str, should_expand);
	return (tt);
}

void	get_herdoc_data_help(char **total, t_fill_info *info, char *limiter)
{
	char	*str;
	char	*ww;

	while (1)
	{
		if (!g_vars->interrupted_mode && isatty(STDIN_FILENO))
			ft_dprintf(1, "> ");
		str = get_next_line(g_vars->heredocs_fd);
		if (g_vars->interrupted_mode == 3)
		{
			g_vars->ex_status = 1;
			*total = "";
			break ;
		}
		ww = expand_env(info, str);
		if (!ww || (ww && !ft_strncmp(ww, limiter, -1)))
			break ;
		*total = ft_strjoin((*total), ww);
	}
	g_vars->is_running = 0;
}

char	*get_herdoc_data(t_fill_info *info, char *limiter)
{
	char	*total;
	char	*name;
	int		fd;

	limiter = expand_env(info, ft_strtrim(limiter, "\"\'"));
	limiter = ft_strjoin(limiter, "\n");
	total = "";
	g_vars->is_running = 3;
	if (!isatty(0))
		g_vars->heredocs_fd = 0;
	else
		g_vars->heredocs_fd = dup(0);
	tcsetattr(STDIN_FILENO, TCSANOW, &g_vars->new_term);
	get_herdoc_data_help(&total, info, limiter);
	if (isatty(0))
		close(g_vars->heredocs_fd);
	tcsetattr(STDIN_FILENO, TCSANOW, &g_vars->old_term);
	name = "/tmp/herdoc_data";
	unlink(name);
	fd = open(name, O_WRONLY | O_TRUNC | O_CREAT, 0644);
	write(fd, total, ft_strlen(total));
	close(fd);
	return (name);
}

static int	is_a_redirection(char *str)
{
	if (!str)
		return (0);
	if (!ft_strncmp(str, ">", -1) || !ft_strncmp(str, "<", -1)
		|| !ft_strncmp(str, ">>", -1) || !ft_strncmp(str, "<<", -1))
		return (1);
	return (0);
}

void	red_help2(t_fill_info *info, char **commands, int *i, char *file_name)
{
	char		*heredoc_file;
	t_cmd_redir	*redir;

	if (!ft_strncmp(commands[*i], "<", -1))
	{
		redir = ft_new_redir(INPUT, file_name);
		(*i)++;
		add_redir_in_back(&info->redir, redir);
	}
	if (!ft_strncmp(commands[*i], ">>", -1))
	{
		redir = ft_new_redir(APPEND, file_name);
		(*i)++;
		add_redir_in_back(&info->redir, redir);
	}
	if (!ft_strncmp(commands[*i], "<<", -1))
	{
		heredoc_file = get_herdoc_data(info, commands[++(*i)]);
		redir = ft_new_redir(HEREDOC, heredoc_file);
		add_redir_in_back(&info->redir, redir);
	}
}

void	red_help(t_fill_info *info, char **commands, int *i)
{
	char		**arr;
	char		*file_name;
	int			k;
	t_cmd_redir	*redir;

	if (is_a_redirection(commands[*i]) && ft_strchr(commands[*i + 1], '$'))
	{
		arr = my_alloc(sizeof(char *) * 2);
		arr[0] = ft_strdup(commands[*i + 1]);
		arr[1] = NULL;
		arr = expand_variables(info, arr);
		k = -1;
		while (arr[++k])
			fix_string(info, arr[k], arr[k]);
		file_name = ft_strtrim(*arr, " ");
	}
	else
		file_name = commands[*i + 1];
	if (!ft_strncmp(commands[*i], ">", -1))
	{
		redir = ft_new_redir(OUTPUT, file_name);
		(*i)++;
		add_redir_in_back(&info->redir, redir);
	}
	red_help2(info, commands, i, file_name);
}

void	count_redirections_help(char *command, int *i, int *count)
{
	if (command[(*i)] == '<')
	{
		if ((*i) + 1 < len && command[(*i) + 1] != ' ' && command[(*i)
				+ 1] == '<')
		{
			(*i)++;
			(*count)++;
		}
		else if ((*i) + 1 < len && command[(*i) + 1] != ' ' && command[(*i)
				+ 1] != '<')
			(*count)++;
	}
	else if (command[(*i)] == '>')
	{
		if (((*i) + 1 < len) && command[(*i) + 1] != ' ' && command[(*i)
				+ 1] != '>')
		{
			(*i)++;
			(*count)++;
		}
		else if (command[(*i) + 1] != ' ' && command[(*i) + 1] == '>')
			(*count)++;
	}
}

int	count_redirections(char *command)
{
	int	count;
	int	len;
	int	i;
	int	is_parsing_command;

	count = 0;
	len = ft_strlen(command);
	i = 0;
	is_parsing_command = 1;
	while (i < len)
	{
		if (command[i] == '<' || command[i] == '>')
		{
			count_redirections_help(command, &i, &count);
			is_parsing_command = 0;
		}
		else if (command[i] == ' ' && !is_parsing_command)
		{
			is_parsing_command = 1;
		}
		i++;
	}
	if (is_parsing_command && (len > 0 && command[len - 1] != ' '))
		count++;
	return (count);
}
