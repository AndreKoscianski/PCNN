pkg load statistics

clc
clear all
close all


%---------------------------------------------------
% Gere dados para um teste simples da rede neural.
% Equação de calor, uma barra é aquecida na ponta.
%
% Dois arquivos foram gerados: um com ruído,
%   outro sem. (calor.csv; calor2.csv).
%---------------------------------------------------


% Mão de ferro sobre repetibilidade.
 rand ('seed',123);


 L=1;  % comprimento
 
  % divisao espacial
 nx=31; 
 dx=L/nx;

  % passos de integracao (tempo total)
 nt=511;
 dt=0.0002;
 
% nx=16; 
% dx=L/nx;
% nt=16;
% dt=1/nt;
 
 
 
 % Coeficiente de condução de calor = constante.
 % Aqui seria um lugar para injetar um pouco de ruído.
 k=1;% * normrnd (1.0, 0.1); 
 kk = k * ones(1,nx);

 for j=1:nx
    kk(j) = kk(j);
 endfor


 % inicio da integracao
 alpha = k*dt / dx^2;
 beta  = dt / (dx*dx);
 
 T0   = 0*ones(1,nx);
 T1   = 0*ones(1,nx);

 % condição de fronteira, temperatura inicial zero. 
 T0(1) = 0;
 
 % Dados de saída
 Dados = [];
 Model = []
 
 Vzero = zeros(size(T0));
 
 eixox = linspace(0,1,nx);
 
 for j=1:nt
    for i=2:nx-1
        T1(i)=T0(i)+kk(i)*beta*(T0(i+1)-2*T0(i)+T0(i-1));
    end

    % passo de tempo
    T0 = T1;

    % Temperatura crescente na ponta.
    % Outro lugar bom para colocar ruído.
    T0(1) = j/nt * normrnd (1.0, 0.05); ;

    Model = [T0 ; Model];  
    Dados  = [T0  ; Dados];

    % atualize gráfico 1 vez a cada 5 laços
    %if (0 == mod(j,10))
      %plot  (eixox,T0,'linewidth',2);
      %title (strcat('tempo = ' , num2str(j)))
      %drawnow
    %end
end


Amostra = zeros(16,16);

it=0;
while(it*32 < 512)
   ix = 0;
   while (ix*2 < 32)
      Amostra(1+it,1+ix) = Dados(1+it*32, 1+ix*2);
      ix = ix + 1;
   endwhile
   it = it + 1;
endwhile

surf (Amostra);
%dlmwrite('calor2.csv',Amostra);
