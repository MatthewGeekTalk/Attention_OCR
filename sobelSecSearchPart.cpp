int CPlateLocate::sobelSecSearchPart(Mat &bound, Point2f refpoint,
                                     vector<RotatedRect> &outRects) {
  Mat bound_threshold;

  sobelOperT(bound, bound_threshold, 3, 6, 2);

  Mat tempBoundThread = bound_threshold.clone();

  clearLiuDingOnly(tempBoundThread);

  int posLeft = 0, posRight = 0;
  if (bFindLeftRightBound(tempBoundThread, posLeft, posRight)) {

    // find left and right bounds to repair

    if (posRight != 0 && posLeft != 0 && posLeft < posRight) {
      int posY = int(bound_threshold.rows * 0.5);
      for (int i = posLeft + (int) (bound_threshold.rows * 0.1);
           i < posRight - 4; i++) {
        bound_threshold.data[posY * bound_threshold.cols + i] = 255;
      }
    }

    utils::imwrite("resources/image/tmp/repaireimg1.jpg", bound_threshold);

    // remove the left and right boundaries

    for (int i = 0; i < bound_threshold.rows; i++) {
      bound_threshold.data[i * bound_threshold.cols + posLeft] = 0;
      bound_threshold.data[i * bound_threshold.cols + posRight] = 0;
    }
    utils::imwrite("resources/image/tmp/repaireimg2.jpg", bound_threshold);
  }

  vector<vector<Point>> contours;
  findContours(bound_threshold,
               contours,               // a vector of contours
               CV_RETR_EXTERNAL,
               CV_CHAIN_APPROX_NONE);  // all pixels of each contours

  vector<vector<Point>>::iterator itc = contours.begin();

  vector<RotatedRect> second_rects;
  while (itc != contours.end()) {
    RotatedRect mr = minAreaRect(Mat(*itc));
    second_rects.push_back(mr);
    ++itc;
  }

  for (size_t i = 0; i < second_rects.size(); i++) {
    RotatedRect roi = second_rects[i];
    if (verifySizes(roi)) {
      Point2f refcenter = roi.center + refpoint;
      Size2f size = roi.size;
      float angle = roi.angle;

      RotatedRect refroi(refcenter, size, angle);
      outRects.push_back(refroi);
    }
  }

  return 0;
}
