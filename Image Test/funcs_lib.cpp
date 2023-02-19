#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

extern "C" uint8_t * toGray(const uint8_t * matrix, uint32_t n, uint32_t m)
{
	uint8_t * results = (uint8_t *) malloc(sizeof(uint8_t) * n * m);
	
	for (int y = 0; y < n; y++) {
		for (int x = 0; x < m; x++) {
			int b = matrix[3 * y * m + 3 * x + 0];
			int g = matrix[3 * y * m + 3 * x + 1];
			int r = matrix[3 * y * m + 3 * x + 2];
			results[y * m + x] = (uint8_t)((b + g + r) / 3);
		}
	}

	return results;
}

extern "C" uint8_t * balanceColors (const uint8_t * matrix, uint32_t n, uint32_t m, float blue, float green, float red)
{
	uint8_t * results = (uint8_t *) malloc(sizeof(uint8_t) * n * m * 3);	// 3 for rgb
	
	for (int y = 0; y < n; y++) {
		for (int x = 0; x < m; x++) {
			int b = matrix[3 * y * m + 3 * x + 0] * blue;
			int g = matrix[3 * y * m + 3 * x + 1] * green;
			int r = matrix[3 * y * m + 3 * x + 2] * red;
			results[3 * y * m + 3 * x + 0] = (uint8_t)(MIN(b, 255));
			results[3 * y * m + 3 * x + 1] = (uint8_t)(MIN(g, 255));
			results[3 * y * m + 3 * x + 2] = (uint8_t)(MIN(r, 255));
		}
	}

	return results;
}

const uint img_size = 10;

const uint8_t img[] = {
	255, 255, 255, 255, 255,   0,   0,   0,   0,   0,
	255, 255, 255, 255, 255,   0,   0,   0,   0,   0,
	255, 255, 255, 255, 255,   0,   0,   0,   0,   0,
	255, 255, 255, 255, 255,   0,   0,   0,   0,   0,
	255, 255, 255, 255, 255,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0, 255, 255, 255, 255, 255,
	  0,   0,   0,   0,   0, 255, 255, 255, 255, 255,
	  0,   0,   0,   0,   0, 255, 255, 255, 255, 255,
	  0,   0,   0,   0,   0, 255, 255, 255, 255, 255,
	  0,   0,   0,   0,   0, 255, 255, 255, 255, 255,
};

extern "C" uint8_t * findPic (const uint8_t * matrix, uint n, uint m)
{
	uint * scores = (uint *) malloc (sizeof(uint) * (n - img_size) * (m - img_size));
	uint8_t * results = (uint8_t *) malloc(sizeof(uint8_t) * (n - img_size) * (m - img_size));
	uint max_score = 0;

	for (int y = 0; y < n - img_size; y++) {
		for (int x = 0; x < m - img_size; x++) {
			uint score = 0;
			for (int yi = 0; yi < img_size; yi++) {
				for (int xi = 0; xi < img_size; xi++) {
					score += pow((int)matrix[(y + yi) * m + (x + xi)] - img[yi * img_size + xi], 2);
				}
			}
			max_score = MAX(score, max_score);
			scores[y * (m - img_size) + x] = score;
		}
	}

	float scale = 255.0f / max_score;

	for (int y = 0; y < n - img_size; y++) {
		for (int x = 0; x < m - img_size; x++) {
			results[y * (m - img_size) + x] = (uint8_t)(scores[y * (m - img_size) + x] * scale);
		}
	}

	free(scores);
	return results;
}

