/*

Este código implementa as redes neurais descritas
nos trabalhos a seguir.

[1] Koscianski, A., 2004. Modèles de mécanique pour le temps réel.
   Tese de Doutorado, INSA de Rouen, França.

[2] Koscianski, A. and Souza de Cursi, J.E., 2005. Physically constrained neural networks and regularization of inverse problems. In Proceedings of the 6th World Congress on Structural and Multidisciplinary Optimisation. Rio de Janeiro, Brazil.

*/

#include "mlp.hpp"


//--------------------------------------------------------------
/*! \brief Limites de saturação da rede.
 *
 *   Os valores FAIXA e DEMIFAIXA são usados para mapear entradas quaisquer
 *   para uma faixa de valores limitada; 
 *   a função sigmoidal usada no neurônio tem assíntotas à direita
 *   e à esquerda, e do ponto de vista de cálculo numérico é
 *   mais razoável mapear o domínio de entrada para uma faixa de valores
 *   em que a função seja "mais significativa". Fora dessa faixa,
 *   sig(x+delta) varia muito pouco para pequenos valores de delta,
 *   inutilizando tentativas de otimizar função de custo
 *   (v. descrição global referente à formulação do problema).
 *
 */
//--------------------------------------------------------------
#define FAIXA      6.0l
#define DEMIFAIXA  3.0l


//--------------------------------------------------------------
/*! \brief Constrói uma nova rede.
 *
 *   Dado o número de entradas e a topologia desejada,
 *   constrói uma rede.
 *   Exemplo, init (2, [10 5 1], [-1, 3, -1, 1]
 *   constrói uma rede com duas entradas, camadas com 10, 5 e 1 neurônio,
 *   e com entradas nos intervalos -1 a 3  e  -1 a 1.
 *
 */
void tp_mlp::init (int inputs, int *layers, double *l) {

   int i, n, m;

   i = 0;
   m = 0;
   _nneurons = 0;
   n = _ninputs = inputs;
   while ((layers[i] != 0) && (i < 10)) {

      _nparmsneuron[i] = n+1;
      _nparmscouche[i] = (n+1) * layers[i];

      m += _nparmscouche[i];

      n  = layers[i];
      _layers[i] = n;

      _nneurons += n;

      i  = i+1;
   }

   _nlayers = i-1;

   if (i > 9) throw "too much layers 4 MLP";

   _len_w = m;
   _ninputs = inputs;

   allouer    ();

   _a  = new double[_ninputs];
   _b  = new double[_ninputs];
   _f  = new double[_ninputs];
   _x  = new double [_ninputs];

   _parms = new double [_len_w];

   for (i = 0; i < _ninputs; i++) {
      _a[i] = l[i*2  ];
      _b[i] = l[i*2+1];
      _f[i] = FAIXA / (_b[i] - _a[i]);
   }

   double d = FAIXA / (1.l + (double) _layers[0]);
   for (i = 0; i < _layers[0]; i++) {
      _bias[0][i] = -DEMIFAIXA + d*(i+1);

      for (n = 0; n < _ninputs; n++)  // Inicialização de pesos; seção 2.1.5 da referência [1]
         _w[0][i][n] = 0.0;
   }

   for (n = 1; n <= _nlayers; n++) {
      d = FAIXA / (1.l + (double) _layers[n]);
      for (i = 0; i < _layers[n]; i++) {
         _bias[n][i] = -DEMIFAIXA + d*(i+1);

         m = _layers[n-1];
         for (int j = 0; j < m; j++)
            _w[n][i][j] = 0.0;

      }
   }



}
//-------------------------------------------------
/*! \brief Construtor.
 *
 *   Construtor, invoca init().
 *
 */

tp_mlp::tp_mlp (int p1, int *p2, double *p3) {
   _w      = NULL;
   _bias   = NULL;
   _a = _b = NULL;
   _y      = NULL;
   _remarks = NULL;

   init (p1, p2, p3);
}
//-------------------------------------------------
tp_mlp::tp_mlp (void) {
   _w      = NULL;
   _bias   = NULL;
   _a = _b = NULL;
   _y      = NULL;
   _remarks = NULL;
}
//-------------------------------------------------
tp_mlp::~tp_mlp (void) {
   destroy ();
}
//-------------------------------------------------
/*! \brief Libera memória.
 *
 *   Libera memória.
 *
 */
