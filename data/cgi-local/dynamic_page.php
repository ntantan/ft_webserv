#!/usr/bin/php
<?php
	// print "Content-type: text/html\n\n";
	print "<html>\n";
	print "<head>\n";
	print "<title>Dynamic Page</title>\n";
	print "</head>\n";
	print "</body>\n";
	print "<h1>Hello, ceci est une page html dynamique en php. Merci au revoir.</h>\n";
	print "</body>\n";
	print "</html>\n";

?>
	<head>
	</head>
	<body>
		<nav class="navbar navbar-expand-lg navbar-light bg-light fixed-top">
		  <div class="container">
		    <a class="navbar-brand" href="/">Webserv-42</a>
		    <button class="navbar-toggler" type="button" data-bs-toggle="collapse" data-bs-target="#navbarResponsive" aria-controls="navbarResponsive" aria-expanded="false" aria-label="Toggle navigation">
		      <span class="navbar-toggler-icon"></span>
		    </button>
		    <div class="collapse navbar-collapse" id="navbarResponsive">
		      <ul class="navbar-nav ms-auto">
		        <li class="nav-item">
		          <a class="nav-link" href="/galery.php">Galery</a>
		        </li>
		        <li class="nav-item">
		          <a class="nav-link" href="/upload.php">Upload</a>
		        </li>
		        <li class="nav-item">
		          <a class="nav-link" href="/file_list.php">Files</a>
		        </li>
		        <li class="nav-item">
		          <a class="nav-link" href="/delete_files.php">Delete</a>
		        </li>
		        <li class="nav-item">
		          <a class="nav-link" href="/session.php">Session</a>
		        </li>
				<li class="nav-item">
					<a class="nav-link" href="/cookies.php">Cookies</a>
				</li>  
				<li class="nav-item">
					<a class="nav-link" href="/perl.pl">Perl</a>
				</li>  
				<li class="nav-item">
					<a class="nav-link" href="/python-cgi.py">Python</a>
				</li>  
		      </ul>
		    </div>
		  </div>
		</nav>
	</body>