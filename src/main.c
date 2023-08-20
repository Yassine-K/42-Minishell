/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykhayri <ykhayri@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/18 12:30:57 by abouabra          #+#    #+#             */
/*   Updated: 2023/08/20 13:57:14 by ykhayri          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include <stdio.h>
#include <sys/_types/_pid_t.h>
#include <sys/fcntl.h>
#include <unistd.h>

// int		g_var[3];
t_args	*g_vars;

void	handle_signals(int signum)
{
	if (signum == SIGINT)
	{
		g_vars->interrupted_mode = 1;
		// if (g_vars->is_running == 1)
		// 	printf("^C");
		ft_dprintf(1, "\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		g_vars->is_interrupted = 1;
		if (g_vars->is_running == 3)
		{
			g_vars->interrupted_mode = 3;
			close(g_vars->heredocs_fd);
		}
		if (g_vars->is_running == 0)
		{
			g_vars->is_interrupted = 0;
			// g_var[is_interrupted] = 0;
			g_vars->ex_status = 1;
			// g_var[ex_status] = 1;
			rl_redisplay();
		}
	}
	if (signum == SIGQUIT)
	{
		g_vars->interrupted_mode = 2;
		// if (!g_var[is_running])
		if (g_vars->is_running)
		{
			if (g_vars->is_running != 3)
				ft_dprintf(1, "Quit: 3\n");
			// g_var[ex_status] = 131;
			g_vars->is_interrupted = 1;
			// g_var[is_interrupted] = 1;
			return ;
		}
		g_vars->is_interrupted = 0;
	}
}

void	execute(t_command **tmp, int *index)
{
	// int		status;
	int i;
	int status;
	
	i = *index;
	if (i > 0 && g_vars->op[(i - 1) * 2] == '2')
	{
		// printf("op: %s   im working on: %s I: %d\n", g_vars->op, (*tmp)->command_args[0],*index);
		int j = -1;
		while (++j < i)
		{
			if (waitpid(g_vars->pid[j], &status, 0) != -1)
			{
				// printf
				(g_vars->ex_status) = status >> 8;
			}
		}
		if (g_vars->ex_status != 0 && g_vars->op[(i - 1) * 2 + 1] == '&')
		{
			while (g_vars->op[(i - 1) * 2] && ((g_vars->op[(i - 1) * 2] == '1' && g_vars->op[(i - 1) * 2 + 1] == '|') || (g_vars->op[(i - 1) * 2] == '2' && g_vars->op[(i - 1) * 2 + 1] != '|')))
			{
				i++;
				(*tmp) = (*tmp)->next;
			}
		}
		if (g_vars->ex_status == 0 && g_vars->op[(i - 1) * 2 + 1] == '|')
		{
			while (g_vars->op[(i - 1) * 2] && ((g_vars->op[(i - 1) * 2] == '1' && g_vars->op[(i - 1) * 2 + 1] == '|') || (g_vars->op[(i - 1) * 2] == '2' && g_vars->op[(i - 1) * 2 + 1] != '&')))
			{
				i++;
				(*tmp) = (*tmp)->next;
			}
		}
		*index = i;
		if (i == g_vars->command_count)
			return ;
		// printf("total: %s || operator: %c || cmd: %s\n",g_vars->op, g_vars->op[(i - 1) * 2 +1], (*tmp)->command_args[0]);
	}

	// a[args] = expand_variables(in, a[args]);
	// check permisions for redir
	t_cmd_redir *redir = (*tmp)->redir;
	while (redir)
	{
		//check if file exists using check_permision function
		if (redir->type == OUTPUT)
			check_permision(NULL, redir->file, 3);
		else if (redir->type == INPUT)
			check_permision(NULL, redir->file, 2);
		else if (redir->type == APPEND)
			check_permision(NULL, redir->file, 3);
		redir = redir->next;
	}

	// check permisions for command
	if ((*tmp)->command_args && (*tmp)->command_args[0])
	{
		if (check_permision((*tmp)->command_path, (*tmp)->command_args[0], 1))
			(*tmp)->is_valid_command = 0;
		else
			(*tmp)->is_valid_command = 1;
	}

	// printf("im working on: %s I: %d\n", (*tmp)->command_args[0],*index);
	// if (i < g_vars->command_count - 1 && g_vars->op[i * 2] == '1')
	// printf("before: prev[0]: %d   prev[1]: %d\n",g_vars->prev_pipefd[0],g_vars->prev_pipefd[1]);
	// printf("before: next[0]: %d   next[1]: %d\n",g_vars->next_pipefd[0],g_vars->next_pipefd[1]);

	if ((g_vars->pipe== 1  || g_vars->pipe == 3)|| (i < g_vars->command_count - 1 && (!g_vars->op[0] || (g_vars->op[0] && g_vars->op[i * 2] == '1'))))
	{
		// g_vars->pipe = 0;
		// printf("gg %s\n", (*tmp)->command_args[0]);

		pipe(g_vars->next_pipefd);
	}

	// printf("pipe after: prev[0]: %d   prev[1]: %d\n",g_vars->prev_pipefd[0],g_vars->prev_pipefd[1]);
	// printf("pipe after: next[0]: %d   next[1]: %d\n",g_vars->next_pipefd[0],g_vars->next_pipefd[1]);
	g_vars->pid[i] = fork();
	if (g_vars->pid[i] == -1)
		return ;
	if (g_vars->pid[i] == 0)
		handle_child((*tmp), i);
	if ((i == 0 && g_vars->op[i * 2] == '1') || (i > 0 && (g_vars->op[(i - 1) * 2] == '1' || g_vars->op[(i) * 2] == '1')))
	{
		fd_handler(i);
	}
	
	// else
	// 	(g_vars->ex_status) = WEXITSTATUS(status);
}

void	execution_phase()
{
	t_command	*tmp;
	int			i;
	int			j;
	int			status;
	g_vars->pid = my_alloc(sizeof(int) * g_vars->command_count);
	tmp = g_vars->command_head;
	i = -1;
	while (++i < g_vars->command_count)
	{
		if (!tmp->command_args[0])
		{
			// printf("pp\n");
			tmp->is_valid_command = 69;
		}
		
		tmp->command_args = expand_variables(tmp->info, tmp->command_args);
		j = -1;
		while (tmp->command_args[++j])
			fix_string(tmp->info, tmp->command_args[j], tmp->command_args[j]);
		// printf("0: %s || 1: %s || is: %d\n", tmp->command_args[0], tmp->command_args[1],built_in_should_execute_in_main(tmp));
		if (tmp->command_args[0] && built_in_should_execute_in_main(tmp) && (!g_vars->op[0] || (g_vars->op[0] && (i - 1 >= 0 && g_vars->op[(i - 1) * 2] != '1') && (g_vars->op[i * 2] && g_vars->op[i * 2] != '1')) || (g_vars->op[0] && (i == 0 && (g_vars->op[i * 2] && g_vars->op[i * 2] != '1')))))
		{
			execute_built_in(tmp);
		}
		else
		{
			// printf("pp\n");
			execute(&tmp, &i);
		}
		if (tmp)
			tmp = tmp->next;
	}
	// printf("op: %s\n", g_vars->op);
	tmp = g_vars->command_head;
	i=-1;
	while (++i < g_vars->command_count)
	{
		if (!built_in_should_execute_in_main(tmp) || (built_in_should_execute_in_main(tmp) && ((g_vars->op[0] && ((i - 1 >= 0 && g_vars->op[(i - 1) * 2] == '1') || (g_vars->op[i * 2] && g_vars->op[i * 2] == '1'))))))
		{
			int j = -1;
			while (++j <= i)
			{
				if (waitpid(g_vars->pid[j], &status, 0) != -1)
					(g_vars->ex_status) = status >> 8;
			}
		}
		tmp = tmp->next;
	}
	if (g_vars->is_interrupted)
	{
		// g_var[is_interrupted] = 0;
		if (g_vars->interrupted_mode == 1)
			g_vars->ex_status = 130;
		if (g_vars->interrupted_mode == 2)
			g_vars->ex_status = 131;
		if (g_vars->interrupted_mode == 3)
			g_vars->ex_status = 1;
		g_vars->is_interrupted = 0;
		g_vars->interrupted_mode = 0;
		// g_var[ex_status] = g_vars->ex_status;
		// g_var[ex_status] = g_vars->ex_status;
	}
	// printf("interrupted_mode %d || is interupted %d || is_running %d\n", g_vars->interrupted_mode, g_vars->is_interrupted,g_vars->is_running);
}

void	start_ter()
{
	char	*line;

	g_vars->is_running = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &g_vars->new_term);
	if (isatty(STDIN_FILENO))
	{	
		//user@hostname:/path/to/current/directory$
		line = readline("minishell> ");
		garbage_collector(line, 0);
	}
	else
		line = ft_strtrim(get_next_line(0), " \t\n\v\f\r");
	// g_var[is_running] = 0;
	g_vars->is_running = 1;
	tcsetattr(STDIN_FILENO, TCSANOW, &g_vars->old_term);
	if (!line)
	{
		// ft_dprintf(1, "exit\n");
		tcsetattr(STDIN_FILENO, TCSANOW, &g_vars->old_term);
		custom_exit(g_vars->ex_status);
	}
	if (line && line[0])
	{
		add_history(line);
		// parse_commands(line);
		char *arr[2] = {line, NULL};
		g_vars->iter_else_count = 0;
		g_vars->iteration = 0;
		g_vars->op = "";
		if (nested_par(arr,0, 0))
		{
			// printf("pp\n");
			g_vars->op = "";
			g_vars->iter_else_count = 0;
			g_vars->iteration = 0;
			char *arr2[2] = {line, NULL};
			
			

			
			g_vars->pipe = 0;
			if (g_vars->prev_pipefd[0] != 0)
			{
				close(g_vars->prev_pipefd[0]);
				g_vars->prev_pipefd[0] = 0;
			}
			if (g_vars->prev_pipefd[1] != 0)
			{
				close(g_vars->prev_pipefd[1]);
				g_vars->prev_pipefd[1] = 0;
			}
			if (g_vars->next_pipefd[0] != 0)
			{
				close(g_vars->next_pipefd[0]);
				g_vars->next_pipefd[0] = 0;
			}
			if (g_vars->next_pipefd[1] != 0)
			{
				close(g_vars->next_pipefd[1]);
				g_vars->next_pipefd[1] = 0;
			}
			
			
			



			nested_par(arr2,1,0);
		}
		g_vars->command_head = NULL;
	}
	// g_var[is_running] = 1;
	
}

int	main(int ac, char **av, char **ev)
{

	(void)ac;
	(void)av;
	g_vars = my_alloc(sizeof(t_args));
	if (!g_vars)
		return (0);
	g_vars->ev = ev;
	set_env();
	init_termio();
	signal(SIGINT, handle_signals);
	signal(SIGQUIT, handle_signals);
	// g_var[is_running] = 1;
	g_vars->is_running = 0;
	// g_vars->is_running = &g_var[is_running];
	// g_vars->ex_status = &g_var[ex_status];
	// g_vars->is_interrupted = &g_var[is_interrupted];
	while (1)
		start_ter();
	custom_exit(0);
	return (0);
}
