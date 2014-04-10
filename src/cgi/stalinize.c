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
  
  void replace_face(cv::Mat & source, cv::Rect & roi) {
    cv::Size box(roi.width,roi.height);

    cv::Mat stalin_local;
    cv::resize(stalin, stalin_local, box);

    cv::Mat stalin_mask_local;
    cv::resize(stalin_mask, stalin_mask_local, box);

    stalin.copyTo(source(roi), stalin_mask);

  }

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

    printf("faces found %d\n", faces.size());

    double alpha = 0.5; double beta; double input;
    beta = ( 1.0 - alpha );
    cv::Mat dst;

    for( std::vector<cv::Rect>::const_iterator r = faces.begin(); r != faces.end(); r++, i++ )
      {
        printf("Got a face\n");

        cv::Point center;        
        int radius;
        double aspect_ratio = (double)r->width/r->height;

        center.x = cvRound((r->x + r->width*0.5)*scale);
        center.y = cvRound((r->y + r->height*0.5)*scale);
        radius = cvRound((r->width + r->height)*0.40*scale);
        printf("Face x=%d y=%d radius=%d\n",
               center.x ,
               center.y,
               radius);
        cv::Rect roi(
                     center.x - radius,
                     center.y - radius,
                     radius * 2,
                     radius * 2
                     );

        replace_face(img, roi);
       }

    cv::imwrite( "/tmp/output.jpg", dst );

  }

  void main()
  {
    stalin = cv::imread( "Stalin61.jpg", -1 );
    stalin_mask = cv::imread( "Stalin6_mask.jpg", -1 );

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

  private:
  cv::Mat stalin;
  cv::Mat stalin_mask;
};


int main()
{	
  Stalinizer app;
  app.main();
  return 0;
}
