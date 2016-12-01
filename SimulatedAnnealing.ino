#include <math.h>

//Coordenadas ciudades
const int xCoord[] = {1150,630,40,750,750,1030,1650,1490,790,710,840,1170,970,510,750,1280,230,460,1040,590,830,490,1840,1260,1280,490,1460,1260,360};
const int yCoord[] = {1760,1660,2090,1100,2030,2070,650,1630,2260,1310,550,2300,1340,700,900,1200,590,860,950,1390,1770,500,1240,1500,790,2130,1420,1910,1980};
byte ruta[sizeof(xCoord)/2];
byte vecino[sizeof(ruta)];

void creditos();
byte check();
void solucionInicial(byte n_ciudades);
long calcularEnergia(byte *solucion);
int calcularDistancia(int nodoAx, int nodoBx, int nodoAy, int nodoBy);
void generarVecino();
bool aceptarSolucion(double deltaEnergia, double temperatura);
float coolingSchedule(float t_inicial);

void setup(){
	int t_inicial = 300, epsilon = 1, numIter = 4;
  int menorEnergia, deltaEnergia;
  
  randomSeed(analogRead(0));
	Serial.begin(9600);
  creditos();
  byte tamano = check();
  Serial.print("La ruta inicial es: ");
  solucionInicial(tamano);
  int eneRuta, eneVecino;
  eneRuta = calcularEnergia(ruta);

  for(byte i = 0; i < numIter; i++){
    Serial.println(" ");
    Serial.print("Iteracion: ");
    Serial.print(i + 1);
    Serial.print("/");
    Serial.print(numIter);
    generarVecino();
    eneVecino = calcularEnergia(vecino);
    deltaEnergia = eneVecino - eneRuta;
    aceptarSolucion(deltaEnergia, t_inicial);
  }
  
}

void loop(){

}

//FUNCIONES
void creditos(){
  Serial.println("Simulated Annealing para el Problema del Vendedor Viajero - Arduino");
  Serial.println("Autor: Dagoberto Cifuentes Lobos");
  Serial.println("e-mail: dagocifuentes@udec.cl");
  Serial.println(" ");
}

byte check(){
  byte x = sizeof(xCoord)/2;
  byte y = sizeof(yCoord)/2;
  if(x == y){  
    Serial.print("Ambos vectores tienen el mismo tamano: ");
    Serial.println(x);
  }
  else{
    Serial.print("ERROR: Los vectores no tienen el mismo tamano");
  }
  return x;  
}

void solucionInicial(byte n_ciudades){
  byte auxi[n_ciudades];
  for(byte i = 0; i < n_ciudades; i++){
    auxi[i] = i + 1;
  }
  randomSeed(analogRead(0));
  //Serial.print("Ruta: ");
  for(byte j = 0; j < n_ciudades; j++){
    byte ran = random(n_ciudades - j);
    ruta[j] = auxi[ran];
    auxi[ran] = auxi[n_ciudades - j - 1];
    Serial.print(ruta[j]);
    Serial.print(" ");
  }
}

long calcularEnergia(byte *solucion){
  byte i, x = sizeof(ruta);
  unsigned int j;
  long acum = 0;
  Serial.println("");
  for(i = 0; i < x - 1; i++){
    //Serial.print("(");
    //Serial.print(solucion[i]);
    //Serial.print(", ");
    //Serial.print(solucion[i + 1]);
    //Serial.print(") = ");
    j = calcularDistancia(xCoord[solucion[i] - 1], xCoord[solucion[i + 1] - 1], yCoord[solucion[i] - 1], yCoord[solucion[i + 1] - 1]);
    //Serial.print(j);
    //Serial.println("");
    acum = acum + j;
  }
  //Serial.print("(");
  //Serial.print(solucion[x - 1]);
  //Serial.print(", ");
  //Serial.print(solucion[0]);
  //Serial.print(") = ");
  j = calcularDistancia(xCoord[solucion[x - 1] - 1], xCoord[solucion[0] - 1], yCoord[solucion[x - 1] - 1], yCoord[solucion[0] - 1]);
  //Serial.println(j);
  acum = acum + j;
  Serial.print("Distancia acumulada: ");
  Serial.println(acum);
  return acum;
}

int calcularDistancia(int nodoAx, int nodoBx, int nodoAy, int nodoBy){
  int difX = nodoBx - nodoAx;
  int difY = nodoBy - nodoAy;
  float dist = sqrt(pow(difX,2) + pow(difY,2));
  unsigned int distancia = abs(dist);
  return distancia;
}

void generarVecino(){
  byte i, k;
  i = random(1, sizeof(ruta));
  k = random(1, sizeof(ruta));
  while(i == k){
    k = random(1, sizeof(ruta));
  }
  if(i > k){
    byte j = i;
    i = k;
    k = j;
  }
  Serial.println("");
  Serial.print("Random: ");
  Serial.print(i);
  Serial.print(" ");
  Serial.println(k);
  for(byte x = 0; x < i; x++){
    vecino[x] = ruta[x];
  }
  byte a = 0;
  for(byte x = i; x <= k; x++){
    vecino[x] = ruta[k - a];
    a++;
  }
  for(byte x = k + 1; x < sizeof(ruta); x++){
    vecino[x] = ruta[x];
  }
  Serial.print("Vecino: ");
  for(byte x = 0; x < sizeof(vecino); x++){
    Serial.print(vecino[x]);
    Serial.print(" ");
  }
}

bool aceptarSolucion(double deltaEnergia, double temperatura){
  if(deltaEnergia < 0){
    Serial.println("Se acepta la ruta porque mejora la solucion");
    for(byte i = 0; i < sizeof(ruta); i++){
      ruta[i] = vecino[i];
    }
  }
  else{
    byte r = random(1, 101);
    double prob = 100 * pow(M_E, -deltaEnergia/temperatura);
    Serial.print("Prob: ");
    Serial.println(prob);
    Serial.print("Random: ");
    Serial.println(r);
    if(prob > r){
      Serial.println("Se acepta la ruta a pesar de que no mejora la solucion");
      Serial.println(" ");
      for(byte i = 0; i < sizeof(ruta); i++){
        ruta[i] = vecino[i];
      }
    }
    else{
      Serial.println("No se acepta la solucion");
      Serial.println(" ");
    }
  }
}

float coolingSchedule(float t_inicial){
  //Enfriamiento geometrico
  float t_final, alpha = 0.95;
  t_final = t_inicial * alpha;
  return t_final;
}

