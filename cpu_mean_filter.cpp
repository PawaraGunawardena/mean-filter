
#include "stb_image.h"
#include "stb_image_write.h"

//using namespace std;

static void compute_average(unsigned char* destination, unsigned char* source, int width, int height, int kernal_radius)
{
    const unsigned int elements = ((2 * kernal_radius) + 1) * ((2 * kernal_radius) + 1);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            unsigned int sum = 0;
            for (int ker_y = -kernal_radius; ker_y <= kernal_radius; ker_y++) {
                const int cur_y = y + ker_y;
                if (cur_y < 0 || cur_y > height) {
                    continue;
                }

                for (int ker_x = -kernal_radius; ker_x <= kernal_radius; ker_x++) {
                    const int cur_x = x + ker_x;
                    if (cur_x < 0 || cur_x > width) {
                        continue;
                    }

                    const int cur_position = (cur_y * width + cur_x);
                    if (cur_position >= 0 && cur_position < (width * height)) {
                        sum += source[cur_position];
                    }
                }
            }
            destination[y * width + x] = (unsigned char) (sum / elements);
        }
    }
}

void average_cpu(unsigned char* input_image, unsigned char* output_image, int width, int height, int num_channels, int kernal_radius)
{
    unsigned char*  input_channel = (unsigned char*)malloc(width * height * sizeof(unsigned char));
    unsigned char*  output_channel = (unsigned char*)malloc(width * height * sizeof(unsigned char));
    int cur_channel;

    for (cur_channel = 0; cur_channel < num_channels; cur_channel++) {
	    int i = 0;
	    for (i = 0; i < (width * height); i++) {
	        input_channel[i] = input_image[num_channels * i + cur_channel];
	    }

        compute_average(output_channel, input_channel, width, height, kernal_radius);

	    int j = 0;
	    for (j = 0; j < (width * height); j++) {
	        output_image[num_channels * j + cur_channel] = (output_channel[j]);
	    }
    }
    free(input_channel);
    free(output_channel);
}

int main(int argc, char **argv)
{
	unsigned char* image = NULL;
    unsigned char* image_result = NULL;

    int width = -1;
    int height = -1;
    int channels = 3;
    int kernal_radius = 8;

    char *source_path = NULL;
    char *destination_path = NULL;

    int comp = -1;

    source_path = argv[1];
    destination_path = argv[2];
    image = stbi_load(source_path, &width, &height, &comp, STBI_rgb);

    const int num_runs = 3;


    image_result = (unsigned char*)malloc(width * height * channels * sizeof(unsigned char));

    int i;
    for (i = 0; i < num_runs; i++) {
        average_cpu(image, image_result, width, height, channels, kernal_radius);
    }

    stbi_write_jpg(destination_path, width, height, comp, image_result, 90);

    free(image);
    free(image_result);

    return 0;
}
