/*

Este código implementa as redes neurais descritas
nos trabalhos a seguir.

Neste caso uma rede de uma única camada

[1] Koscianski, A., 2004. Modèles de mécanique pour le temps réel.
   Tese de Doutorado, INSA de Rouen, França.

[2] Koscianski, A. and Souza de Cursi, J.E., 2005. Physically constrained neural networks and regularization of inverse problems. In Proceedings of the 6th World Congress on Structural and Multidisciplinary Optimisation. Rio de Janeiro, Brazil.

*/

#include "slp.hpp"

/*

Todo o código abaixo é uma versão reduzida para uma só camada,
do código para um perceptron multi-camada.

Os comentários presentes em "mlp.cpp" aplicam-se igualmente aqui.

O interesse em uma implementação separada de camada única
é livrar-se de laços e condições verificando se há ou não 
camadas adicionais e, com isso, ganhar alguns ciclos de CPU.

Otimizações dessa natureza são sempre relevantes, mas têm
impacto mais notável em aplicações combinando hardware limitado
(e.g.: IoT e outros dispositivos dependentes de bateria)
com um volume relativamente grande de cálculo.

*/


#define FAIXA      6.0l
#define DEMIFAIXA  3.0l

//-------------------------------------------------
void tp_slp::init (int inputs, int neurons, double *l) {

   int i;

   _qi = inputs;
   _qt = _qi + 1;
   _ql = _qt * neurons;

   _nneurons = neurons;

   _qn = _ql+neurons+1;

   _w  = new double[_qn];
   _a  = new double[_qi];
   _b  = new double[_qi];
   _f  = new double[_qi];
   _x  = new double [_qi];
   _y  = new double [neurons+1];
   _y2 = new double [neurons+1];


   for (i = 0; i <= _nneurons; i++)
      _y[i]=_y[2]=0;

   for (i = 0; i < _qn; i++)  // Inicialização de pesos; seção 2.1.5 da referência [1]
      _w[i]=0.0;


   for (i = 0; i < _qi; i++) {
      _a[i] = l[i*2  ];
      _b[i] = l[i*2+1];
      _f[i] = FAIXA / (_b[i] - _a[i]);
   }

   double d = FAIXA / (1.l + (double) _nneurons);
   for (i = 0; i < _nneurons; i++) {
      _w[i * _qt] = -DEMIFAIXA + d*(i+1);
   }

}


//-------------------------------------------------
/*! \brief Ajuste da entrada.
 *
 * Função que remapeia entradas para uma faixa de valores
 * que não provoque saturação dos neurônios.
 * V. seção 2.1.5 da referência [1].
 *
 */
void tp_slp::adjust (double *x) {

   for (int i = 0; i < _qi; i++)
      _x[i] = (x[i] - _a[i]) * _f[i] - 3.0l;
}

//-------------------------------------------------
tp_slp::tp_slp (void) {
   _w = _a = _b = _y = NULL;
   _remarks = NULL;
}
//-------------------------------------------------
tp_slp::~tp_slp (void) {
   destroy ();
}
//-------------------------------------------------
void tp_slp::destroy (void) {
   if (NULL == _w) return;
   delete[] _w;
   delete[] _x;
   delete[] _f;
   delete[] _a;
   delete[] _b;
   delete[] _y;
   delete[] _y2;
}


//-------------------------------------------------
/*! \brief dot product.
 *
 *   Implementação de produto vetorial entre saídas
 *   de neurônios e respectivos pesos.
 *
 */
double tp_slp::dotproduct (int in) {

   int i;
   double total = 0;

   for (i = 0; i < _qi; i++)
      total += _x[i] * _w[in * _qt + i + 1];

   return total;
}


//-------------------------------------------------
/*! \brief Tangente sigmoidal.
 *
 * Função tangente sigmoidal.
 */
