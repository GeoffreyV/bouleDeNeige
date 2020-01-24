#include <stdio.h>
#include <stdlib.h>

#include "libClient.h"
#include "FonctionsBDN.h"
#include "time.h"






int main (int argc, char **argv)
{
    Jeu jeu;
    Moi moi;

    time_t timer1, timer2;
    double tempsAvSeRelever = 2.0;
    double tempsAdversaireCache = 3.0;
    Adversaire adversaire;
    int nbBoules;
    Boule boule[BOULES_NB_MAX];
    int etatAdversaire = 0;
    enum States {
    ATTENTE = 0,
    SACCROUPI = 1,
    RASSEMBLE = 2,
    COMPACTE = 4,
    SERELEVE = 8,
    LANCE = 16,
    AVANCE = 32,
    RECULE = 64,
    AVANCE_MUR = 128,
    ATTENTE_SE_RELEVE = 256,
    ATTENTE_SAFE_BEFORE_ME_RELEVE = 512,
    } etat = AVANCE_MUR;

    double energie =110, angle = 15;
    int hauteurAFrapper = 190;
    int neigeACompacter = 50;

    int hauteurAccroupi = 120;
    int hauteurDebout = 180;
    int hauteurBonnet = 200;

    // adresse IP du serveur sous forme de chaine de caracteres
    char adresse[255] = "127.0.0.1";//"172.26.0.6"; // 172.26.0.6
    // numero du port du serveur
    int port = 1050;

    if (serveurConnecter(adresse, port) != SERVEUR_CONNECTE)
    {
        printf("Serveur introuvable %s:%d\n", adresse, port);
        exit(-1);
    }
    else
    {
        printf("Connexion au serveur %s:%d\n", adresse, port);
    }

    serveurUpload(IMAGE_DEBOUT, "../images/debout.png");
    serveurUpload(IMAGE_MARCHE, "../images/marche.png");
    serveurUpload(IMAGE_ACCROUPI, "../images/accroupi.png");
    serveurUpload(IMAGE_LANCE, "../images/lance.png");
    serveurUpload(IMAGE_BONNET_PROFIL, "../images/bonnet_profil.png");
    serveurUpload(IMAGE_VICTOIRE, "../images/victoire.png");
    serveurUpload(IMAGE_DEFAITE, "../images/defaite.png");
    serveurUpload(IMAGE_BONNET_FACE, "../images/bonnet_face.png");

    serveurNomRobot("Gilbert");
    double gainAngle = 0.001;
    double gainEnergie = 0.001;
    serveurCaracRobot(5, 3, 2, 0);

    serveurDemarrerMatch();

    while (1)
    {
        serveurRecevoirSituation(&jeu, &moi, &adversaire, &nbBoules, boule);
        if (!(jeu.chrono % 100))
        {
            printf("%d     %d     %d    %d      %4d     %d  %d\n",moi.x,adversaire.x,etat,nbBoules, adversaire.blackOut, neigeACompacter, etatAdversaire);
        }

        if (adversaire.blackOut > 1200 * 0.77 )
            neigeACompacter = 20;
        else if (moi.etat != ROBOT_COMPACTE_BOULE)
            neigeACompacter = 50;

        // MACHINES A ETATS
// MACHINES A ETATS
        switch (etatAdversaire){
        case 0 : // Adversaire ne se cache pas
            if (adversaire.etat == ROBOT_ACCROUPI)
            {
                etatAdversaire = 1;
                time(&timer2);
            }
            break;
        case 1 : // Se cache-t-il ?
            if (adversaire.etat != ROBOT_ACCROUPI)
                etatAdversaire = 0;
            else if (time(NULL) - timer2 > tempsAdversaireCache)
                etatAdversaire = 2;
            break;
        case 2 :
            if (adversaire.etat != ROBOT_ACCROUPI)
                etatAdversaire = 0;
            break;

        }


        switch (etat)
        {
        case AVANCE_MUR:
            serveurAvancer();
            if (moi.x >=380)
                etat = ATTENTE;
            break;

        case ATTENTE :
            serveurStopperAction();
            if(moi.etat == ROBOT_IMMOBILE && moi.neigeDispo >= neigeACompacter )
                etat=SACCROUPI;
            else if(moi.neigeDispo < neigeACompacter)
                etat = RECULE;
            break;

        case SACCROUPI :
            serveurSAccroupir();
            if(moi.etat == ROBOT_ACCROUPI)
            {
                etat=RASSEMBLE;
            }
            break;

        case RASSEMBLE :
            serveurRassemblerNeige();
            if(moi.neigeRassemblee)
            {
                etat=COMPACTE;
            }
            break;

        case COMPACTE :
            serveurCompacterNeige(neigeACompacter);
            if (moi.nbBoule == 1)
            {
                time(&timer1);
                etat=ATTENTE_SAFE_BEFORE_ME_RELEVE;
            }
            break;

        case ATTENTE_SAFE_BEFORE_ME_RELEVE :
            if (moi.etat == ROBOT_SE_RELEVE)
                serveurStopperAction();
            else if (moi.etat != ROBOT_ACCROUPI)
            {
                serveurSAccroupir();
            }
            else if ((moi.nbBoule == 1 && adversaire.nbBoule==0 && (nbBoules == 0 || (nbBoules == 1 && boule[0].x < moi.x))) || time(NULL) - timer1 > tempsAvSeRelever )
            {
                etat=SERELEVE;
            }
            break;

        case SERELEVE :
            serveurSeRelever();
            if(moi.etat == ROBOT_IMMOBILE)
            {
                time(&timer1);
                etat=ATTENTE_SE_RELEVE;
            }
            break;

        case LANCE:
            CalculParamTrajectoire(moi.x, adversaire.x, jeu.hauteurMur, hauteurAFrapper, &energie, 250, &angle);
            if (etatAdversaire == 2 && adversaire.x < 680){
                gainAngle = 0.5;
                gainEnergie = 0.1;
                serveurLancer(energie + correctionEnergie(jeu.ventX, jeu.ventY, gainEnergie), angle + correctionAngle(jeu.ventX, jeu.ventY, gainAngle));
            }
            else{
                serveurLancer(energie + correctionEnergie(jeu.ventX, jeu.ventY, gainEnergie), angle + correctionAngle(jeu.ventX, jeu.ventY, gainAngle));
            }
            if(moi.nbBoule == 0)
            {
                etat = ATTENTE;
            }
            break;
        case AVANCE :
            serveurAvancer();
            if (moi.neigeDispo > neigeACompacter)
                etat = ATTENTE;
            else if (moi.x >=380)
                etat=RECULE;
            break;
        case RECULE :
            serveurReculer();
            if (moi.neigeDispo > neigeACompacter)
                etat = ATTENTE;
            if (moi.x <= 200){
                serveurStopperAction();
                etat = AVANCE_MUR;
            }
            break;
        case ATTENTE_SE_RELEVE :
            serveurStopperAction();
            if (adversaire.etat == ROBOT_SE_RELEVE || time(NULL) - timer1 > 3.0)// || (adversaire.etat == ROBOT_IMMOBILE && adversaire.nbBoule ==0) )
            {
                if (adversaire.bonnet)
                    hauteurAFrapper = hauteurBonnet;
                else
                    hauteurAFrapper = hauteurDebout;
                etat = LANCE;
            }
            else if (adversaire.etat == ROBOT_LANCE)
            {
                etat = ATTENTE_SAFE_BEFORE_ME_RELEVE;
            }
            else if (etatAdversaire == 2)
            {
                hauteurAFrapper = hauteurAccroupi;
                etat = LANCE;
            }
            break;

        default :
            serveurNeRienChanger();
            etat = 0;
        }
    }

    serveurFermer();
    printf("Serveur deconnecte\n");

    return 0;
}