void tp_mlp::destroy (void) {

   if (NULL == _w) return;

   deallouer    ();

   delete[] _x;
   delete[] _f;
   delete[] _a;
   delete[] _b;

   delete[] _parms;
}


//-------------------------------------------------
/*! \brief dot product.
 *
 *   Implementação de produto vetorial entre saídas
 *   de neurônios e respectivos pesos.
 *
 */
double tp_mlp::dotproduct (int c, int n) {

   int i;
   double total = 0;

   if (0 == c)
      for (i = 0; i < _ninputs; i++)
         total += _x[i] * _w[c][n][i];
   else
      for (i = 0; i < _layers[c-1]; i++)
         total += _y[c-1][i] * _w[c][n][i];  // [layer][neuron][input]

   return total;
}


//-------------------------------------------------
/*! \brief Tangente sigmoidal.
 *
 * Aqui a famosa tangente sigmoidal.
 * Para efeito de aceleração de cálculo, valores fora
 * de +-40 são saturados artificialmente.
 * Essa faixa de valores foi escolhida arbitrariamente.
 * Os neurônios de entrada recebem valores ajustados
 * (variáveis _a, _b, _f, mas os neurônios de camadas intermediárias
 * podem ter de lidar com valores quaisquer.
 * Estes ifs pretenderiam ganhar alguma otimização em plataformas
 * de cálculo lentas.
 */
double tp_mlp::sigma (double x) {
   if (x > 40.)
   return 1.;
   if (x < -40.)
   return -1.;
   return 2./(1.0+exp(-2.*x))-1.;
}



//-------------------------------------------------
/*! \brief Calcula saída da rede.
 *
 * Calcula saída da rede.
 * As variáveis _y e _y1 implementam um mecanismo de
 * cache para acelerar cálculo das derivadas.
 *
 */
double tp_mlp::calc (double *x) {

   int c,n;
   double total = 0, t;

   adjust (x);

   for (c = 0; c < _nlayers; c++) {
      for (n = 0; n < _layers[c]; n++) {
         t = sigma ( dotproduct(c,n) + _bias[c][n]);
         _y [c][n] = t;
         _y1[c][n] = 1.0 - (t * t);
      }
   }

   for (n = 0; n < _layers[_nlayers]; n++) {
      total += dotproduct(_nlayers,n);
   }
   total += _bias[_nlayers][0];
   _y [c][0] = total;

   return total;
}


//-------------------------------------------------
/*! \brief Primeira derivada.
 *
 * Calcula derivada.
 * As equações correspondentes à implementação das derivadas 
 * são descritas na referência [1], seção 2.1 (v. topo deste arquivo).
 *
 */
double tp_mlp::deriv1 (int ix) {

   int c,n,i;

   for (n = 0; n < _layers[0]; n++)
      _ydx[0][n][ix] = _y1[0][n] * _w[0][n][ix];

   for (c = 1; c < _nlayers; c++) {
      for (n = 0; n < _layers[c]; n++) {
         _ydx[c][n][ix] = 0.0;

         for (i = 0; i < _layers[c-1]; i++)
            _ydx[c][n][ix] += _w[c][n][i] * _ydx[c-1][i][ix];

         _ydx[c][n][ix] *= _y1[c][n];
      }
   }

   c = _nlayers - 1;
   n = _layers[c];
   _ydx[_nlayers][0][ix] = 0;
   for (i = 0; i < n; i++) {
      _ydx[_nlayers][0][ix] += _w[_nlayers][0][i] * _ydx[c][i][ix];
   }

   return _ydx[_nlayers][0][ix] * _f[ix];
}


