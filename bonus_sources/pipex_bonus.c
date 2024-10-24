/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzhang2 <lzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 17:19:46 by lzhang2           #+#    #+#             */
/*   Updated: 2024/10/24 18:00:38 by lzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pipex_bonus.h"

void	init_struct_prog(t_prog *prog, char **argv, char **envp)
{
	prog->infile = -1;
	prog->outfile = -1;
	prog->pid = -1;
	prog->num_cmd = 0;
	prog->pipes = NULL;
	prog->num_pipes = 0;
	prog->is_first_cmd = 0;
	prog->is_last_cmd = 0;
	prog->argv = argv;
	prog->envp = envp;
	prog->last_cmd = false;
	prog->here_doc = 0;
	prog->exec.path_env = NULL;
	prog->exec.paths = NULL;
	prog->exec.full_path = NULL;
	prog->exec.access_ok = NULL;
	prog->exec.i = 0;
}
int	wait_for_children(int num_cmd)
{
	int status;
	int exit_status = 0;
	pid_t pid;

	for (int i = 0; i < num_cmd; i++)
	{
		pid = wait(&status);
		if (pid > 0) // 成功等待到一个子进程
		{
			if (WIFEXITED(status)) // 检查子进程是否正常退出
			{
				exit_status = WEXITSTATUS(status); // 获取子进程的退出状态
				printf("Child %d exited with status %d\n", pid, exit_status);
			}
			else if (WIFSIGNALED(status)) // 检查子进程是否被信号终止
			{
				printf("Child %d terminated by signal %d\n", pid, WTERMSIG(status));
			}
		}
		else
		{
			perror("waitpid failed");
		}
	}
	return exit_status; // 返回最后一个子进程的退出状态
}


void	handle_here_doc(const char *limiter, char *temp_file)
{
	char	*line;
	int		fd;

	fd = open(temp_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		perror("Failed to open temp_file");
		exit(EXIT_FAILURE);
	}
	line = NULL;
	while (1)
	{
		write(STDOUT_FILENO, "here_doc> ", 10);
		line = get_next_line(STDIN_FILENO);
		if (!line)
		{
			perror("Failed to read line");
			unlink(temp_file);
			close(fd);
			exit(EXIT_FAILURE);
		}
		if (ft_strncmp(line, limiter, ft_strlen(limiter))
			== 0 && line[ft_strlen(limiter)] == '\n')
		{
			free(line);
			break ;
		}
		write(fd, line, ft_strlen(line));
		free(line);
	}
	close(fd);
}

void	is_here_doc(t_prog *prog, int argc, char **argv)
{
	if (argc >= 6 && (ft_strncmp(argv[1], "here_doc", 8) == 0)
		&& argv[1][8] == '\0')
	{
		prog->here_doc = 1;
		prog->num_cmd = argc - 4;
		prog->is_first_cmd = 3;
		prog->is_last_cmd = argc - 2;
	}
	else if (argc < 6 && (ft_strncmp(argv[1], "here_doc", 8) == 0)
		&& argv[1][8] == '\0')
	{	
		ft_putstr_fd("Usage: ./pipex here_doc delimiter cmd1 cmd2 file2\n", 2);
		exit(EXIT_FAILURE);
	}
	else
	{
		prog->num_cmd = argc - 3;
		prog->is_first_cmd = 2;
		prog->is_last_cmd = argc - 2;
	}
	prog->num_pipes = prog->num_cmd - 1;
}

void	close_pipes(t_prog *prog)
{
	int	i;

	if (prog->pipes)
	{
		i = 0;
		while (i < prog->num_pipes)
		{
			if (prog->pipes[i])
			{
				close(prog->pipes[i][0]);
				close(prog->pipes[i][1]);
				free(prog->pipes[i]);
			}
			i++;
		}
	}
	free(prog->pipes);
	prog->pipes = NULL;
}

void	create_pipes(t_prog *prog)
{
	int	i;

	prog->pipes = malloc(sizeof(int *) * prog->num_pipes);
	if (!prog->pipes)
		exit(EXIT_FAILURE);
	i = 0;
	while (i < prog->num_pipes)
	{
		prog->pipes[i] = malloc(sizeof(int) * 2);
		if (pipe(prog->pipes[i]) == -1)
		{
			perror("Failed to create pipe");
			exit(EXIT_FAILURE);
		}
		i++;
	}
}

void	ft_close(t_prog *prog)
{
	if (prog->infile != -1)
	{
		close(prog->infile);
		prog->infile = -1;
	}
	if (prog->outfile != -1)
	{
		close(prog->outfile);
		prog->outfile = -1;
	}
	close_pipes(prog);
}

void ft_dup_error(t_prog *prog)
{
	ft_putstr_fd("Failed to duplicate infile to stdin\n", 2);
	ft_close(prog);
	exit(EXIT_FAILURE);
}

void	ft_child(t_prog *prog, int index)
{
	if (index == prog->is_first_cmd)
	{
		printf("Executing first command: %s\n", prog->argv[index]);
		if ((dup2(prog->infile, STDIN_FILENO) == -1) || (dup2(prog->pipes[index
						- prog->is_first_cmd][1], STDOUT_FILENO) == -1))
			ft_dup_error(prog);
	}
	else if (index > prog->is_first_cmd && index < prog->is_last_cmd)
	{
		printf("Executing midcommand: %s\n", prog->argv[index]);
		if ((dup2(prog->pipes[index - prog->is_first_cmd - 1][0], STDIN_FILENO)
			== -1) || (dup2(prog->pipes[index
			- prog->is_first_cmd][1], STDOUT_FILENO) == -1))
			ft_dup_error(prog);
	}
	else if (index == prog->is_last_cmd)
	{
		prog->last_cmd = true;
		printf("Executing last command: %s\n", prog->argv[index]);
		if (dup2(prog->pipes[index - prog->is_first_cmd - 1][0], STDIN_FILENO)
			== -1 || dup2(prog->outfile, STDOUT_FILENO) == -1)
			ft_dup_error(prog);
	}
	ft_close(prog);
	execute_command(prog, prog->argv[index], prog->envp);
}


int	main(int argc, char **argv, char **envp)
{
	t_prog	prog;
	char	*temp_file;
	int		index;
	int		exit_status;

	temp_file = "/tmp/heredoc.txt";
	if (argc < 5)
	{
		ft_putstr_fd("Usage: ./pipex file1 cmd1 cmd2 file2\n", 2);
		exit(EXIT_FAILURE);
	}
	init_struct_prog(&prog, argv, envp);
	is_here_doc(&prog, argc, argv);
	if (prog.here_doc)
	{
		handle_here_doc(argv[2], temp_file);
		check_infile(&prog, temp_file);
	}
	else
		check_infile(&prog, argv[1]);
	if (prog.infile == -1)
	{
		perror("Failed to open infile");
		exit(EXIT_FAILURE);
	}
	check_outfile(&prog, argv[argc - 1]);
	create_pipes(&prog);
	index = prog.is_first_cmd;
	while (index <= prog.is_last_cmd)
	{
		prog.pid = fork();
		if (prog.pid == 0)
		{
			ft_child(&prog, index);
			exit(EXIT_SUCCESS);
		}
		else if (prog.pid < 0)
		{
			perror("fork failed");
			ft_close(&prog);
			exit(EXIT_FAILURE);
		}
		index++;
	}
	unlink(temp_file);
	ft_close(&prog);
	exit_status = wait_for_children(prog.num_cmd);
	return (exit_status);
}

/*strace -e signal=SIGPIPE -o trace.log < input /usr/bin/cat
| /usr/bin/wcgd > output
*/
/*valgrind --leak-check=full --show-leak-kinds=all 
--trace-children=yes ./pipex input /usr/bin/cat dls output
*/