/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ntan <ntan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/21 15:56:33 by rsung             #+#    #+#             */
/*   Updated: 2022/12/23 14:51:47 by ntan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"

// CgiHandler::CgiHandler(void) {}

CgiHandler::CgiHandler(Context &context) : context(context)
{
	if (context.request.body.values.size() >= 3 && !context.request.body[2].empty())
		_body = context.request.body[2];
	else
		_body = std::string();
	this->_initEnv();
}

// CgiHandler::CgiHandler(CgiHandler const &other)
// {
// 	if (this != &other)
// 	{
// 		this->_body = other._body;
// 		this->_env = other._env;
// 	}
// 	return ;
// }

CgiHandler::~CgiHandler(void) {}

// CgiHandler	&CgiHandler::operator=(CgiHandler const &other)
// {
// 	if (this != &other)
// 	{
// 		this->_body = other._body;
// 		this->_env = other._env;
// 	}
// 	return (*this);
// }

void	CgiHandler::_initEnv()
{
	this->_env["REDIRECT_STATUS"] = "200";
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_env["CONTENT_TYPE"] = "";
	this->_env["CONTENT_LENGTH"] = this->_body.length();
	this->_env["HTTP_USER_AGENT"] = context.request.user_agent[0];
	this->_env["PATH_INFO"] = context.request.path[0]; //path for CGI script
	this->_env["PATH_TRANSLATED"] = context.location.root[0];
	this->_env["QUERY_STRING"] = context.request.path[0];
	this->_env["REMOTE_ADDR"] = context.request.host[1];
	this->_env["REMOTE_HOST"] = "";
	this->_env["REMOTE_IDENT"] = "";
	this->_env["REMOTE_USER"] = "";
	this->_env["REQUEST_METHOD"] = context.request.method[0];
	this->_env["SCRIPT_FILENAME"] = context.request.path[0]; // the full path to the CGI script
	this->_env["SCRIPT_NAME"] = context.request.path[0]; // CGI script name
	this->_env["SERVER_NAME"] = context.request.host[0]; // server IP address or hostname
	this->_env["SERVER_PORT"] = context.request.host[1]; // port 8000
	this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_env["SERVER_SOFTWARE"] = "42webserv/1.0";
	this->_env["BODY"] = _body;
}

char	**CgiHandler::getEnv() const
{
	char	**env = new char*[this->_env.size() + 1];
	int		j = 0;

	for (std::map<std::string, std::string>::const_iterator i = this->_env.begin(); i != this->_env.end(); i++)
	{
		std::string	element = i->first + "=" + i->second;
		env[j] = new char[element.size() + 1];
		env[j] = strcpy(env[j], (const char*)element.c_str());
		j++;
	}
	env[j] = NULL;
	return (env);
}

std::string	CgiHandler::executeCGI(const std::string  &scriptName)
{
	pid_t		pid;
	int			tmpStdin;
	int			tmpStdout;
	char		**env;
	std::string	newBody;

	try
	{
		env = this->getEnv();
	}
	catch (std::bad_alloc &e)
	{
		std::cerr << "error: bad alloc." << std::endl;
	}

	tmpStdin = dup(STDIN_FILENO);
	tmpStdout = dup(STDOUT_FILENO);

	FILE	*fIn = std::tmpfile();
	FILE	*fOut = std::tmpfile();
	long	fdIn = fileno(fIn);
	long	fdOut = fileno(fOut);
	int		ret = 1;

	std::fputs(_body.c_str(), fIn);
	std::rewind(fIn);
	// write(fdIn, _body.c_str(), _body.size());
	// lseek(fdIn, 0, SEEK_SET);

	pid = fork();

	if (pid == -1)
	{
		std::cerr << "error: fork() failed." << std::endl;
		return ("Status: 500\n");
	}
	else if (!pid)
	{
		// char * const * nll = NULL;

		dup2(fdIn, STDIN_FILENO);
		dup2(fdOut, STDOUT_FILENO);
		
		// execve(scriptName.c_str(), nll, env);
		char *path = (char*) scriptName.c_str();
		char *prog = (char*) "/bin/php";
		char *argv[3] = {prog, path, NULL};
		execve(prog, argv, env);

		std::cerr << "error: execve() failed. : " << scriptName << std::endl;
		write(STDOUT_FILENO, "Status: 500\n", 12);
	}
	else
	{
		char	buffer[CGI_SIZE] = {0};

		waitpid(-1, NULL, 0);
		lseek(fdOut, 0, SEEK_SET);

		ret = 1;
		while (ret > 0)
		{
			memset(buffer, 0, CGI_SIZE);
			ret = read(fdOut, buffer, CGI_SIZE + 1);
			newBody += buffer;
		}
	}

	dup2(tmpStdin, STDIN_FILENO);
	dup2(tmpStdout, STDOUT_FILENO);
	fclose(fIn);
	fclose(fOut);
	close(fdIn);
	close(fdOut);
	close(tmpStdin);
	close(tmpStdout);

	for (size_t i = 0; env[i]; i++)
		delete[] env[i];
	delete[] env;

	if (!pid)
		exit(0);

	return (newBody);
}