//-------------------------------------------------
/*! \brief Segunda derivada.
 *
 * Calcula derivada.
 * As equações correspondentes à implementação das derivadas 
 * são descritas na referência [1], seção 2.1 (v. topo deste arquivo).
 *
 */
double tp_mlp::deriv2 (int ix0, int ix1) {

   int c,n,i;
   double tmp, tmp2;

   for (n = 0; n < _layers[0]; n++)
      _ydx2[0][n] = -2.0*_y[0][n]*_y1[0][n]*_w[0][n][ix0]*_w[0][n][ix1];

   for (c = 1; c < _nlayers; c++) {
      for (n = 0; n < _layers[c]; n++) {

         tmp  = 0.0;
         tmp2 = 0.0;
         for (i = 0; i < _layers[c-1]; i++) {
            tmp  += _w[c][n][i] * _ydx [c-1][i][ix0];
            tmp2 += _w[c][n][i] * _ydx2[c-1][i];
         }

         _ydx2[c][n] = (-2.0 * _y[c][n] * _y1[c][n] * tmp) +
                       (_y1[c][n] * tmp2);
      }
   }

   c = _nlayers-1;
   n = _layers[c];
   _ydx2[_nlayers][0] = 0;
   for (i = 0; i < n; i++) {
      _ydx2[_nlayers][0] += _w[_nlayers][0][i] * _ydx2[c][i];
   }

   return _ydx2[_nlayers][0] * _f[ix0] * _f[ix1];
}

//-------------------------------------------------
/*! \brief Ajuste da entrada.
 *
 * Função que remapeia entradas para uma faixa de valores
 * que não provoque saturação dos neurônios.
 * V. seção 2.1.5 da referência [1].
 *
 */
void tp_mlp::adjust (double *x) {

   for (int i = 0; i < _ninputs; i++)
      _x[i] = (x[i] - _a[i]) * _f[i] - 3.0l;
}



void tp_mlp::allouer (void) {

   _w = new double**[_nlayers+1];        // STEP 1: SET UP THE ROWS.

   _bias = new double *[_nlayers+1];

   for (int j = 0; j <= _nlayers; j++) {

      _w[j]    = new double*[_layers[j]];
      _bias[j] = new double [_layers[j]];


      for (int k = 0; k < _layers[j]; k++)
         _w[j][k] = new double [_nparmsneuron[j]-1];


   }

   _y    = new double*[_nlayers+1];        // STEP 1: SET UP THE ROWS.
   _y1   = new double*[_nlayers+1];        // STEP 1: SET UP THE ROWS.
   _ydx  = new double**[_nlayers+1];        // STEP 1: SET UP THE ROWS.
   _ydx2 = new double *[_nlayers+1];        // STEP 1: SET UP THE ROWS.

   for (int j = 0; j < _nlayers+1; j++) {

      _y   [j] = new double[_layers[j]];
      _y1  [j] = new double[_layers[j]];
      _ydx [j] = new double*[_layers[j]];
      _ydx2[j] = new double[_layers[j]];

      for (int k = 0; k < _layers[j]; k++) {
         _ydx [j][k] = new double[_ninputs];
//         _ydx2[k] = new double [_ninputs]; // seria útil para derivadas de ordem 3.
      }

   }
}

//----------------------------------------------------------------

void tp_mlp::deallouer (void) {

   for (int j = 0; j < _nlayers+1; j++) {

      delete[] _bias[j];
      for (int k = 0; k < _layers[j]; k++)
         delete[] _w[j][k];

      delete[] _w[j];

   }
   delete[] _w;
   delete[] _bias;


   for (int j = 0; j < _nlayers+1; j++) {

      delete[] _y   [j];
      delete[] _y1  [j];

      for (int k = 0; k < _layers[j]; k++) {
         delete[] _ydx [j][k];
//         delete[] _ydx2[j][k]; // seria útil como cache p/ derivadas de ordem 3.
      }

      delete[] _ydx [j];
      delete[] _ydx2[j];


   }
   delete[] _y   ;
   delete[] _y1  ;
   delete[] _ydx ;
   delete[] _ydx2;
}


