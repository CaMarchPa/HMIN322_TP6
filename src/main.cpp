#include "CImg.h"
#include <iostream>
#include <sstream>
#include <vector>

using namespace cimg_library;

int main(int argc, char const *argv[]) {
    CImg<float> img_read;

    int nbr_of_decomposition; // Nombre de décomposition
    int quality; // La qualité des images à décomposer

    if (argc != 3) {
		std::cout << "USAGE : ./main image.extension nbr_of_decomposition" << std::endl;
		return 1;
	}
    img_read.load(argv[1]);
    int height = img_read.height();
    int width = img_read.width();
    std::stringstream convert(argv[2]);
    convert >> nbr_of_decomposition;

    //Changement d'espace de couleur RGB => YCrCb
    CImg<> img_Y(width, height, 1, 1);
    CImg<> img_Cb(width, height, 1, 1);
    CImg<> img_Cr(width, height, 1, 1);

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            img_Y(i, j, 0) = 0.3 * img_read(i, j, 0, 0) + 0.6 * img_read(i, j, 0, 1) + 0.1 * img_read(i, j, 2);
            img_Cb(i, j, 0) = -0.1687 * img_read(i, j, 0, 0) - 0.3313 * img_read(i, j, 0, 1) + 0.5 * img_read(i, j, 0, 2) + 128;
            img_Cr(i, j, 0) = 0.5 * img_read(i, j, 0, 0) - 0.4187 * img_read(i, j, 0, 1) - 0.0813 * img_read(i, j, 0, 2) + 128;
        }
    }
    //Sauvegarde des images 
    char Y[30] = "../img/image_Y.png", Cb[30] = "../img/image_Cb.png", Cr[30] = "../img/image_Cr.png";
    img_Y.save_png(Y);
    img_Cb.save_png(Cb);
    img_Cr.save_png(Cr);

    CImg<> tab_of_images[4];
    tab_of_images[0] = img_Y;
    tab_of_images[1] = CImg<>(width, height, 1, 1);
    tab_of_images[2] = CImg<>(width, height, 1, 1);
    tab_of_images[3] = CImg<>(width, height, 1, 1);

    //Matrice des coefficients
    int matrix_size = height;
    std::vector< std::vector<int> > matrix_of_coef(matrix_size);
    int step = log(height) / log(nbr_of_decomposition);
    float score = 0;

    for (int j = 0; j < step; ++j)
        for (int i = 0; i < step; ++i)
            matrix_of_coef[j][i] = 1;

    int half = height;
    for (int loop = 0; loop < nbr_of_decomposition; loop++)
    {
        height = half;
        half /= 2;
        for (int j = 0; j < matrix_size; j++)
            for (int i = 0; i < matrix_size; i++)
                tab_of_images[loop](i,j) = tab_of_images[loop-1](i, j);

        //(left, top)
        //(top, right)
        //(left, bottom)
        //(right, bottom)
    }

    return 0;
}