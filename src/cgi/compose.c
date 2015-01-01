#include <time.h>
#include <cgic.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include "opencv/cxcore.h"
#include <opencv2/imgproc/types_c.h>

void print_mat(const char * name, cv::Mat & m){
  return;
  cv::_InputArray psrc1(m);
  cv::imwrite( name, m);

  int kind = psrc1.kind();
  int type = psrc1.type();
  int depth = CV_MAT_DEPTH(type);
  int cn = CV_MAT_CN(type);
  int dims1 = psrc1.dims();
  cv::Size sz = psrc1.size();

  std::cout <<  "name:" << name;
  std::cout <<  " Kind:" << kind;
  std::cout <<  " Type:" << type;
  std::cout <<  " depth:" << depth;
  std::cout <<  " cn:" << cn;
  std::cout <<  " dims:" << dims1;
  std::cout <<  " Size: width:" << sz.width 
            <<  " heigh:t" << sz.height
            << std::endl;
}

int main (int argc, char ** argv) {
  if (argc < 4) {
    printf("args: x y radius, will read in input.jpg\n");
    return -1;
  }
  cv::Mat source = cv::imread( "input.jpg", 1 );
  cv::Mat stalin = cv::imread( "Stalin61.jpg", 1 );
  cv::Mat stalin_mask = cv::imread( "Stalin6_mask.jpg", 1 );
  int x=atoi(argv[1]);
  int y=atoi(argv[2]);
  int radius=atoi(argv[3]);
  std::cout <<  " x:" << x 
            <<  " y:" << y 
            <<  " radius:" << radius << std::endl;
  cv::Rect roi(
               x - radius,
               y - radius,
               radius * 2,
               radius * 2
               );
  cv::Size box(roi.width,roi.height);

  cv::resize(stalin, stalin, box);
  cv::resize(stalin_mask, stalin_mask, box);

  cv::Mat mask_image( source.size(), source.type());
  stalin_mask.copyTo(mask_image(roi));
  print_mat( "mask.jpg",  mask_image);

  cv::Mat masked_image( source.size(), source.type());
  cv::Mat masked_image2( source.size(), source.type());
  cv::bitwise_not(masked_image,masked_image2);

  stalin.copyTo(masked_image(roi));
  print_mat( "masked.jpg",  masked_image);

  stalin.copyTo(masked_image2(roi));
  print_mat( "masked2.jpg",  masked_image2);

  cv::Mat result_mask1;
  cv::bitwise_and(masked_image,mask_image,result_mask1);
  print_mat( "merged1.jpg", result_mask1);

  cv::Mat result_mask2;
  cv::add(source,mask_image,result_mask2);
  print_mat( "merged2.jpg", result_mask2);

  cv::Mat result_mask3;
  cv::bitwise_and(masked_image2,result_mask2,result_mask3);
  print_mat( "merged4a.jpg", result_mask3);
  cv::imwrite( "output.jpg", result_mask3);

}
