/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_signals.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykhayri <ykhayri@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/24 18:30:46 by abouabra          #+#    #+#             */
/*   Updated: 2023/08/19 13:19:40 by ykhayri          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	init_termio(void)
{
	tcgetattr(STDIN_FILENO, &g_vars->old_term);
	g_vars->new_term = g_vars->old_term;
	g_vars->new_term.c_lflag &= ~ECHOCTL;
	tcsetattr(STDIN_FILENO, TCSANOW, &g_vars->new_term);
}
