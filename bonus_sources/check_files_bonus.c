/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_files_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzhang2 <lzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/18 17:17:46 by lzhang2           #+#    #+#             */
/*   Updated: 2024/10/23 20:10:48 by lzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pipex_bonus.h"

void	check_infile(t_prog *prog, char *infile)
{
	prog->infile = open(infile, O_RDONLY);
	if (prog->infile < 0)
	{
		if (errno == EACCES || errno == ENOENT || errno == EISDIR)
		{
			perror(infile);
			ft_close(prog);
			exit(0);
		}
	}
}

void	check_outfile(t_prog *prog, char *outfile)
{
	prog->outfile = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (prog->outfile < 0)
	{
		if (errno == EACCES)
		{
			perror(outfile);
			ft_close(prog);
			exit(1);
		}
		else
		{
			perror(outfile);
			ft_close(prog);
			exit(0);
		}
	}
}
