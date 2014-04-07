//#include <stdio.h>
#include <time.h>
#include <cgi.h>

#include <string>
#include <cstring>
#include <cstdlib>

//#include <cstdio>
#include <iostream>
//#include <fstream>

void process_image(
                   int content_length, 
                   std::string content_type
                   ) 
{
  char buffer[content_length];
  memset(buffer,' ', content_length);  
  std::cin.read(buffer, content_length);
  int readcount = std::cin.gcount();  
  printf("<p>read size %d\n", readcount);
  printf("<p>read buffer %d\n<table>", std::strlen(buffer));
  //  printf("<p>raw:\"%s\"\n",buffer); 
  for (int i=0; i < content_length; i++){
    printf("<tr><td>%d</td><td>%X</td></tr>\n",
           i, 
           //           buffer[i],
           buffer[i]);
  }
  printf("</table>");
  // cv::Mat data_mat(data);
  // cv::Mat frame(imdecode(data_mat,1));
}

int main()
{	
  //	cgi_init();
  //	cgi_process_form();	
  //	cgi_init_headers();
  printf("Content-Type: text/html\n\n");

  cgi_include("stalinize.html");

  if(std::getenv("CONTENT_TYPE")!=NULL)
    printf("<p>content type:%s",getenv("CONTENT_TYPE"));
  
  if(getenv("QUERY_STRING")!=NULL)
    printf("<p>DEBUG:%s\n",getenv("QUERY_STRING"));
  
  if(getenv("CONTENT_LENGTH")!=NULL)	  
    printf( "<p>LEN:%s",getenv("CONTENT_LENGTH") );
  
  if( getenv("HTTP_ACCEPT_LANGUAGE") )
    printf( "<p>Lang:%s", getenv("HTTP_ACCEPT_LANGUAGE") );
  
  if(getenv("CONTENT_LENGTH")!=NULL){
    int _CONTENT_LENGTH = atoi( getenv("CONTENT_LENGTH"));
    printf("<p>content len %d\n", _CONTENT_LENGTH );
    // write the msg to the file
    //	  std::ofstream fp;
    //  fp.open("/tmp/test.png");
    //  if (!fp) 
    //	    cgi_fatal("Failed to image file for appending");
    process_image(
                  //fp, 
                  _CONTENT_LENGTH, 
                  std::string(
                              std::getenv("CONTENT_TYPE")
                              )
                  );
  } else { 
  cgi_include("stalinize_results.html"); 
 }	
  cgi_include("stalinize_end.html");

  return 0;
}
