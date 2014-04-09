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
 
//#include "opencv/cvaux.h"
#include "opencv/cxcore.h"
//#include <opencv2/core/types_c.h>

#include <opencv2/imgproc/types_c.h>

//#include "Stalin5.h"

int main () {
  cv::Mat source = cv::imread( "stalinize.me.jpg", 1 );
  cv::Mat stalin = cv::imread( "Stalin61.png", -1 );
  cv::Mat stalin_mask = cv::imread( "Stalin6_mask.png", -1 );
  int x=566;
  int y=328;
  int radius=120;
  cv::Rect roi(
               x - radius,
               y - radius,
               radius * 2,
               radius * 2
               );
  cv::Size box(roi.width,roi.height);

  cv::resize(stalin, stalin, box);
  cv::resize(stalin_mask, stalin_mask, box);
  cv::Mat source_w_alpha( source.size(), stalin.type());
  cv::cvtColor(source, source_w_alpha, CV_BGR2RGBA, 4);
  stalin.copyTo(source_w_alpha(roi), stalin_mask);

  cv::imwrite( "merged.png", source_w_alpha );
}