//-------------------------------------------------
/*! \brief Comprimento do vetor de parâmetros da rede.
 *
 * Comprimento de um vetor reunindo todos os parâmetros da rede.
 * Um bom cientista da computação simplesmente derivaria a fórmula,
 * mas um programador com uma dose de paranóia contra bugs 
 * prefere (preferiu) contar nos dedos.
 *
 */
int tp_mlp::length (void) {

   int c,n,i,k,q;

   k = 0;
   for (c = 0; c <= _nlayers; c++) {
      for (n = 0; n < _layers[c]; n++) {

         if (!c) q = _ninputs;
         else    q = _layers[c-1];

         k++;

         for (i = 0; i < q; i++) {
            k++;
         }
      }
   }

   return k;
}


//---------------------------------------------
/*! \brief Vetor de parâmetros da rede.
 *
 * Vetor reunindo todos os parâmetros da rede,
 * retornado como um simples array de doubles.
 *
 */
double *tp_mlp::theta (void) {

   int c,n,i,k,q;

   k =0;
   for (c = 0; c <= _nlayers; c++) {
      for (n = 0; n < _layers[c]; n++) {

         if (!c) q = _ninputs;
         else    q = _layers[c-1];

         _parms[k] = _bias[c][n];
         k++;

         for (i = 0; i < q; i++) {
            _parms[k] = _w[c][n][i];
            k++;
         }
      }
   }

   return _parms;
}


//---------------------------------------------
/*! \brief Define vetor de parâmetros da rede.
 *
 * Recebe um vetor com os parâmetros da rede.
 * Necessário que a rede já tenha sido inicializada;
 * não há nenhum teste contra erros.
 *
 */
void tp_mlp::settheta (double *p) {

   int c,n,i,k,q;

   k =0;
   for (c = 0; c <= _nlayers; c++) {
      for (n = 0; n < _layers[c]; n++) {

         if (!c) q = _ninputs;
         else    q = _layers[c-1];

         _bias[c][n] = p[k];
         k++;

         for (i = 0; i < q; i++) {
            _w[c][n][i] = p[k];
            k++;
         }
      }
   }
}


//-------------------------------------------------
/*! \brief Grava a rede em um arquivo csv (separador = ;)
 *
 * Grava a rede em um arquivo csv (separador = ;),
 * como uma sequência (única linha) de números.
 * A sequência é:
 *  número de entradas, 
 *  número de neurônios por camada,
 *  0, (número zero)
 *  valor mínimo, valor máximo, de cada entrada, para todas entradas
 *  fator de conversão de cada entrada, para todas entradas,
 *  1.0, (número um)
 *  "bias" de cada neurônio, começando da camada zero, neurônio zero,
 *  "pesos" para cada entrada para cada neurônio,
 *  "pesos" para cada conexão para cada neurônio de cada camada,
 *  0.0 (número zero).
 * 
 */
int tp_mlp::save (const char *s) {


   FILE *file;

   int i;


   if ((NULL == _w) || (NULL == (file = fopen (s, "wt"))))
     return 0;

   fprintf (file, "%6.18lf;", (double) _ninputs);

   for (i = 0; i <= _nlayers; i++)
      fprintf (file, "%6.18lf;", (double) _layers[i]);

   fprintf (file, "0.;");


   for (i = 0; i < _ninputs; i++) {
     if (1 > fprintf (file, "%6.18lf;", (double) _a[i]))
        return 0;
     if (1 > fprintf (file, "%6.18lf;", (double) _b[i]))
        return 0;
   }

   for (i = 0; i < _ninputs; i++) {
     if (1 > fprintf (file, "%6.18lf;", _f[i]))
        return 0;
   }

   if (1 > fprintf (file, "1.000000;")) //, _scaleY))
     return 0;


   // BIAS
   for (int c = 0; c < _nlayers; c++) {
      for (int n = 0; n < _layers[c]; n++) {
         if (1 > fprintf (file, "%6.18lf;", (double) _bias[c][n]))
            return 0;
      }
   }

   fprintf (file, "%6.18lf;", (double) _bias[_nlayers][0]);

   // WEIGTH
   for (int n = 0; n < _layers[0]; n++) {
      for (int i = 0; i < _ninputs; i++) {
         if (1 > fprintf (file, "%6.18lf;", (double) _w[0][n][i]))
            return 0;
      }
   }

   for (int c = 1; c <= _nlayers; c++) {
      for (int n = 0; n < _layers[c]; n++) {
         for (int i = 0; i < _nparmsneuron[c]-1; i++) {

            if (1 > fprintf (file, "%6.18lf;", (double) _w[c][n][i]))
               return 0;
         }
      }
   }

   if (1 > fprintf (file, "0.0\n"))
      return 0;

  fclose (file);


  return 1;
}


