#include <stdio.h>
#include <time.h>
#include <cgi.h>

int main()
{	
	cgi_init();
	cgi_process_form();	
	cgi_init_headers();

	if (cgi_param("action")) {
		// write the msg to the file
		FILE *fp;
		fp = fopen("/tmp/test.png", "a");
		if (!fp) 
		  cgi_fatal("Failed to image file for appending");
		fseek(fp, 0, SEEK_SET);
		fprintf(fp,cgi_param("image"));
		fclose(fp);
	}

	cgi_include("stalinize.html");
	cgi_end();
	return 0;
}
