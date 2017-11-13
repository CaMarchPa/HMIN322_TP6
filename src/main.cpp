#include "CImg.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <stdlib.h>

using namespace cimg_library;

int log_2(int nb,int puissance){
        for(int i=0;i<puissance;i++)
            nb/=2;
        return nb;
}

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

    CImg<> tab_of_images[3][nbr_of_decomposition];
    tab_of_images[0][0] = img_Y;
    tab_of_images[1][0] = img_Cr;
    tab_of_images[2][0] = img_Cb;

    for (int i = 1; i < nbr_of_decomposition; i++) {
        tab_of_images[0][i] = CImg<>(width, height, 1, 1);
        tab_of_images[1][i] = CImg<>(width, height, 1, 1);
        tab_of_images[2][i] = CImg<>(width, height, 1, 1);
    }

    // Matrice des coefficients
    int matrix_size = height;
    int matrix_of_coef[matrix_size][matrix_size];
    int step = log_2(height, nbr_of_decomposition);
    float score = 0;
    for (int j = 0; j < step; ++j)
        for (int i = 0; i < step; ++i)
            matrix_of_coef[j][i] = 1;

    int half = height;

    for (int loop = 1; loop < nbr_of_decomposition; loop++) {
        height = half;
        half /= 2;
        for (int j = 0; j < matrix_size; j++)
            for (int i = 0; i < matrix_size; i++) {
                tab_of_images[0][loop](i,j) = tab_of_images[0][loop - 1](i, j);
                tab_of_images[1][loop](i,j) = tab_of_images[1][loop - 1](i, j);
                tab_of_images[2][loop](i,j) = tab_of_images[2][loop - 1](i, j);
            }
        //(top, left)
        for (int j = 0; j < height; j+=2)
            for (int i = 0; i < height; i+=2) {
                tab_of_images[0][loop](i/2, j/2) = ( tab_of_images[0][loop - 1](i, j) + tab_of_images[0][loop - 1](i, j + 1) + tab_of_images[0][loop - 1](i + 1, j) + tab_of_images[0][loop - 1](i + 1, j + 1) ) / 4;
                tab_of_images[1][loop](i/2, j/2) = ( tab_of_images[1][loop - 1](i, j) + tab_of_images[1][loop - 1](i, j + 1) + tab_of_images[1][loop - 1](i + 1, j) + tab_of_images[1][loop - 1](i + 1, j + 1) ) / 4;
                tab_of_images[2][loop](i/2, j/2) = ( tab_of_images[2][loop - 1](i, j) + tab_of_images[2][loop - 1](i, j + 1) + tab_of_images[2][loop - 1](i + 1, j) + tab_of_images[2][loop - 1](i + 1, j + 1) ) / 4;

            }

        //(top, right)
        for (int j = 0; j < height; j+=2)
            for (int i = 0; i < height; i+=2) {
                tab_of_images[0][loop](half + i / 2, j / 2) = (tab_of_images[0][loop - 1](i, j) - tab_of_images[0][loop - 1](i+1, j)) / 2 + (tab_of_images[0][loop - 1](i, j + 1) - tab_of_images[0][loop - 1](i + 1, j + 1)) / 2 + 128;
                tab_of_images[1][loop](half + i / 2, j / 2) = (tab_of_images[1][loop - 1](i, j) - tab_of_images[1][loop - 1](i+1, j)) / 2 + (tab_of_images[1][loop - 1](i, j + 1) - tab_of_images[1][loop - 1](i + 1, j + 1)) / 2 + 128;
                tab_of_images[2][loop](half + i / 2, j / 2) = (tab_of_images[2][loop - 1](i, j) - tab_of_images[2][loop - 1](i+1, j)) / 2 + (tab_of_images[2][loop - 1](i, j + 1) - tab_of_images[2][loop - 1](i + 1, j + 1)) / 2 + 128;

                if (tab_of_images[0][loop](half + i / 2, j / 2) > 128) {
                    tab_of_images[0][loop](half + i / 2, j / 2) = 128;
                }
                if (tab_of_images[1][loop](half + i / 2, j / 2) > 128) {
                    tab_of_images[1][loop](half + i / 2, j / 2) = 128;
                }
                if (tab_of_images[2][loop](half + i / 2, j / 2) > 128) {
                    tab_of_images[2][loop](half + i / 2, j / 2) = 128;
                }
                score = 0.8;
                score *= (float)(nbr_of_decomposition + 4 - loop) / (float) (nbr_of_decomposition + 4);
                matrix_of_coef[j / 2][half + i / 2] = score + 1;
            }

        //(bottom, left)
        for (int j = 0; j < height; j+=2)
            for (int i = 0; i < height; i+=2) {
                tab_of_images[0][loop](i / 2, half + j / 2) = ( tab_of_images[0][loop - 1](i, j) - tab_of_images[0][loop - 1](i + 1, j) ) / 2 + ( tab_of_images[0][loop - 1](i, j + 1) - tab_of_images[0][loop - 1](i + 1, j + 1) ) / 2 + 128;
                tab_of_images[1][loop](i / 2, half + j / 2) = ( tab_of_images[1][loop - 1](i, j) - tab_of_images[1][loop - 1](i + 1, j) ) / 2 + ( tab_of_images[1][loop - 1](i, j + 1) - tab_of_images[1][loop - 1](i + 1, j + 1) ) / 2 + 128;
                tab_of_images[2][loop](i / 2, half + j / 2) = ( tab_of_images[2][loop - 1](i, j) - tab_of_images[2][loop - 1](i + 1, j) ) / 2 + ( tab_of_images[2][loop - 1](i, j + 1) - tab_of_images[2][loop - 1](i + 1, j + 1) ) / 2 + 128;

                if (tab_of_images[0][loop](i / 2, half + j / 2) > 128) {
                    tab_of_images[0][loop](i / 2, half + j / 2) = 128;
                }
                if (tab_of_images[1][loop](i / 2, half + j / 2) > 128) {
                    tab_of_images[1][loop](i / 2, half + j / 2) = 128;
                }
                if (tab_of_images[2][loop](i / 2, half + j / 2) > 128) {
                    tab_of_images[2][loop](i / 2, half + j / 2) = 128;
                }
                score = 0.8;
                score *= (float)(nbr_of_decomposition + 4 - loop) / (float)(nbr_of_decomposition + 4);
                matrix_of_coef[half + j / 2][i / 2] = score + 1;
            }

        //(bottom, right)
        for (int j = 0; j < height; j+=2)
            for (int i = 0; i < height; i+=2) {
                tab_of_images[0][loop](half + i / 2, half + j / 2) = (tab_of_images[0][loop - 1](i, j) - tab_of_images[0][loop - 1](i, j + 1)) - (tab_of_images[0][loop - 1](i + 1, j) - tab_of_images[0][loop - 1](i + 1, j + 1)) / 2 + 128;
                tab_of_images[1][loop](half + i / 2, half + j / 2) = (tab_of_images[1][loop - 1](i, j) - tab_of_images[1][loop - 1](i, j + 1)) - (tab_of_images[1][loop - 1](i + 1, j) - tab_of_images[1][loop - 1](i + 1, j + 1)) / 2 + 128;
                tab_of_images[2][loop](half + i / 2, half + j / 2) = (tab_of_images[2][loop - 1](i, j) - tab_of_images[2][loop - 1](i, j + 1)) - (tab_of_images[2][loop - 1](i + 1, j) - tab_of_images[2][loop - 1](i + 1, j + 1)) / 2 + 128;

                if (tab_of_images[0][loop](half + i / 2, half + j / 2) > 128) {
                    tab_of_images[0][loop](half + i / 2, half + j / 2) = 128;
                }
                if (tab_of_images[1][loop](half + i / 2, half + j / 2) > 128) {
                    tab_of_images[1][loop](half + i / 2, half + j / 2) = 128;
                }
                if (tab_of_images[2][loop](half + i / 2, half + j / 2) > 128) {
                    tab_of_images[2][loop](half + i / 2, half + j / 2) = 128;
                }
                score = 0.8;
                score *= (float)(nbr_of_decomposition + 4 - loop) / (float)(nbr_of_decomposition + 4);
                matrix_of_coef[half + j / 2][half + i / 2] = score + 1;
            }
    }
    int compt = 0;
    for (int j = 0; j < matrix_size; j++) {
        for (int i = 0; i < matrix_size; i++) {
            if (matrix_of_coef[j][i] == 0)
                matrix_of_coef[j][i] = 1;
            tab_of_images[0][nbr_of_decomposition-1](i, j) /= matrix_of_coef[j][i];
            tab_of_images[1][nbr_of_decomposition-1](i, j) /= matrix_of_coef[j][i];
            tab_of_images[2][nbr_of_decomposition-1](i, j) /= matrix_of_coef[j][i];
            if (tab_of_images[0][nbr_of_decomposition-1](i, j) < 10)
                compt++;
        }
    }
    //
    char result_Y[20] = "../img/result_Y.png";
    char result_Cr[25] = "../img/result_Cr.png";
    char result_Cb[25] = "../img/result_Cb.png";
    //
    tab_of_images[0][nbr_of_decomposition - 1].save_png(result_Y);
    tab_of_images[1][nbr_of_decomposition - 1].save_png(result_Cr);
    tab_of_images[2][nbr_of_decomposition - 1].save_png(result_Cb);
    //
    CImg<> tab_of_images_rec[3][nbr_of_decomposition];
    tab_of_images_rec[0][nbr_of_decomposition - 1] = tab_of_images[0][nbr_of_decomposition - 1];
    tab_of_images_rec[1][nbr_of_decomposition - 1] = tab_of_images[1][nbr_of_decomposition - 1];
    tab_of_images_rec[2][nbr_of_decomposition - 1] = tab_of_images[2][nbr_of_decomposition - 1];
    //
    for (int i = nbr_of_decomposition - 2; i >= 0; i--) {
        tab_of_images_rec[0][i] = CImg<>(width, matrix_size, 1, 1);
        tab_of_images_rec[1][i] = CImg<>(width, matrix_size, 1, 1);
        tab_of_images_rec[2][i] = CImg<>(width, matrix_size, 1, 1);
    }

    for (int j = 0; j < matrix_size; j++) {
        for (int i = 0; i < matrix_size; i++) {
            tab_of_images_rec[0][nbr_of_decomposition - 1](i, j) *= matrix_of_coef[j][i];
            tab_of_images_rec[1][nbr_of_decomposition - 1](i, j) *= matrix_of_coef[j][i];
            tab_of_images_rec[2][nbr_of_decomposition - 1](i, j) *= matrix_of_coef[j][i];
        }
    }

    //
    for (int loop = nbr_of_decomposition - 2; loop >= 0; loop--) {
        height = half;
        half *= 2;
        std::cout << "height : " << height << ", half : " << half << '\n';
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < height; i++) {
                tab_of_images_rec[0][loop](2*i, 2*j) = tab_of_images_rec[0][loop + 1](i, j);
                tab_of_images_rec[0][loop](2*i, 2*j+1) = tab_of_images_rec[0][loop + 1](i, j) + tab_of_images_rec[0][loop + 1](i + half, j);
                tab_of_images_rec[0][loop](2*i+1, 2*j) = tab_of_images_rec[0][loop + 1](i, j) + tab_of_images_rec[0][loop + 1](i, j + half);
                tab_of_images_rec[0][loop](2*i+1, 2*j+1) = tab_of_images_rec[0][loop + 1](i, j) + tab_of_images_rec[0][loop + 1](i + half, j + half);
                //Cr
                tab_of_images_rec[1][loop](2*i, 2*j) = tab_of_images_rec[1][loop + 1](i, j);
                tab_of_images_rec[1][loop](2*i, 2*j+1) = tab_of_images_rec[1][loop + 1](i, j) + tab_of_images_rec[1][loop + 1](i + half, j) ;
                tab_of_images_rec[1][loop](2*i+1, 2*j) = tab_of_images_rec[1][loop + 1](i, j) + tab_of_images_rec[1][loop + 1](i, j + half) ;
                tab_of_images_rec[1][loop](2*i+1, 2*j+1) = tab_of_images_rec[1][loop + 1](i, j) + tab_of_images_rec[1][loop + 1](i + half, j + half);
                //Cb
                tab_of_images_rec[2][loop](2*i, 2*j) = tab_of_images_rec[2][loop + 1](i, j);
                tab_of_images_rec[2][loop](2*i, 2*j+1) = tab_of_images_rec[2][loop + 1](i, j) + tab_of_images_rec[2][loop + 1](i + half, j);
                tab_of_images_rec[2][loop](2*i+1, 2*j) = tab_of_images_rec[2][loop + 1](i, j) + tab_of_images_rec[2][loop + 1](i, j + half);
                tab_of_images_rec[2][loop](2*i+1, 2*j+1) = tab_of_images_rec[2][loop + 1](i, j) + tab_of_images_rec[2][loop + 1](i + half, j + half);
            }
        }
    }
    //
    char result_f[30] = "../img/result_rgb.png";
    // // RECONSTRUCTION
    CImg<> result_rec_img(matrix_size, matrix_size, 1, 3);
    for (int j = 0; j < matrix_size; j++) {
        for (int i = 0; i < matrix_size; i++) {
            result_rec_img(i, j, 0, 0) = tab_of_images_rec[0][0](i, j);// + 1.402 * (tab_of_images_rec[1][0](i, j) - 128);
            result_rec_img(i, j, 0, 1) = tab_of_images_rec[1][0](i, j);// - 0.34414 * (tab_of_images_rec[2][0](i, j) - 128) - 0.71414 * (tab_of_images_rec[1][0](i, j) - 128);
            result_rec_img(i, j, 0, 2) = tab_of_images_rec[2][0](i, j);// + 1.772 * (tab_of_images_rec[2][0](i, j) - 128);
        }
    }
    //
    result_rec_img = result_rec_img.get_RGBtoYCbCr();
    result_rec_img.save_png(result_f);
    double psnr = img_read.PSNR(result_rec_img);
    std::cout << "PSNR = " << psnr << '\n';
    std::cout << " E N D " << std::endl;
    return 0;
}