//-------------------------------------------------
/*! \brief Carrega rede de um arquivo csv (separador = ;)
 *
 * Consultar documentação da função save().
 *
 */
int tp_mlp::load (const char *s) {


   FILE *file;

   int i, k;
   double aux;
   double osranges[20];
   int    oslayers[10];


   //.......................................................

   destroy ();


   if (NULL == (file = fopen (s, "rt")))
     return 0;

   if (1 > fscanf (file, "%lf;", &aux))
      return 0;

   _ninputs = (int) aux;

   k = 0;
   _nneurons = 0;
   do {

      if (1 > fscanf (file, "%lf;", &aux))
         return 0;

      oslayers[k] = (int) aux;

      _nneurons += (int) aux;
      k++;
   } while (aux);


   k = 0;
   for (i = 0; i < _ninputs; i++) {
      osranges[k] = -1.; ++k;
      osranges[k] =  1.; ++k;
   }

   // Inicialize (aloque) rede e use
   //   valores de domínio arbitrários [-1..1].
   // Na sequência de leitura do arquivo isso será ajustado.
   init (_ninputs, oslayers, osranges);


   k = 0;
   for (i = 0; i < _ninputs; i++) {

      if (1 > fscanf (file, "%lf;", &aux))
         return 0;

      _a[k] = aux;

      if (1 > fscanf (file, "%lf;", &aux))
         return 0;

      _b[k] = aux;
      k++;
   }

   k = 0;
   for (i = 0; i < _ninputs; i++) {

      if (1 > fscanf (file, "%lf;", &aux))
         return 0;

      _f[k] = aux;
      k++;
   }

   if (1 > fscanf (file, "%lf;", &aux))
      return 0;

   // BIAS
   for (int c = 0; c < _nlayers; c++) {
      for (int n = 0; n < _layers[c]; n++) {
         if (1 > fscanf (file, "%lf;", &aux))
            return 0;
         _bias[c][n] = aux;
      }
   }

   if (1> fscanf (file, "%lf;", &aux))
      return 0;

   _bias[_nlayers][0] = aux;


   // WEIGTH
   for (int n = 0; n < _layers[0]; n++) {
      for (int i = 0; i < _ninputs; i++) {
         if (1 > fscanf (file, "%lf;", &aux))
            return 0;
         _w[0][n][i] = aux;
      }
   }

   for (int c = 1; c <= _nlayers; c++) {
      for (int n = 0; n < _layers[c]; n++) {
         for (int i = 0; i < _nparmsneuron[c]-1; i++) {

            if (1 > fscanf (file, "%lf;", &aux))
               return 0;
            _w[c][n][i] = aux;
         }
      }
   }

   if (1 > fscanf (file, "%lf", &aux))
      return 0;

  fclose (file);


  return 1;
}



//-------------------------------------------------
// Simplesmente retorne em um vetor a sequência de parâmetros.
tp_vector tp_mlp::vectortheta (void) {

   tp_vector x (_len_w);

   x.setvector (theta ());

   return x;
}

//---------------------------------------------------
/* função auxiliar (bypass) para receber
um vetor w e usá-lo como parâmetros da rede */
void tp_mlp::setvectortheta (tp_vector w) {

   double *p;

   p = new double[w.size ()];

   w.todouble (p);

   settheta (p);

   delete[] p;

}