double tp_slp::sigma (double x) {
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
double tp_slp::calc (double *x) {

   int i;
   double total = 0, t;

   adjust (x);

   for (i = 0; i < _nneurons; i++) {
      _y [i] = t = sigma ( dotproduct(i) + _w[i * _qt]);
      _y2[i] = 1.0 - (t * t);

      total += t * _w[i + _ql];
   }

   return _y[_nneurons] = total + _w[_qn-1];
}



//-------------------------------------------------
/*! \brief Primeira derivada.
 *
 * Calcula derivada.
 * As equações correspondentes à implementação das derivadas 
 * são descritas na referência [1], seção 2.1 (v. topo deste arquivo).
 * Notar que o caso aqui implementado é mais simples
 * (uma única camada)
 *
 */
double tp_slp::deriv1 (int ix) {

   int i;
   double total = 0;

   for (i = 0; i < _nneurons; i++) {
      total += _w[i + _ql] * neuronderiv1 (i) * _w[i * _qt + ix + 1];
   }

   return total * _f[ix];
}


//-------------------------------------------------
/*! \brief Segunda derivada.
 *
 * Calcula derivada.
 * As equações correspondentes à implementação das derivadas 
 * são descritas na referência [1], seção 2.1 (v. topo deste arquivo).
 * Notar que o caso aqui implementado é mais simples
 * (uma única camada)
 *
 */
double tp_slp::deriv2 (int x0, int x1) {

   int i;
   double total = 0;

   for (i = 0; i < _nneurons; i++)
      total += _y[i] * _w[i + _ql] * _y2[i]  * _w[i * _qt + x0 + 1] * _w[i * _qt + x1 + 1];

   return _y[_nneurons] = -2.0 * total * _f[x0] * _f[x1];
}



//-------------------------------------------------
/*! \brief Derivadas.
 *
 * Calcula derivadas.
 * Uma implementação para derivadas de ordem superior, mas 
 * limitada: não se aplica para funções a mais de uma variável.
 *
 */
double tp_slp::neuronderiv (int i, int degre) {

   double
       O
      ,A
      ,B
      ,C
      ;

   if (1 == degre) return _y2[i];
   if (2 == degre) return -2.0 * _y[i] * _y2[i];

   O = _y[i];
   A = _y2[i];
   B = -2.0 * O * A;

   if (3 == degre) return - 2.0 * A * A -  2.0 * O * B;

   C = - 2.0 * A * A -  2.0 * O * B;

   if (4 == degre) return -6.0*A*B - 2.0*O*C;

   return 0.;
}





//-------------------------------------------------
/*! \brief Grava a rede em um arquivo csv (separador = ;)
 *
 * Grava a rede em um arquivo csv (separador = ;),
 * como uma sequência (única linha) de números.
 * A sequência é:
 *  número de entradas, 
 *  número de neurônios,
 *  0, (número zero)
 *  valor mínimo, valor máximo, de cada entrada, para todas entradas
 *  fator de conversão de cada entrada, para todas entradas,
 *  1.0, (número um)
 *  "bias" de cada neurônio, começando da camada zero, neurônio zero,
 *  "pesos"
 *  0.0 (número zero).
 * 
 */
int tp_slp::save (const char *s) {


  FILE *file;


   int i, j, k, n;


   if (NULL == (file = fopen (s, "wt")))
     return 0;

   fprintf (file, "%6.18lf;%6.18lf;0.;", (double) _qi, (double) _nneurons);

   for (i = 0; i < _qi; i++) {
     if (1 > fprintf (file, "%6.18lf;", (double) _a[i]))
        return 0;
     if (1 > fprintf (file, "%6.18lf;", (double) _b[i]))
        return 0;
   }

   for (i = 0; i < _qi; i++) {
     if (1 > fprintf (file, "%6.18lf;", 6. / ((double) _b[i]-(double) _a[i])))
        return 0;
   }

   if (1 > fprintf (file, "1.000000;")) //, _scaleY))
     return 0;

   // BIAS
   for (i = 0; i < _nneurons; i++)
     if (1 > fprintf (file, "%6.18lf;", (double) _w[i * _qt]))
        return 0;

   fprintf (file, "%6.18lf;", (double) _w[_qn-1]);


   // WEIGTH
  for (i = 0; i < _nneurons; i++)
  for (j = 0; j < _qi; j++)
     if (1 > fprintf (file, "%6.18lf;", (double) _w[i * _qt + j + 1]))
        return 0;

  for (i = 0; i < _nneurons; i++)
     if (1 > fprintf (file, "%6.18lf;", (double) _w[i + _ql]))
        return 0;


  if (1 > fprintf (file, "0.0\n"))
     return 0;


   // Um analista muito minucioso notará uma surpresa aqui.
   // O campo 'remarks' foi deixado sem uso em MLP.cpp.
   // O propósito é gravar junto com os parâmetros da rede neural
   //   uma string descritiva.
   // No frigir dos ovos não é uma função de extrema relevância e
   //   sim um adicional - como frisos coloridos na porta -
   //   e por esse motivo não foi mantido no outra rede.
   if (NULL != _remarks) {
     fputs (_remarks, file);
   }


  fclose (file);


  return 1;
}

//-------------------------------------------------
/*! \brief Carrega rede de um arquivo csv (separador = ;)
 *
 * Consultar documentação da função save().
 *
 */
int tp_slp::load (const char *s) {


   FILE *file;

   int i, j, k, n; double aux;
   double coisa[20];


   for (i = 0; i < 20; i++) coisa[i] = i;

   //.......................................................

   destroy ();

   if (NULL == (file = fopen (s, "rt")))
     return 0;

   if (1 > fscanf (file, "%lf;", &aux))
      return 0;
   _qi = (int) aux;

   if (1 > fscanf (file, "%lf;", &aux))
      return 0;
   _nneurons = (int) aux;

   if (1 > fscanf (file, "%lf;", &aux))
      return 0;

   // Crie (aloque) uma rede com valores quaisquer
   //   de _a e _b. Na sequência os números passados
   //   dentro de 'coisa' serão lidos do arquivo.
   init (_qi, _nneurons, coisa);

   for (i = 0; i < _qi; i++) {
      if (1 > fscanf (file, "%lf;", &aux))
         return 0;
      _a[i] = aux;
      if (1 > fscanf (file, "%lf;", &aux))
         return 0;
      _b[i] = aux;
   }

   // factor
   for (i = 0; i < _qi; i++) {
      if (1 > fscanf (file, "%lf;", &aux))
        return 0;
      _f[i] = aux;
   }

   // scaleY
   if (1 > fscanf (file, "%lf;", &aux))
     return 0;


   // BIAS
   for (i = 0; i < _nneurons; i++) {
      if (1 > fscanf (file, "%lf;", &aux))
        return 0;
     _w[i * _qt] = aux;
   }

   if (1 > fscanf (file, "%lf;", &aux))
     return 0;
   _w[_qn - 1] = aux;


   // WEIGHT
   for (i = 0; i < _nneurons; i++)
      for (j = 0; j < _qi; j++) {
         if (1 > fscanf (file, "%lf;", &aux))
           return 0;
        _w[i * _qt + j + 1] = aux;
      }

   for (i = 0; i < _nneurons; i++) {
      if (1 > fscanf (file, "%lf;", &aux))
        return 0;

     _w[_ql + i] = aux;
   }

   // veja comentário em ::save.
   if (NULL != _remarks) {
      fgets (_remarks, 1000, file);
   }
   
  fclose (file);


  return 1;
}

//-------------------------------------------------
/*
Teste de arquivos
*/
//void testando (void) {
//
//
//   tp_slp net;
//   double as[] = {-0.1, 5.1};
//
//   net.init (1, 7, as);
//
//   net.load ("c:\\temp\\net.txt");
//   net.save ("c:\\temp\\nnet.txt");
//}
