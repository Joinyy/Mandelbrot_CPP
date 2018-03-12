#include "../hdr/mandelbrot.hpp"
#include "../hdr/helper_functions.hpp"

sf::Uint8 *arr;

void mandelThread(int start, int end, double* im_min, double* im_max, double* re_min, double* re_max, double* max_betrag, int max_iter);
int calculatePoint(double x, double y, double xadd, double yadd, double max_betrag_2, int max_iter);

void mandelMain(double x_cent, double y_cent, double zoom, double max_betrag, int xpixels, int ypixels, int max_iter) {
	double re_min = x_cent - (zoom / 2);
	double re_max = x_cent + (zoom / 2);

	double im_min = y_cent - ((zoom / 2) / xpixels)*ypixels;
	double im_max = y_cent + ((zoom / 2) / xpixels)*ypixels;

	std::vector<std::thread> ThreadPool(THREADCOUNT);
	int start = 0, end = ypixels / THREADCOUNT;
	for (auto& Thr : ThreadPool) {
		Thr = std::thread(mandelThread, start, end, &im_min, &im_max, &re_min, &re_max, &max_betrag, max_iter);
		start += ypixels / THREADCOUNT;
		end += ypixels / THREADCOUNT;
	}

	for (auto & Thr : ThreadPool) {
		Thr.join();
	}
}

void mandelThread(int start, int end, double* im_min, double* im_max, double* re_min, double* re_max, double* max_betrag, int max_iter) {
	double c_im = 0, c_re = 0;
	int iterationen = 0;
	color PointColor;
	for (int y = start; y < end; y++) {
		c_im = *im_min + (*im_max - *im_min)*y / YW;

		for (std::size_t x = 0; x < XW; x++) {
			c_re = *re_min + (*re_max - *re_min)*x / XW;

			iterationen = calculatePoint(c_re, c_im, c_re, c_im, *max_betrag, max_iter);

			PointColor = ColorMap[(int)floor(abs((iterationen / (max_iter*1.0))*255.0))];
			arr[(x + (YW - 1 - y) * XW) * 4]     = PointColor.R;
			arr[(x + (YW - 1 - y) * XW) * 4 + 1] = PointColor.G;
			arr[(x + (YW - 1 - y) * XW) * 4 + 2] = PointColor.B;
			arr[(x + (YW - 1 - y) * XW) * 4 + 3] = 255;

		}

	}
}


int calculatePoint(double x, double y, double xadd, double yadd, double max_betrag_2, int max_iter) {
	int remain_iter = max_iter;
	double xx = x*x;
	double yy = y*y;
	double xy = x*y;
	double betrag_2 = xx + yy;

	while ((betrag_2 <= max_betrag_2) && (remain_iter > 0)) {
		remain_iter = remain_iter - 1;
		x = xx - yy + xadd;
		y = xy + xy + yadd;
		xx = x*x;
		yy = y*y;
		xy = x*y;
		betrag_2 = xx + yy;
	}

	return max_iter - remain_iter;
}
