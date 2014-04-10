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
#include <vector>
#include "opencv/cxcore.h"
#include <opencv2/imgproc/types_c.h>


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
  cv::imwrite( "mask.jpg",  mask_image);

  cv::Mat masked_image( source.size(), source.type());
  stalin.copyTo(masked_image(roi));
  cv::imwrite( "masked.jpg",  masked_image);

  ////////////////////////////////////////////////////
  //masked_image.copyTo(source, mask_image);
  std::vector<cv::Mat> maskChannels(3);

  cv::split(masked_image, maskChannels);
  cv::imwrite( "chan1.jpg", maskChannels[0] );
  cv::imwrite( "chan2.jpg", maskChannels[1] );
  cv::imwrite( "chan3.jpg", maskChannels[2] );

  std::vector<cv::Mat> result_mask(3);
  cv::bitwise_and(maskChannels[0],mask_image,result_mask[0]);
  cv::bitwise_and(maskChannels[1],mask_image,result_mask[1]);
  cv::bitwise_and(maskChannels[2],mask_image,result_mask[2]);
  cv::Mat m;
  cv::merge(result_mask,m );
  cv::imwrite( "masked_merged.jpg", m);
 
  cv::Mat mask2 = 255 - mask2;
  std::vector<cv::Mat> srcChannels(3);
  cv::split(source, srcChannels);
  cv::imwrite( "schan1.jpg", srcChannels[0] );
  cv::imwrite( "schan2.jpg", srcChannels[1] );
  cv::imwrite( "schan3.jpg", srcChannels[2] );

  cv::bitwise_and(srcChannels[0],mask2,result_mask[0]);
  cv::bitwise_and(srcChannels[1],mask2,result_mask[1]);
  cv::bitwise_and(srcChannels[2],mask2,result_mask[2]);

  cv::Mat m1;
  cv::merge(result_mask,m1 );      
  cv::addWeighted(m,1,m1,1,0,m1); 
  cv::imwrite( "merged.jpg", m1 );


}
