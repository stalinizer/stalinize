#include <stdio.h>
#include <time.h>
#include <cgi.h>

#include <string>
#include <cstdlib>

#include <cstdio>
#include <iostream>
#include <fstream>

void process_image(std::ofstream & of, int content_length, std::string content_type) {
int blocksize = 256;
char * buffer = new char [blocksize];
std::cin.read (buffer,blocksize);
if (std::cin) { 
printf("read %d",std::cin.gcount());
of.write(buffer, std::cin.gcount());
}
}

int main()
{	
	cgi_init();
	cgi_process_form();	
	cgi_init_headers();

	cgi_include("stalinize.html");

	if(std::getenv("CONTENT_TYPE")!=NULL)
	  printf("content type:%s",getenv("CONTENT_TYPE"));

	if(getenv("QUERY_STRING")!=NULL)
	  printf("DEBUG:%s\n",getenv("QUERY_STRING"));

	if(getenv("CONTENT_LENGTH")!=NULL)	  
	  printf( getenv("CONTENT_LENGTH") );

	if( getenv("HTTP_ACCEPT_LANGUAGE") )
	  printf( getenv("HTTP_ACCEPT_LANGUAGE") );

	if(getenv("CONTENT_LENGTH")!=NULL){
	  int _CONTENT_LENGTH = atoi( getenv("CONTENT_LENGTH"));

	  // write the msg to the file
	  std::ofstream fp;
	  fp.open("/tmp/test.png");
	  if (!fp) 
	    cgi_fatal("Failed to image file for appending");
	  process_image(fp, 
			_CONTENT_LENGTH, 
			std::string(
				    std::getenv("CONTENT_TYPE")
				    )
			);
	  fp.close();
	} else {
	  cgi_include("stalinize_results.html");
	}	
	cgi_include("stalinize_end.html");
	cgi_end();
	return 0;
}
