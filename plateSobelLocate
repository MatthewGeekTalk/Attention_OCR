int CPlateLocate::plateSobelLocate(Mat src, vector<CPlate> &candPlates,
                                   int index) {
  vector<RotatedRect> rects_sobel_all;
  rects_sobel_all.reserve(256);

  vector<CPlate> plates;
  plates.reserve(32);

  vector<Rect_<float>> bound_rects;
  bound_rects.reserve(256);

  sobelFrtSearch(src, bound_rects);

  vector<Rect_<float>> bound_rects_part;
  bound_rects_part.reserve(256);

  // enlarge area 
  for (size_t i = 0; i < bound_rects.size(); i++) {
    float fRatio = bound_rects[i].width * 1.0f / bound_rects[i].height;
    if (fRatio < 3.0 && fRatio > 1.0 && bound_rects[i].height < 120) {
      Rect_<float> itemRect = bound_rects[i];

      itemRect.x = itemRect.x - itemRect.height * (4 - fRatio);
      if (itemRect.x < 0) {
        itemRect.x = 0;
      }
      itemRect.width = itemRect.width + itemRect.height * 2 * (4 - fRatio);
      if (itemRect.width + itemRect.x >= src.cols) {
        itemRect.width = src.cols - itemRect.x;
      }

      itemRect.y = itemRect.y - itemRect.height * 0.08f;
      itemRect.height = itemRect.height * 1.16f;

      bound_rects_part.push_back(itemRect);
    }
  }

  // second processing to split one
#pragma omp parallel for
  for (int i = 0; i < (int)bound_rects_part.size(); i++) {
    Rect_<float> bound_rect = bound_rects_part[i];
    Point2f refpoint(bound_rect.x, bound_rect.y);

    float x = bound_rect.x > 0 ? bound_rect.x : 0;
    float y = bound_rect.y > 0 ? bound_rect.y : 0;

    float width =
        x + bound_rect.width < src.cols ? bound_rect.width : src.cols - x;
    float height =
        y + bound_rect.height < src.rows ? bound_rect.height : src.rows - y;

    Rect_<float> safe_bound_rect(x, y, width, height);
    Mat bound_mat = src(safe_bound_rect);

    vector<RotatedRect> rects_sobel;
    rects_sobel.reserve(128);
    sobelSecSearchPart(bound_mat, refpoint, rects_sobel);

#pragma omp critical
    {
      rects_sobel_all.insert(rects_sobel_all.end(), rects_sobel.begin(), rects_sobel.end());
    }
  }

#pragma omp parallel for
  for (int i = 0; i < (int)bound_rects.size(); i++) {
    Rect_<float> bound_rect = bound_rects[i];
    Point2f refpoint(bound_rect.x, bound_rect.y);

    float x = bound_rect.x > 0 ? bound_rect.x : 0;
    float y = bound_rect.y > 0 ? bound_rect.y : 0;

    float width =
        x + bound_rect.width < src.cols ? bound_rect.width : src.cols - x;
    float height =
        y + bound_rect.height < src.rows ? bound_rect.height : src.rows - y;

    Rect_<float> safe_bound_rect(x, y, width, height);
    Mat bound_mat = src(safe_bound_rect);

    vector<RotatedRect> rects_sobel;
    rects_sobel.reserve(128);
    sobelSecSearch(bound_mat, refpoint, rects_sobel);

#pragma omp critical
    {
      rects_sobel_all.insert(rects_sobel_all.end(), rects_sobel.begin(), rects_sobel.end());
    }
  }

  Mat src_b;
  sobelOper(src, src_b, 3, 10, 3);

  deskew(src, src_b, rects_sobel_all, plates);

  //for (size_t i = 0; i < plates.size(); i++) 
  //  candPlates.push_back(plates[i]);

  candPlates.insert(candPlates.end(), plates.begin(), plates.end());

  return 0;
}
