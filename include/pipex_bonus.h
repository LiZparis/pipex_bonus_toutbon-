/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzhang2 <lzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 19:30:51 by lzhang2           #+#    #+#             */
/*   Updated: 2024/10/23 20:11:38 by lzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_BONUS_H
# define PIPEX_BONUS_H

# include <errno.h>
# include <stdio.h>
# include <stdlib.h>
// # include <string.h>
# include <fcntl.h>
# include <unistd.h>
// unlink
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
	int			pid;
	int			num_cmd;
	int			**pipes;
	int			num_pipes;
	int			is_first_cmd;
	int			is_last_cmd;
	char		**argv;
	char		**envp;
	t_flag		last_cmd;
	int			here_doc;
	t_find_exec	exec;
}	t_prog;

/*pipex.c*/
void	close_pipes(t_prog *prog);
void	ft_child_1(t_prog *prog, char **argv, char **envp);
void	ft_child_2(t_prog *prog, char **argv, char **envp);
void	init_struct_prog(t_prog *prog, char **argv, char **envp);
void	check_infile(t_prog *prog, char *infile);
void	check_outfile(t_prog *prog, char *infile);

/*check_param.c*/
void	is_here_doc(t_prog *prog, int argc, char **argv);

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

/*execute_command_bonus.c*/
char	*is_accessible(t_prog *prog, char *command, char **args);
void	is_execvable(t_prog *prog, char *path, char **args, char **envp);
void	execute_relative_cmd(t_prog *prog, char **args, char **envp);
void	execute_abs_cmd(t_prog *prog, char **args, char **envp);
void	execute_command(t_prog *prog, char *cmd, char **envp);
#endif