#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include "slp.hpp"
#include "mlp.hpp"
#include "otimizador.hpp"


tp_slp        Slp;
tp_mlp        Mlp;

#define Rede Mlp

tp_Otimizador Otimizador;

double GEModelo, GEDados;





//-------------------------------------------------


double G_stepx = 1. / 16.;
double G_stept = 1. / 16.;

double G_dados[16][16];


/* 
Este não é o modo padrão de usar arquivos de cabeçalho,
    mas não é um modo errado de usar o pré-processador.

Simplesmente substituímos a ligação com um arquivo
externo e uma segunda ativação do compilador.
*/

#include "Modelos.h"



//--------------------------------------------------------------
/*! \brief Função objetivo a ser otimizada.
 *
 *  Função objetivo, deve ser substituída dependendo do
 *    problema a ser resolvido.
 */
//--------------------------------------------------------------
double FuncaoObjetivo (tp_vector v) {

   return (GEModelo = Modelo (v.todouble())) +
          (GEDados  = Dados  (v.todouble()));

   return 
          ParabolaTeste (v.todouble())
           +
          DerivadaParabolaTeste (v.todouble());


}


//--------------------------------------------------------------
/*! \brief Grava saída calculada pela rede.
 *
 *  Grava saída calculada pela rede, permitindo em seguida
 *    plotagem por arquivo auxiliar (Python / matplotlib).
 */
//--------------------------------------------------------------
void GerarMatrizSaida () {

   //unlink ("output1.csv"); // paranoia - fopen já faz isso.

   FILE *arq = fopen ("output1.csv", "wt");

   double x[2];

   int ix, it;

   for (it = 0; it < 16; it++) {

      x[1] = (double) it / 15.0;

      for (ix = 0; ix < 15; ix++) {

         x[0] = (double) ix / 15.0;

         fprintf (arq, "%f,", Rede.calc (x));
      }

      x[0] = (double) ix / 15.0;

      fprintf (arq, "%f\n", Rede.calc (x));
   }

   fclose (arq);


}

//===========================================================
void Andamento (int passo, double valor) {
   if (!(passo % 10)) printf ("%d) %f\n", passo, valor);
}

//===========================================================
int main (int argc, char *argv[]) {

//   Rede.load     ("boarede.csv");   GerarMatrizSaida();return 0;

   char buffer[2048];

   int    camadas[] = {25, 10, 0};

   double dominio[] = { -0.25, 1.25, -0.25, 1.25};

   int dimensao;

   Slp.init (2, 25     , dominio);
   Mlp.init (2, camadas, dominio);

   dimensao = Rede.length();

   printf ("\nEspaço de busca = %d\n", dimensao);

   Otimizador.prepare       (dimensao);
   Otimizador.set_loops     (1000);
   Otimizador.set_pop       (1000);
   Otimizador.set_evaluator (FuncaoObjetivo);
   Otimizador.set_announcer (Andamento);
   Otimizador.temperature   (0.125);

   tp_vector v(dimensao), v0(dimensao);

   v0.setvector (Rede.theta());

   v = Otimizador.minimise (v0);

   Rede.settheta (v.todouble());
   Rede.save     ("pcnn.csv");

   GerarMatrizSaida();

   printf ("Modelo = %f\nDados  = %f\n", GEModelo, GEDados);

   return 0;
}
