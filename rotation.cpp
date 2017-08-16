bool CPlateLocate::rotation(Mat &in, Mat &out, const Size rect_size,
                            const Point2f center, const double angle) {
  if (0) {
    imshow("in", in);
    waitKey(0);
    destroyWindow("in");
  }

  Mat in_large;
  in_large.create(int(in.rows * 1.5), int(in.cols * 1.5), in.type());

  float x = in_large.cols / 2 - center.x > 0 ? in_large.cols / 2 - center.x : 0;
  float y = in_large.rows / 2 - center.y > 0 ? in_large.rows / 2 - center.y : 0;

  float width = x + in.cols < in_large.cols ? in.cols : in_large.cols - x;
  float height = y + in.rows < in_large.rows ? in.rows : in_large.rows - y;

  /*assert(width == in.cols);
  assert(height == in.rows);*/

  if (width != in.cols || height != in.rows) return false;

  Mat imageRoi = in_large(Rect_<float>(x, y, width, height));
  addWeighted(imageRoi, 0, in, 1, 0, imageRoi);

  Point2f center_diff(in.cols / 2.f, in.rows / 2.f);
  Point2f new_center(in_large.cols / 2.f, in_large.rows / 2.f);

  Mat rot_mat = getRotationMatrix2D(new_center, angle, 1);

  /*imshow("in_copy", in_large);
  waitKey(0);*/

  Mat mat_rotated;
  warpAffine(in_large, mat_rotated, rot_mat, Size(in_large.cols, in_large.rows),
             CV_INTER_CUBIC);

  /*imshow("mat_rotated", mat_rotated);
  waitKey(0);*/

  Mat img_crop;
  getRectSubPix(mat_rotated, Size(rect_size.width, rect_size.height),
                new_center, img_crop);

  out = img_crop;

  if (0) {
    imshow("out", out);
    waitKey(0);
    destroyWindow("out");
  }

  /*imshow("img_crop", img_crop);
  waitKey(0);*/

  return true;
}
