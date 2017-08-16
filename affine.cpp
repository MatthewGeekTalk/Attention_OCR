void CPlateLocate::affine(const Mat &in, Mat &out, const double slope) {
  // imshow("in", in);
  // waitKey(0);

  Point2f dstTri[3];
  Point2f plTri[3];

  float height = (float) in.rows;
  float width = (float) in.cols;
  float xiff = (float) abs(slope) * height;

  if (slope > 0) {

    // right, new position is xiff/2

    plTri[0] = Point2f(0, 0);
    plTri[1] = Point2f(width - xiff - 1, 0);
    plTri[2] = Point2f(0 + xiff, height - 1);

    dstTri[0] = Point2f(xiff / 2, 0);
    dstTri[1] = Point2f(width - 1 - xiff / 2, 0);
    dstTri[2] = Point2f(xiff / 2, height - 1);
  } else {

    // left, new position is -xiff/2

    plTri[0] = Point2f(0 + xiff, 0);
    plTri[1] = Point2f(width - 1, 0);
    plTri[2] = Point2f(0, height - 1);

    dstTri[0] = Point2f(xiff / 2, 0);
    dstTri[1] = Point2f(width - 1 - xiff + xiff / 2, 0);
    dstTri[2] = Point2f(xiff / 2, height - 1);
  }

  Mat warp_mat = getAffineTransform(plTri, dstTri);

  Mat affine_mat;
  affine_mat.create((int) height, (int) width, TYPE);

  if (in.rows > HEIGHT || in.cols > WIDTH)

    warpAffine(in, affine_mat, warp_mat, affine_mat.size(),
               CV_INTER_AREA);
  else
    warpAffine(in, affine_mat, warp_mat, affine_mat.size(), CV_INTER_CUBIC);

  out = affine_mat;
}
