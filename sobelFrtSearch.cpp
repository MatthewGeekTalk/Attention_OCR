int CPlateLocate::sobelFrtSearch(const Mat &src,
                                 vector<Rect_<float>> &outRects) {
  Mat src_threshold;

  sobelOper(src, src_threshold, m_GaussianBlurSize, m_MorphSizeWidth,
            m_MorphSizeHeight);

  vector<vector<Point>> contours;
  findContours(src_threshold,
               contours,               // a vector of contours
               CV_RETR_EXTERNAL,
               CV_CHAIN_APPROX_NONE);  // all pixels of each contours

  vector<vector<Point>>::iterator itc = contours.begin();

  vector<RotatedRect> first_rects;

  while (itc != contours.end()) {
    RotatedRect mr = minAreaRect(Mat(*itc));


    if (verifySizes(mr)) {
      first_rects.push_back(mr);

      float area = mr.size.height * mr.size.width;
      float r = (float) mr.size.width / (float) mr.size.height;
      if (r < 1) r = (float) mr.size.height / (float) mr.size.width;
    }

    ++itc;
  }

  for (size_t i = 0; i < first_rects.size(); i++) {
    RotatedRect roi_rect = first_rects[i];

    Rect_<float> safeBoundRect;
    if (!calcSafeRect(roi_rect, src, safeBoundRect)) continue;

    outRects.push_back(safeBoundRect);
  }
  return 0;
}
