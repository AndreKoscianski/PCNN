/*

Physically Constrained Radial Basis Neural Network




disposição (layout) da rede
um neurônio calcula  exp (ax + by + ... + w)
cada neurônio é multiplicado por P
a rede tem um bias B, e calcula B + P*neuron1 P2*neuron2...
 [B P P2  A B W A2 B2 W2]
  0 1 2   3 4 5 6  7  8  ..9
Há um total de _Nn número de neurônios 3
Há um total de _Ne número de entradas  2
Cada neurônio tem _Npi parâmetros internos = _Ne+1  3
Cada neurônio tem _Npp parâmetros = _Npi+1  4
A rede tem  _Npt parâmetros no total = 1+(_Npp*_Nn) 9

*/





#include "pcrbnn.hpp"



// acesse peso de um neurônio
#define M_P(neuron) _w[1+neuron]

// acesse um parâmetro interno de um neurônio (0,0)
#define M_w(neuron,entrada) _w[_Nn+(neuron*_Npi)+entrada]

// acesse bias dentro de um neurônio
#define M_b(neuron) _w[_Nn+(neuron*_Npi)+_Ne]




//-------------------------------------------------
void tp_gnn::init (int inputs, int neurons, double *l) {

   int i;


// Há um total de _Nn número de neurônios 3
// Há um total de _Ne número de entradas  2
// Cada neurônio tem _Npi parâmetros internos = _Ne+1  3
// Cada neurônio tem _Npp parâmetros = _Npi+1  4
// A rede tem  _Npt parâmetros no total = 1+(_Npp*_Nn) 9
   _Nn = neurons;
   _Ne = inputs;

   _Npi = _Ne  + 1;
   _Npp = _Npi + 1;
   _Npt = (_Npp * _Nn) + 1;

   _qi = inputs;         // quantas entradas
   _qt = _qi + 1;        // parâmetros dentro de um neurônio
   _ql = _qt * neurons;  // parâmetros todos os neurônios
   _qn = _ql+neurons+1;  // parâmetros toda a rede

   _Nn = neurons;

   _w  = new double [_Npt];
   _a  = new double [_Ne];
   _b  = new double [_Ne];
   _f  = new double [_Ne];
   _x  = new double [_Ne];
   _y  = new double [_Nn+1];
   _neuron = new double [_Nn];


   for (i = 0; i <= _Nn; i++)
      _y[i]=_y[2]=0;

   for (i = 0; i < _qn; i++)
      _w[i]=0.0;


   for (i = 0; i < _qi; i++) {
      _a[i] = l[i*2  ];
      _b[i] = l[i*2+1];
      _f[i] = FAIXA / (_b[i] - _a[i]);
   }

   double d = FAIXA / (1.l + (double) _Nn);
   for (i = 0; i < _Nn; i++) {
      _w[i * _qt] = -DEMIFAIXA + d*(i+1);
   }

}

//-------------------------------------------------
/*! \brief Ajuste da entrada.
 *
 * Função que remapeia entradas para uma faixa de valores
 * que não provoque saturação dos neurônios.
 *
 */
void tp_gnn::adjust (double *x) {

   for (int i = 0; i < _qi; i++)
      _x[i] = (x[i] - _a[i]) * _f[i] - DEMIFAIXA;
}

//-------------------------------------------------
tp_gnn::tp_gnn (void) {
   _w = _a = _b = _y = NULL;
   _remarks = NULL;
}
//-------------------------------------------------
tp_gnn::~tp_gnn (void) {
   destroy ();
}
//-------------------------------------------------
void tp_gnn::destroy (void) {
   if (NULL == _w) return;
   delete[] _w;
   delete[] _x;
   delete[] _f;
   delete[] _a;
   delete[] _b;
   delete[] _y;
   delete[] _neuron;
}

//-------------------------------------------------
/*! \brief dot product.
 *
 *   Implementação de produto vetorial entre saídas
 *   de neurônios e respectivos pesos.
 *
 */
double tp_gnn::dotproduct (int neuron) {

   double total = 0;

   for (int entrada = 0; entrada < _qi; entrada++)
      total += M_w(neuron,entrada) * _x[entrada];

   return total + M_b(neuron);
}

//-------------------------------------------------
/*! \brief Função gaussiana.
 *
 * Cada neurônio calcula isto.
 */
double tp_gnn::sigma (double x) {
   return exp (- (x*x));
}



//-------------------------------------------------
/*! \brief Saída da rede neural.
 *
 * Cálculo da rede neural.
 */
