#include "ImageBase.h"
#include <stdio.h>
#include <iostream>
#include <math.h>
using namespace std;

int log2(int nb,int puissance){
        for(int i=0;i<puissance;i++)
            nb/=2;
        return nb;
}

int main(int argc, char **argv)
{
    char cNomImgLue[250], cNomImgEcrite[250];

    int S1,S2,S3;
    int H=0,W=0;
  
    if (argc < 4)
    {
        printf("Usage: ImageIn.ppm nbDecomp qualite entre 1  et 100\n");
        return 1;
    }
    int NB=atoi(argv[2]);
    int qualite = atoi(argv[3]);
    qualite+=(100-qualite)*(double)((double)3/(double)5);
    sscanf (argv[1],"%s",cNomImgLue);
    
    ImageBase imIn;
    imIn.load(cNomImgLue);
    H=imIn.getWidth();
    W=imIn.getHeight();
    ImageBase imOutY(W,H , !imIn.getColor());
    ImageBase imOutCr(W, H, !imIn.getColor());
    ImageBase imOutCb(W, H, !imIn.getColor());
    
    //transformation
    for(int y = 0; y < H ; y++)
        for(int x = 0; x < W ; x++)
        {
            imOutY[y][x] = 0.299*imIn[y*3][x*3+0]+0.587*imIn[y*3][x*3+1]+0.114*imIn[y*3][x*3+2]; // Y
            imOutCb[y][x] = -0.1687*imIn[y*3][x*3+0]-0.3313*imIn[y*3][x*3+1]+0.5*imIn[y*3][x*3+2]+128; // Cb
            imOutCr[y][x] = 0.5*imIn[y*3][x*3+0]-0.4187*imIn[y*3][x*3+1]-0.0813*imIn[y*3][x*3+2]+128; // Cr
        }
        
    // imOutY.save("../img/test/Image_Y.pgm");
    // imOutCr.save("../img/test/Image_Cr.pgm");
    // imOutCb.save("../img/test/Image_Cb.pgm");

    int X=H;
    int mTaille=H;
    ImageBase newY(W,H , !imIn.getColor());
    ImageBase newY2(W,H , !imIn.getColor());
    ImageBase newY3(W,H , !imIn.getColor());

    // ImageBase tabImages[4] = {ImageBase(W, H, !imIn.getColor()), ImageBase(W,H , !imIn.getColor()), ImageBase(W,H , !imIn.getColor()), ImageBase(W,H , !imIn.getColor())};
    ImageBase img_tmp[3] = {ImageBase(W, H, !imIn.getColor()), ImageBase(W, H, !imIn.getColor()), ImageBase(W, H, !imIn.getColor())};
    ImageBase img_res[3] = {ImageBase(W, H, !imIn.getColor()), ImageBase(W, H, !imIn.getColor()), ImageBase(W, H, !imIn.getColor())};

    // for (int i = 1; i < NB; ++i)
    // {
    //     tabImages[i] = ImageBase(W, H, !imIn.getColor());
    // }
    /*tabImages[0] = ImageBase(H, H, !imIn.getColor());*/
    for (int i = 0; i < H; i++)
        for (int j = 0; j < H; j++) {
            // tabImages[0][i][j] = imOutY[i][j];
            img_tmp[0][i][j] = imOutY[i][j];
            img_tmp[1][i][j] = imOutCr[i][j];
            img_tmp[2][i][j] = imOutCb[i][j];
        }
    //tabImages[1]=newY;
    //tabImages[2]=newY2;
    //tabImages[3]=newY3;
    
    ///matrice des coefficients
    int coefs[mTaille][mTaille];
    int pas=log2(mTaille,NB);
    int nouveauPas;
    // remplir les coefficients
    for(int i=0 ; i < pas;i++)
            for(int j=0 ; j < pas;j++)
                coefs[i][j]=1;

    


    for(int tourBoucle=1;tourBoucle<NB;tourBoucle++){    
    H=X;
    X/=2;
    int ip,jp;
    double coefi,coefj;
    double score;
    double tmp;
    double qual=(double)qualite;
    double div = 0.8;
    
    for(int i=0 ; i < mTaille ; i++)
        for(int j=0 ; j < mTaille ; j++){
            // tabImages[tourBoucle][i][j] = tabImages[tourBoucle-1][i][j];
            img_res[0][i][j] = img_tmp[0][i][j];
            img_res[1][i][j] = img_tmp[1][i][j];
            img_res[2][i][j] = img_tmp[2][i][j];
        }

    //en haut a gauche
    for(int i=0 ; i < H ; i+=2){
        for(int j=0 ; j < H ; j+=2){
            // if (j % 2 == 0){
                // tabImages[tourBoucle][i/2][j/2]=(tabImages[tourBoucle-1][i][j]+tabImages[tourBoucle-1][i+1][j]+tabImages[tourBoucle-1][i][j+1]+tabImages[tourBoucle-1][i+1][j+1])/4;
                img_res[0][i/2][j/2] = (img_tmp[0][i][j] + img_tmp[0][i+1][j] + img_tmp[0][i][j+1] + img_tmp[0][i+1][j+1]) / 4;
                img_res[1][i/2][j/2] = (img_tmp[1][i][j] + img_tmp[1][i+1][j] + img_tmp[1][i][j+1] + img_tmp[1][i+1][j+1]) / 4;
                img_res[2][i/2][j/2] = (img_tmp[2][i][j] + img_tmp[2][i+1][j] + img_tmp[2][i][j+1] + img_tmp[2][i+1][j+1]) / 4;
            // }
            // else{
            //     // tabImages[tourBoucle][i/2][j/2]=(tabImages[tourBoucle-1][i][j]+tabImages[tourBoucle-1][i+1][j])/16;
            //     img_res[i/2][j/2] = (img_tmp[i][j] + img_tmp[i+1][j]) / 16;
            // }
        }
    }

    //en haut a droite
    for(int i=0 ; i < H ; i+=2){
        for(int j=0 ; j < H ; j+=2){
            // tabImages[tourBoucle][i/2][j/2+X]=((tabImages[tourBoucle-1][i][j]-tabImages[tourBoucle-1][i][j+1])/2)+
            //                  ((tabImages[tourBoucle-1][i+1][j]-tabImages[tourBoucle-1][i+1][j+1])/2)+128;
            img_res[0][i/2][j/2+X] = (img_tmp[0][i][j] - img_tmp[0][i][j+1]) / 2 + (img_tmp[0][i+1][j] - img_tmp[0][i+1][j+1]) / 2 + 128;
            img_res[1][i/2][j/2+X] = (img_tmp[1][i][j] - img_tmp[1][i][j+1]) / 2 + (img_tmp[1][i+1][j] - img_tmp[1][i+1][j+1]) / 2 + 128;
            img_res[2][i/2][j/2+X] = (img_tmp[2][i][j] - img_tmp[2][i][j+1]) / 2 + (img_tmp[2][i+1][j] - img_tmp[2][i+1][j+1]) / 2 + 128;
            if(img_res[0][i/2][j/2+X] >128) {
                // tabImages[tourBoucle][i/2][j/2+X]=128;
                img_res[0][i/2][j/2+X] = 128;
            }
            if(img_res[0][i/2][j/2+X] >128) {
                // tabImages[tourBoucle][i/2][j/2+X]=128;
                img_res[0][i/2][j/2+X] = 128;
            }
            if(img_res[0][i/2][j/2+X] >128) {
                // tabImages[tourBoucle][i/2][j/2+X]=128;
                img_res[0][i/2][j/2+X] = 128;
            }
            //plus le score est petit plus on veut un coefficient petit

            score= div ;
            //de plus plus on est loin dans la decomp plus on veut un score petit
            score= score*(double)((double)(NB+4-tourBoucle)/(double)(NB+4));
           // score=score - (score * (qualite /100) ) ;

            coefs[i/2][j/2+X]=(110*(1-qual/100))*score+1;
            
    }}

    //en bas a gauche
    for(int i=0 ; i < H ; i+=2){
        for(int j=0 ; j < H ; j+=2){
            // tabImages[tourBoucle][i/2+X][j/2]=((tabImages[tourBoucle-1][i][j]-tabImages[tourBoucle-1][i+1][j])/2)+
            //                  ((tabImages[tourBoucle-1][i+1][j]-tabImages[tourBoucle-1][i+1][j+1])/2)+128;
            img_res[i/2+X][j/2] = (img_tmp[i][j] - img_tmp[i+1][j]) / 2 + (img_tmp[i][j+1] - img_tmp[i+1][j+1]) / 2 + 128; 
            if(img_res[i/2+X][j/2]>128) {
                // tabImages[tourBoucle][i/2+X][j/2]=128;
                img_res[i/2+X][j/2] = 128;
            }
        
            //plus le score est petit plus on veut un coefficient petit

            score= div ;
            //de plus plus on est loin dans la decomp plus on veut un score petit
            score= score*(double)((double)(NB+4-tourBoucle)/(double)(NB+4));
          //  score=score - (score * (qualite /100) ) ;
            coefs[i/2+X][j/2]=(110*(1-qual/100))*score+1;
   
    }}

    //en bas a droite
    for(int i=0 ; i < H ; i+=2){
        for(int j=0 ; j < H ; j+=2){
            // tabImages[tourBoucle][i/2+X][j/2+X]=((tabImages[tourBoucle-1][i][j]-tabImages[tourBoucle-1][i][j+1]))-
            //                  ((tabImages[tourBoucle-1][i+1][j]-tabImages[tourBoucle-1][i+1][j+1]))+128;
            img_res[i/2+X][j/2+X] = (img_tmp[i][j] - img_tmp[i][j+1]) - (img_tmp[i+1][j] - img_tmp[i+1][j+1]) + 128;  
            if(img_res[i/2+X][j/2+X] >128) { 
                // tabImages[tourBoucle][i/2+X][j/2+X]=128;
                img_res[i/2+X][j/2+X] = 128;
            }
            //plus le score est petit plus on veut un coefficient petit

            score= div ;
            //de plus plus on est loin dans la decomp plus on veut un score petit
            score= score*(double)((double)(NB+4-tourBoucle)/(double)(NB+4));
           // score=score - (score * (qualite /100) ) ;
            coefs[i/2+X][j/2+X]=(110*(1-qual/100))*score+1;
        }
    }
    img_tmp.copy(img_res);
    }

    int compteur=0;
    for(int i=0 ; i < mTaille-1;i++){
        for(int j=0 ; j < mTaille-1;j++){
                       // printf("TEST %d %d %d\n",coefs[i][j],i,j);
            if(coefs[i][j] == 0) {coefs[i][j] = 1;
                img_res[i][j]/=coefs[i][j];}
            if(img_res[i][j] <10)
                compteur++;
                //printf("TEST %d %d %d\n",tabImages[NB][i][j],i,j);

        }
    }
    printf("compteur :%lf\n",(double)compteur/(mTaille*mTaille));
    // tabImages[1].save("../img/test/resultat1.pgm");
    // tabImages[2].save("../img/test/resultat2.pgm");

    char char_res[40] = "../img/test/resultat.pgm";
    img_res.save(char_res);

    return 0;

}
