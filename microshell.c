/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fvizcaya <fvizcaya@student.42madrid.com>   #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-04-21 21:21:25 by fvizcaya          #+#    #+#             */
/*   Updated: 2025-04-21 21:21:25 by fvizcaya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                                                            */
/*   This solution is an homage to fmorenil's celebrated t_cmd                */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>

#define FATAL_ERR return (ft_perror("error: fatal\n"), -1)

typedef struct s_cmd
{
	char	*cmd;
	char	**args;
	char	**env;
	int		ispipe;
	int		numargs;
}				t_cmd;

int	ft_strlen(char *str)
{
	int	len = 0;

	while (str[len])
		len++;
	return (len);
}
int	ft_perror(char *msg)
{
	if (write(2, msg, ft_strlen(msg)) == -1)  FATAL_ERR;
	return (0);
}

int	ft_chdir(t_cmd *cmd)
{
	if (cmd->numargs != 2)
		return (ft_perror("error: cd: bad arguments\n"), 0);
	if (chdir(cmd->args[1]) == -1)
	{
		ft_perror("error: cd: cannot change dir to ");
		ft_perror(cmd->args[1]);
		ft_perror("\n");
		return (-1);
	}
	return (0);
}

int	ft_exec(t_cmd *cmd)
{
	int fd[2];
	int	pid;

	if (cmd->ispipe)
		if (pipe(fd) == -1) FATAL_ERR;
	pid = fork();
	if (!pid)
	{
		cmd->args[cmd->numargs] = NULL;
		if (cmd->ispipe)
		{
			close(1);
			if (dup(fd[1]) == -1) 	FATAL_ERR;
			close(fd[0]);
			close(fd[1]);
		}
		if (execve(cmd->cmd, cmd->args, cmd->env) == -1)
		{
			ft_perror("error: cannot execute ");
			ft_perror(cmd->cmd);
			ft_perror("\n");
			return (-1);
		}
	}
	waitpid(pid, NULL, 0);
	if (cmd->ispipe)
	{
		close(0);
		if(dup(fd[0]) == -1) FATAL_ERR;
		close(fd[0]);
		close(fd[1]);
	}
	return (0);
}
int	ft_man_of_the_middle(t_cmd *cmd)
{
	if (!strcmp(cmd->cmd, "cd"))
		ft_chdir(cmd);
	else
	{
		cmd->ispipe = cmd->args[cmd->numargs] && !strcmp(cmd->args[cmd->numargs], "|");
		if (ft_exec(cmd) == -1)
			return (-1);
	}
	return (0);
}

int main(int argc, char **argv, char **envp)
{
	t_cmd	cmd;

	if (argc < 2)
		return (0);
	cmd.args = argv;
	cmd.env = envp;
	cmd.ispipe = 0;
	cmd.numargs = 0;
	while (cmd.args[cmd.numargs] && cmd.args[cmd.numargs + 1])
	{
		cmd.numargs++;
		cmd.args += cmd.numargs;
		cmd.numargs = 0;
		cmd.cmd = cmd.args[0];
		while (cmd.args[cmd.numargs] && strcmp(cmd.args[cmd.numargs], "|") && strcmp(cmd.args[cmd.numargs], ";"))
			cmd.numargs++;
		if (!strcmp(cmd.cmd, "cd"))
			ft_man_of_the_middle(&cmd);
		else if (cmd.numargs > 0)
			if (ft_man_of_the_middle(&cmd) == -1)
				exit(1);
	}
}