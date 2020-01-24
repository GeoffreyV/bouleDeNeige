#ifndef _FonctionsBDN_H
#define _FonctionsBDN_H

void CalculParamTrajectoire(int xmoi, int xadversaire, int ymur, int yadversaire,double*energie,int masseboule,double*alpha);
double correctionAngle(int xVent, int yVent, int gain);
double correctionEnergie(int xVent, int yVent,int gain);

int CalculPassageBoule(int xmoi, int xboule, int yboule, int Vxboule, int Vyboule);
int Danger(void);
#endif
