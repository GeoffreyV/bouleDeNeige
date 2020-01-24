#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "libClient.h"
#include "FonctionsBDN.h"


void CalculParamTrajectoire(int xmoi, int xadversaire, int ymur, int yadversaire,double*energie,int masseboule, double*alpha){
    int alphatest=0;
    xmoi = xmoi + 50;
    double alpharad=0;
    double Vx= 0.0;
    double V0= 0.0;
    int ycontrole=0;
    int xMur=512;
    int k=1866605;
    int g=1962;
    int margeMur = 40;
    int incrementAngle = 1;

    int degressif = 0;
    if (xadversaire < 680)
    {
        degressif = 1;
        alphatest = 80;
    }

    while(alphatest<90)
    {
    alpharad=(alphatest/360.0)*2*3.14;
    Vx=sqrt((-g/2.0)*pow(xadversaire-xmoi,2)*1.0/((yadversaire-225)-tan(alpharad)*(xadversaire-xmoi)));
    V0=Vx/cos(alpharad);
    *energie=(pow(V0,2)/(2.0*k))*masseboule;
    ycontrole=(-g/2.0)*pow(xMur-xmoi,2)*(1/pow(Vx,2))+tan(alpharad)*(xMur-xmoi)+225;

    if (*energie<=100 && ycontrole-(ymur+margeMur)>0)
        {
        *alpha=alphatest;
        printf("%0.2lf    %0.2f     %0.2f     %0.2lf     %d    %d\n",*alpha,Vx,V0,*energie,(xadversaire-xmoi),ycontrole);
        return;
        }
    else
        {
            if (degressif)
                alphatest=alphatest-incrementAngle;
            else
                alphatest=alphatest+incrementAngle;
        }
    }
    printf("Pas de solutions trouvées pour le lancé\n");

}

double correctionAngle(int xVent, int yVent, int gain)
{
    return -gain * yVent;
}

double correctionEnergie(int xVent, int yVent, int gain)
{
    return -gain * xVent;
}

int CalculPassageBoule(int xmoi, int xboule, int yboule, int Vxboule, int Vyboule){
    double alpharad;
    //double Vx= 0.0;
    //double V0= 0.0;
    int ycontrole=0;
    int xMur=512 -10;
    //int k=1866605;
    int g=1962;
    alpharad=(atan(Vyboule/Vxboule)/360.0)*2*3.14;
    ycontrole=(-g/2.0)*pow(xMur-xmoi,2)*(1/pow(Vxboule,2))+tan(alpharad)*(xMur-xmoi)+yboule;
    return ycontrole;
}
#define CLIENT_DECONNECTE         -1
#define CLIENT_CONNECTE            0
