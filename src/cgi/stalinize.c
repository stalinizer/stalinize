//#include <stdio.h>
#include <time.h>
#include <cgi.h>

#include <string>
#include <cstring>
#include <cstdlib>

#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>


#include <iostream>
#include <fstream>
//#include "multipart.h"
#include <vector>

class Stalinizer  { 

 public:
 
  void opencv(cv::Mat& image) {
    std::string cascadeName = "haarcascade_frontalface_alt.xml";
    cv::CascadeClassifier cascade;
    double scale = 1;    
    bool tryflip=false;

    if( !cascade.load( cascadeName ) )   {
      printf("ERROR: Could not load classifier cascade %s\n",cascadeName.c_str());
      return;
    }    
    detectAndDraw( image, cascade, scale, tryflip );
  }
  
  
  void detectAndDraw( cv::Mat& img, 
                      cv::CascadeClassifier& cascade,
                      double scale, 
                      bool tryflip )
  {
    int i = 0;
    double t = 0;
    std::vector<cv::Rect> faces, faces2;
    
    cv::Mat gray, smallImg( cvRound (img.rows/scale), cvRound(img.cols/scale), CV_8UC1 );

    cvtColor( img, gray, cv::COLOR_BGR2GRAY );
    resize( gray, smallImg, smallImg.size(), 0, 0, cv::INTER_LINEAR );
    equalizeHist( smallImg, smallImg );

    cascade.detectMultiScale( 
                             smallImg,
                             faces,
                             1.1, 2, 
                             0|cv::CASCADE_SCALE_IMAGE,
                             cv::Size(30, 30) );


    cv::Mat stalin = cv::imread( "Stalin.png", 1 );

    printf("After detect\n");

    if( tryflip )
    {
        flip(smallImg, smallImg, 1);
        cascade.detectMultiScale( smallImg, faces2,
                                 1.1, 2, 0
                                 //|CASCADE_FIND_BIGGEST_OBJECT
                                 //|CASCADE_DO_ROUGH_SEARCH
                                  |cv::CASCADE_SCALE_IMAGE
                                 ,
                                  cv::Size(30, 30) );
        for( std::vector<cv::Rect>::const_iterator r = faces2.begin(); r != faces2.end(); r++ )
        {
          faces.push_back(
                          cv::Rect(smallImg.cols - r->x - r->width, r->y, r->width, r->height));
        }
    }

    
    double alpha = 0.5; double beta; double input;
    beta = ( 1.0 - alpha );
    cv::Mat dst;

    for( std::vector<cv::Rect>::const_iterator r = faces.begin(); r != faces.end(); r++, i++ )
      {
        printf("Face\n");
        cv::Mat smallImgROI;
        cv::Point center;        
        int radius;
        double aspect_ratio = (double)r->width/r->height;

        center.x = cvRound((r->x + r->width*0.5)*scale);
        center.y = cvRound((r->y + r->height*0.5)*scale);
        radius = cvRound((r->width + r->height)*0.25*scale);
        printf("x=%d y=%d radius=%d\n",
               center.x ,
               center.y,
               radius);

        cv::addWeighted( img, alpha, stalin, beta, 0.0, dst);
        cv::imwrite( "/tmp/output.jpg", dst );
      }

  }

  
  void main()
  {
    printf("Content-Type: text/html\n\n");    
    cgi_include("stalinize.html");
    
    if(getenv("CONTENT_LENGTH")!=NULL){
      int _CONTENT_LENGTH = atoi( getenv("CONTENT_LENGTH"));

      printf("<p>Got an attachement\n");    
      process_data(
                   _CONTENT_LENGTH, 
                   std::string(
                               std::getenv("CONTENT_TYPE")
                               )
                   );
    } else { 
      cgi_include("stalinize_results.html"); 
    }	
    cgi_include("stalinize_end.html");
    
  }

  void process_data(
                    int content_length, 
                    std::string content_type
                    ) 
  {
    char buffer[content_length];
    std::cin.read(buffer, content_length);
    
    std::vector<char> data(buffer, buffer + content_length);
    cv::Mat data_mat(data);
    cv::Mat frame(imdecode(data_mat,1));

    opencv(frame);

  }

};


int main()
{	
  Stalinizer app;
  app.main();
  return 0;
}