//EXERCICE 1

/*
        // MACHINES A ETATS
        switch (etat)
        {
        case 0 :
            serveurStopperAction();
            if(moi.etat == ROBOT_IMMOBILE) etat=1;
            break;
        case 1 :
            serveurAvancer();
            if(moi.x >= 240)
            {
                etat=2;
            }
            break;
        case 2 :
            serveurStopperAction();
            if(moi.etat == ROBOT_IMMOBILE) etat=3;
            break;
        case 3 :
            serveurReculer();
            if (moi.x <= 80)
            {
                etat=0;
            }
            break;
        default :
            serveurNeRienChanger();
            etat = 0;
        }
*/


//EXERCICE 2

/*

        // MACHINES A ETATS
        switch (etat)
        {
        case 0 :
            serveurStopperAction();
            if(moi.etat == ROBOT_IMMOBILE) etat=1;
            break;
        case 1 :
            serveurSAccroupir();
            if(moi.etat == ROBOT_ACCROUPI) etat=2;
            break;
        case 2 :
            if (moi.neigeRassemblee)
            {
                serveurCompacterNeige(20);
            }
            else
            {
                serveurRassemblerNeige();
            }
            if(moi.nbBoule != 0) etat=3;
            break;
        case 3 :
            serveurSeRelever();
            if(moi.etat == ROBOT_IMMOBILE) etat=0;
            break;

        default :
            serveurNeRienChanger();
            etat = 0;
        }

*/
