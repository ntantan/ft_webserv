/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ntan <ntan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/13 15:49:06 by ntan              #+#    #+#             */
/*   Updated: 2022/12/22 18:22:34 by ntan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"

Response::Response(Context cont) : context(cont),
	version_code_message("version_code_message", "HTTP/1.1;200;OK", ";"),
	server("Server", "42Webserv", ""),
	content_length("Content_Length", "0", ""),
	content_type("Content_Type", "text/plain", ""),
	body("body", "Hello world!", "")
{
	check_response();
	make_response();
}

/////////////////////////////////////////////////////////////////////////////////
///////////////////////////// CHECK RESPONSE FIELDS /////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

void	Response::check_response()
{
	if (check_version())
		return (set_status("505"));
	if (check_max_body_size())
		return (set_status("413"));
	if (check_allowed_methods())
		return (set_status("405"));
	if (check_file())
		return (set_status("404"));
	if (check_access())
		return (set_status("403"));
	if (check_rewrite())
		return (set_status("301"));
	set_status("200");
}

int		Response::check_version()
{
	if (context.request.version[0] != "HTTP/1.1")
		return (1);
	return (0);
}

int		Response::check_max_body_size()
{
	typedef std::vector<std::string>::iterator	iterator;
	iterator	beg = context.request.body.values.begin();
	iterator	end = context.request.body.values.end();
	size_t		count = 0;
	for (;beg != end; beg++)
	{
		count += (*beg).size();
		if (count > std::atof(context.server.client_max_body_size[0].c_str()))
			return (1);
	}
	return (0);
}

int		Response::check_allowed_methods()
{
	typedef std::vector<std::string>::iterator	iterator;
	iterator	beg = context.location.allowed_methods.values.begin();
	iterator	end = context.location.allowed_methods.values.end();
	std::string	find = context.request.method[0];
	if (std::find(beg, end, find) == end)
		return (1);
	return (0);
}

int		Response::check_file()
{
	std::string 	path = context.location.root[0] + context.request.path[0]; 

	struct stat s;
	if( stat(path.c_str(),&s) == 0 )
	{
		if( s.st_mode & S_IFDIR )
			std::cout << path << " is a directory !" << std::endl;
		else if( s.st_mode & S_IFREG )
			std::cout << path << " is a file !" << std::endl;
		else
			std::cout << path << " is something else ?" << std::endl;
		return (0);
	}
	return (1);
}

int		Response::check_access()
{
	std::string 	path = context.location.root[0] + context.request.path[0];

	if (access(path.c_str(), R_OK) < 0)
		return (1);
	return (0);
}

int		Response::check_rewrite()
{
	if (context.location.rewrite.delimiter == "TRUE")
		return (1);
	return (0);
}

void	Response::set_status(std::string code)
{
	this->version_code_message[0] = "HTTP/1.1";
	this->version_code_message[1] = code;
	this->version_code_message[2] = status.get_message(code);
}

/////////////////////////////////////////////////////////////////////////////////
///////////////////////// MAKE BODY ACCORDING TO STATUS /////////////////////////
/////////////////////////////////////////////////////////////////////////////////

void	Response::read_file(std::string path)
{
	std::ifstream input(path.c_str(), std::ios::binary);
    std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

	for (size_t i = 0; i < buffer.size(); i++)
		charvec_response.push_back(buffer[i]);

	int i = buffer.size();
	std::stringstream digit;
	digit << i;
	std::string numberString(digit.str());
	content_length[0] = numberString;
}

int	Response::directory_listing(std::string path)
{
	DIR *dir; struct dirent *diread;
    std::vector<char *> files;

    if ((dir = opendir(path.c_str())) != NULL) {
        while ((diread = readdir(dir)) != NULL) {
            files.push_back(diread->d_name);
        }
        closedir(dir);
    } else {
        std::cerr << "opendir failed" << std::endl;
        return (1);
    }

	for (std::vector<char *>::iterator it = files.begin(); it != files.end(); it++)
	{
		add_string_to_vector("<a href=\"./" + std::string(*it) + "\">" + std::string(*it) + "</a>\n");
	}
	return (0);
}

void	Response::make_body()
{
	add_string_to_vector("\n");
	std::string path = context.location.root[0] + context.request.path[0];
	struct stat s;
	// GET
	if (context.request.method[0] == "GET")
	{
		if( stat(path.c_str(),&s) == 0 && (version_code_message[1][0] != '4' || version_code_message[1][0] != '5'))
		{
			if( s.st_mode & S_IFDIR ) // If is a directory
			{
				if (context.location.autoindex[0] == "on")
					directory_listing(path);
				else
					read_file(context.location.default_dir_request[0]);
			}
			else if( s.st_mode & S_IFREG ) // If is a regular file
			{
				if (context.request.path[0].find(".php") != std::string::npos)
				{
					if (context.location.cgi[0] == "on")
					{
						CgiHandler cgi(context);
						add_string_to_vector(cgi.executeCGI(path));
					}
					else
					{
						set_status("403");
						add_string_to_vector("Status code : " + version_code_message[1] + "/" + version_code_message[2]);
					}
				}
				else
					read_file(path);			
			}
			else
			{
				set_status("418");
				add_string_to_vector("Status code : " + version_code_message[1] + "/" + version_code_message[2]);
			}
		}
		else
			add_string_to_vector("Status code : " + version_code_message[1] + "/" + version_code_message[2]);
	}
	
	// POST
	if (context.request.method[0] == "POST")
	{
		if( stat(path.c_str(),&s) == 0 && (version_code_message[1][0] != '4' || version_code_message[1][0] != '5') )
		{
			if (context.request.path[0].find(".php") != std::string::npos)
			{
				if (context.location.cgi[0] == "on")
				{
					CgiHandler cgi(context);
					add_string_to_vector(cgi.executeCGI(path));
				}
				else
				{
					set_status("403");
					add_string_to_vector("Status code : " + version_code_message[1] + "/" + version_code_message[2]);
				}
			}
			else
			{
				std::cout << "\n----- POST RECEIVED -----" << std::endl;
				std::cout << "----- WHAT AM I SUPPOSED TO DO WITH THAT ? -----" << std::endl;
				for (std::vector<std::string>::iterator it = context.request.body.values.begin(); it != context.request.body.values.end(); it++)
					std::cout << *it << std::endl;
			}
				
		}
		else
			add_string_to_vector("Status code : " + version_code_message[1] + "/" + version_code_message[2]);
	}
	
	// DELETE
	else if (context.request.method[0] == "DELETE")
	{
		if( stat(path.c_str(),&s) == 0 && (version_code_message[1][0] != '4' || version_code_message[1][0] != '5') )
		{
			if (context.request.path[0].find(".php") != std::string::npos)
			{
				if (context.location.cgi[0] == "on")
				{
					CgiHandler cgi(context);
					add_string_to_vector(cgi.executeCGI(path));
				}
				else
				{
					set_status("403");
					add_string_to_vector("Status code : " + version_code_message[1] + "/" + version_code_message[2]);
				}
			}
			else if (remove(path.c_str()))
				add_string_to_vector("Status code : " + version_code_message[1] + "/" + version_code_message[2]);
		}
		else
			add_string_to_vector("Status code : " + version_code_message[1] + "/" + version_code_message[2]);
	}
}

/////////////////////////////////////////////////////////////////////////////////
////////////////////////// FINAL RESPONSE ASSEMBLER /////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

void	Response::add_string_to_vector(std::string str)
{
	for (size_t i = 0; i < str.size(); i++)
		this->charvec_response.push_back(str[i]);
}

void	Response::make_response()
{
	// http 
	add_string_to_vector(version_code_message[0] + " " + version_code_message[1] + " " + version_code_message[2] + "\n");
	
	// header
	add_string_to_vector(server.name + ": " + server[0] + "\n");
	add_string_to_vector(content_type.name + ": " + content_type[0] + "\n");

	// body
	make_body();
} 

const char	*Response::get_response()
{
	return (response.c_str());
}

std::vector<unsigned char> Response::get_vector()
{
	return (charvec_response);
}

///////////////////////////////////////////////////////////////////////////////////// 

void	Response::print_response()
{
	std::cout << std::endl;
	std::cout << "----- [RESPONSE] -----" << std::endl;
	version_code_message.print();
	content_length.print();
	content_type.print();
	server.print();
	// body.print();
}
 