double tp_gnn::calc (double *x) {

   int i;
   double t, total = 0;

   adjust (x);

   for (int nn = 0; nn < _Nn; nn++) {

      _y [nn]     = t = sigma (dotproduct (nn));

      _neuron[nn] = t;

      total     += (M_P(nn) * t);
   }

   return _y[_Nn] = total + _w[0];
}


//-------------------------------------------------
/*! \brief Derivadas da rede.
 *
 * Todas as derivadas da rede em relação às entradas
 * são obtidas a partir desta função.
 *
 */
double tp_gnn::deriv (int degre, int i, int j) {

   double total = 0;

   for (int neuron = 0; neuron < _Nn; neuron++)
      total += M_P(neuron) * neuronderiv (neuron, degre, i, j);

   return total;
}




//-------------------------------------------------
/*! \brief Derivadas da função de ativação (gaussiana).
 *
 * Derivadas da função de ativação da rede.
 * Ou seja, cálculo da derivada de um único neurônio.
 *
 */
double tp_gnn::neuronderiv (int neuron, int degre, int i, int j) {

   double dp = dotproduct (i);

   //dy_dx  − 2a*(ax+b) * e−(ax+b)^2
//   if (1 == degre) return -2.0 * _w[i + _ql] * dp * _neuron[i];
   if (1 == degre) return -2.0 * M_w(neuron,i) * dp * _neuron[i];


   //d2y_dx2 = 4ab(by+ax+c)^2*e^−(by+ax+c)^2  −2ab*e^−(by+ax+c)^2
   if (2 == degre) return  4.0 * M_w(neuron,i) * M_w(neuron,j) * dp * dp * _neuron[i]
                          -2.0 * M_w(neuron,i) * M_w(neuron,j) * _neuron[i];

   return 0.0;
}







//-------------------------------------------------
int tp_gnn::save (const char *s) {


  FILE *file;


   int i, j, k, n;


   if (NULL == (file = fopen (s, "wt")))
     return 0;

   fprintf (file, "%6.18lf;%6.18lf;0.;", (double) _qi, (double) _Nn);

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
   for (i = 0; i < _Nn; i++)
     if (1 > fprintf (file, "%6.18lf;", (double) _w[i * _qt]))
        return 0;

   fprintf (file, "%6.18lf;", (double) _w[_qn-1]);


   // WEIGTH
  for (i = 0; i < _Nn; i++)
  for (j = 0; j < _qi; j++)
     if (1 > fprintf (file, "%6.18lf;", (double) _w[i * _qt + j + 1]))
        return 0;

  for (i = 0; i < _Nn; i++)
     if (1 > fprintf (file, "%6.18lf;", (double) _w[i + _ql]))
        return 0;


  if (1 > fprintf (file, "0.0\n"))
     return 0;

   if (NULL != _remarks) {
     fputs (_remarks, file);
   }


  fclose (file);


  return 1;
}


//-------------------------------------------------
int tp_gnn::load (const char *s) {


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
   _Nn = (int) aux;

   if (1 > fscanf (file, "%lf;", &aux))
      return 0;

   init (_qi, _Nn, coisa);

   for (i = 0; i < _qi; i++) {
      if (1 > fscanf (file, "%lf;", &aux))
         return 0;
      _a[i] = aux;
      if (1 > fscanf (file, "%lf;", &aux))
         return 0;
      _b[i] = aux;
   }

   // factor, not used
   for (i = 0; i < _qi; i++) {
      if (1 > fscanf (file, "%lf;", &aux))
        return 0;
      _f[i] = aux;
   }

   // scaleY
   if (1 > fscanf (file, "%lf;", &aux))
     return 0;


   // BIAS
   for (i = 0; i < _Nn; i++) {
      if (1 > fscanf (file, "%lf;", &aux))
        return 0;
     _w[i * _qt] = aux;
   }

   if (1 > fscanf (file, "%lf;", &aux))
     return 0;
   _w[_qn - 1] = aux;


   // WEIGHT
   for (i = 0; i < _Nn; i++)
      for (j = 0; j < _qi; j++) {
         if (1 > fscanf (file, "%lf;", &aux))
           return 0;
        _w[i * _qt + j + 1] = aux;
      }

   for (i = 0; i < _Nn; i++) {
      if (1 > fscanf (file, "%lf;", &aux))
        return 0;

     _w[_ql + i] = aux;
   }

   if (NULL != _remarks) {
      fgets (_remarks, 1000, file);
      fgets (_remarks, 1000, file);
   }
   
  fclose (file);


  return 1;
}
//-------------------------------------------------


// void testando (void) {


//    tp_gnn net;
//    double as[] = {-0.1, 5.1};

//    net.init (1, 7, as);

//    net.load ("c:\\temp\\net.txt");
//    net.save ("c:\\temp\\nnet.txt");

// }
