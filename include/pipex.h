/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzhang2 <lzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 19:30:51 by lzhang2           #+#    #+#             */
/*   Updated: 2024/10/23 17:00:04 by lzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include <errno.h>
# include <stdio.h>
# include <stdlib.h>
// # include <string.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <unistd.h>
# include "../libraries/Libft/libft.h"

typedef enum s_flag
{
	true,
	false
}	t_flag;

typedef struct s_find_exec
{
	char	*path_env;
	char	**paths;
	char	*full_path;
	char	*access_ok;
	int		i;
}	t_find_exec;

typedef struct s_prog
{
	int			infile;
	int			outfile;
	int			pipe_fd[2];
	pid_t		pid1;
	pid_t		pid2;
	t_find_exec	exec;
	t_flag		is_last_cmd;
	t_flag		is_abs_cmd;
}	t_prog;

/*check_files.c*/
void	init_struct_prog(t_prog *prog);
void	check_infile(t_prog *prog, char *infile);
void	check_outfile(t_prog *prog, char *infile);

/*pipex.c*/
int		wait_for_children(pid_t pid1, pid_t pid2);
void	ft_child_1(t_prog *prog, char **argv, char **envp);
void	ft_child_2(t_prog *prog, char **argv, char **envp);

/*find_executable.c*/
void	check_path_env_accessible(t_prog *prog, char **command);
char	*ft_get_path_env(char **envp);
char	*build_full_path(const char *dir, const char *command);
char	*find_executable(t_prog *prog, char **command, char **envp);

/*free.c*/
void	path_env_error(char *command);
void	ft_close(t_prog *prog);
void	ft_free_split(char **split);
void	is_last_cmd(t_prog *prog);

/*execute_command.c*/
char	*is_accessible(t_prog *prog, char *command, char **args);
void	is_execvable(t_prog *prog, char *path, char **args, char **envp);
void	execute_relative_cmd(t_prog *prog, char **args, char **envp);
void	execute_abs_cmd(t_prog *prog, char **args, char **envp);
void	execute_command(t_prog *prog, char *cmd, char **envp);

#endif