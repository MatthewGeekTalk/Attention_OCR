int CPlateLocate::deskew(const Mat &src, const Mat &src_b,
                         vector<RotatedRect> &inRects,
                         vector<CPlate> &outPlates, bool useDeteleArea, Color color) {
  Mat mat_debug;
  src.copyTo(mat_debug);

  for (size_t i = 0; i < inRects.size(); i++) {
    RotatedRect roi_rect = inRects[i];

    float r = (float) roi_rect.size.width / (float) roi_rect.size.height;
    float roi_angle = roi_rect.angle;

    Size roi_rect_size = roi_rect.size;
    if (r < 1) {
      roi_angle = 90 + roi_angle;
      swap(roi_rect_size.width, roi_rect_size.height);
    }

    if (m_debug) {
      Point2f rect_points[4];
      roi_rect.points(rect_points);
      for (int j = 0; j < 4; j++)
        line(mat_debug, rect_points[j], rect_points[(j + 1) % 4],
             Scalar(0, 255, 255), 1, 8);
    }

    // changed
    // rotation = 90 - abs(roi_angle);
    // rotation < m_angel;

    // m_angle=60
    if (roi_angle - m_angle < 0 && roi_angle + m_angle > 0) {
      Rect_<float> safeBoundRect;
      bool isFormRect = calcSafeRect(roi_rect, src, safeBoundRect);
      if (!isFormRect) continue;

      Mat bound_mat = src(safeBoundRect);
      Mat bound_mat_b = src_b(safeBoundRect);

      if (0) {
        imshow("bound_mat_b", bound_mat_b);
        waitKey(0);
        destroyWindow("bound_mat_b");
      }

      Point2f roi_ref_center = roi_rect.center - safeBoundRect.tl();

      Mat deskew_mat;
      if ((roi_angle - 5 < 0 && roi_angle + 5 > 0) || 90.0 == roi_angle ||
          -90.0 == roi_angle) {
        deskew_mat = bound_mat;
      } else {
        Mat rotated_mat;
        Mat rotated_mat_b;

        if (!rotation(bound_mat, rotated_mat, roi_rect_size, roi_ref_center, roi_angle))
          continue;

        if (!rotation(bound_mat_b, rotated_mat_b, roi_rect_size, roi_ref_center, roi_angle))
          continue;

        // we need affine for rotatioed image
        double roi_slope = 0;
        // imshow("1roated_mat",rotated_mat);
        // imshow("rotated_mat_b",rotated_mat_b);
        if (isdeflection(rotated_mat_b, roi_angle, roi_slope)) {
          affine(rotated_mat, deskew_mat, roi_slope);
        } else
          deskew_mat = rotated_mat;
      }

      Mat plate_mat;
      plate_mat.create(HEIGHT, WIDTH, TYPE);

      // haitungaga add，affect 25% to full recognition.
      if (useDeteleArea)
        deleteNotArea(deskew_mat, color);

      if (deskew_mat.cols * 1.0 / deskew_mat.rows > 2.3 && deskew_mat.cols * 1.0 / deskew_mat.rows < 6) {
        if (deskew_mat.cols >= WIDTH || deskew_mat.rows >= HEIGHT)
          resize(deskew_mat, plate_mat, plate_mat.size(), 0, 0, INTER_AREA);
        else
          resize(deskew_mat, plate_mat, plate_mat.size(), 0, 0, INTER_CUBIC);

        CPlate plate;
        plate.setPlatePos(roi_rect);
        plate.setPlateMat(plate_mat);
        if (color != UNKNOWN) plate.setPlateColor(color);
        outPlates.push_back(plate);
      }
    }
  }
  return 0;
}
