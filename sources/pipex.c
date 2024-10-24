/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzhang2 <lzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 17:19:46 by lzhang2           #+#    #+#             */
/*   Updated: 2024/10/18 22:14:31 by lzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pipex.h"

int	wait_for_children(pid_t pid1, pid_t pid2)
{
	int		status;
	int		exit_status;
	pid_t	pid;
	int		child_waited;

	exit_status = 0;
	child_waited = 0;
	while (child_waited < 2)
	{
		pid = wait(&status);
		if (pid == pid1 || pid == pid2)
		{
			if (WIFEXITED(status))
				exit_status = WEXITSTATUS(status);
		}
		child_waited++;
	}
	return (exit_status);
}

void	ft_child_1(t_prog *prog, char **argv, char **envp)
{
	prog->pid1 = fork();
	if (prog->pid1 == 0)
	{
		check_infile(prog, argv[1]);
		if ((dup2(prog->infile, STDIN_FILENO) == -1)
			|| (dup2(prog->pipe_fd[1], STDOUT_FILENO) == -1))
		{
			ft_putstr_fd("Failed to duplicate infile to stdin\n", 2);
			ft_close(prog);
			exit(EXIT_FAILURE);
		}
		ft_close(prog);
		execute_command(prog, argv[2], envp);
	}
}

void	ft_child_2(t_prog *prog, char **argv, char **envp)
{
	prog->pid2 = fork();
	if (prog->pid2 == 0)
	{
		check_outfile(prog, argv[4]);
		if ((dup2(prog->pipe_fd[0], STDIN_FILENO) == -1)
			|| dup2(prog->outfile, STDOUT_FILENO) == -1)
		{
			ft_putstr_fd("Failed to duplicate infile to stdout\n", 2);
			ft_close(prog);
			exit(EXIT_FAILURE);
		}
		ft_close(prog);
		prog->is_last_cmd = true;
		execute_command(prog, argv[3], envp);
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_prog	prog;
	int		exit_status;

	if (argc != 5)
	{
		ft_putstr_fd("argc should be 5\n", 2);
		exit(EXIT_FAILURE);
	}
	init_struct_prog(&prog);
	if (pipe(prog.pipe_fd) == -1)
	{
		ft_putstr_fd("pipe failed", 2);
		ft_close(&prog);
		return (1);
	}
	ft_child_1(&prog, argv, envp);
	ft_child_2(&prog, argv, envp);
	ft_close(&prog);
	exit_status = wait_for_children(prog.pid1, prog.pid2);
	return (exit_status);
}

/*strace -e signal=SIGPIPE -o trace.log < input /usr/bin/cat
| /usr/bin/wcgd > output
*/
/*valgrind --leak-check=full --show-leak-kinds=all 
--trace-children=yes ./pipex input /usr/bin/cat dls output
*/