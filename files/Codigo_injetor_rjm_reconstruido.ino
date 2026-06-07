#include <TimerOne.h>
#include <LiquidCrystal.h>

/*
  Codigo_injetor_rjm - reconstruido a partir de frames de video.

  AVISO IMPORTANTE:
  - As partes claramente visiveis no video foram mantidas o mais fiel possivel.
  - Onde o video nao mostra todo o bloco, foi completado seguindo a logica do projeto.
  - Pinos confirmados no video:
      13 = Indutivo
      12 = Indutivo complementar
      11 = Hall
       3 = Fase
  - LCD confirmado no video:
      LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
  - A0 = teclado do LCD shield
  - A1 = potenciometro de RPM
*/

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int botao = 0;
int tecla = 0;
int menu = 1;

float rpm = 0;
float rps = 0;
float tempo = 0;
int contador = 0;

float potenciometro = 0;
float tensao_pot = 0;

int dentes = 1;
int folgas = 1;

int contador1 = 1;
int contador2 = 1;

int cont_frontier = 1;
int cont_amarok = 1;
int cont_brava = 1;
int cont_Civic_1_6 = 1;
int cont_sentra = 1;
int cont_ka = 1;
int cont_tucson = 1;
int cont_pld = 1;
int cont_fusion = 1;

/////////////////////////////////////////////////
// Funcoes auxiliares reconstruidas
/////////////////////////////////////////////////

void resetaSaidas()
{
  digitalWrite(13, LOW);
  digitalWrite(12, LOW);
  digitalWrite(11, LOW);
  digitalWrite(3, LOW);
}

void pulsoIndutivoHallFasePadrao(int limitePulso, int limiteReset)
{
  if (contador >= 0 && contador <= limitePulso)
  {
    digitalWrite(13, !digitalRead(13));
    digitalWrite(11, !digitalRead(11));

    if (digitalRead(13) == 1)
    {
      digitalWrite(12, LOW);
    }

    if (digitalRead(13) == 0)
    {
      digitalWrite(12, HIGH);
    }
  }

  if (contador > limitePulso)
  {
    digitalWrite(13, LOW);
    digitalWrite(12, LOW);
    digitalWrite(11, LOW);

    if (contador >= limiteReset)
    {
      contador = 0;
    }
  }
}

void rodaPersonalizada()
{
  int limitePulso = ((dentes * 2) - (2 * folgas));
  int limiteReset = (dentes * 2) - 1;

  if (contador >= 0 && contador <= limitePulso)
  {
    digitalWrite(13, !digitalRead(13));
    digitalWrite(11, !digitalRead(11));

    if (digitalRead(13) == 1)
    {
      digitalWrite(12, LOW);
    }

    if (digitalRead(13) == 0)
    {
      digitalWrite(12, HIGH);
    }
  }

  if (contador > limitePulso)
  {
    digitalWrite(13, LOW);
    digitalWrite(12, LOW);
    digitalWrite(11, LOW);

    if (contador >= limiteReset)
    {
      contador = 0;
    }
  }
}

// Gera sinal Hall com dentes ativos e falhas.
void hallComFalha(int limitePulso, int limiteReset, int pinoHall)
{
  if (contador >= 0 && contador <= limitePulso)
  {
    digitalWrite(pinoHall, !digitalRead(pinoHall));
  }

  if (contador > limitePulso)
  {
    digitalWrite(pinoHall, LOW);

    if (contador >= limiteReset)
    {
      contador = 0;
    }
  }
}

void fasePorTabela2(int fimContador, int fimVoltas,
                   int a1, int b1, int c1, int d1,
                   int a2, int b2, int c2, int d2)
{
  if (contador2 == 1)
  {
    if (contador == a1) digitalWrite(3, HIGH);
    if (contador == b1) digitalWrite(3, LOW);
    if (contador == c1) digitalWrite(3, HIGH);
    if (contador == d1) digitalWrite(3, LOW);
  }

  if (contador2 == 2)
  {
    if (contador == a2) digitalWrite(3, HIGH);
    if (contador == b2) digitalWrite(3, LOW);
    if (contador == c2) digitalWrite(3, HIGH);
    if (contador == d2) digitalWrite(3, LOW);
  }

  if (contador == fimContador)
  {
    contador = 0;
    contador2 = contador2 + 1;
    if (contador2 > fimVoltas) contador2 = 1;
  }
}

/////////////////////////////////////////////////
// Interrupcao Timer1
/////////////////////////////////////////////////

void interrupcao()
{
  contador = contador + 1;

  // Menus basicos com saida indutiva + hall
  if (menu == 100) // 60-2 H/I
  {
    pulsoIndutivoHallFasePadrao(116, 119);
  }

  if (menu == 101) // 60-2 H/I - variacao
  {
    pulsoIndutivoHallFasePadrao(116, 119);
  }

  if (menu == 102) // 36-1 H/I
  {
    pulsoIndutivoHallFasePadrao(70, 71);
  }

  if (menu == 103) // 36-1 H/I
  {
    pulsoIndutivoHallFasePadrao(70, 71);
  }

  if (menu == 104) // Person H/I
  {
    pulsoIndutivoHallFasePadrao(70, 71);
  }

  if (menu == 105) // 44-4 H/I
  {
    pulsoIndutivoHallFasePadrao(80, 87);
  }

  // Menus de simulacao com fase, vistos no video.
  if (menu == 110) // Frontier - bloco visivel no video
  {
    pulsoIndutivoHallFasePadrao(116, 119);
  }

  if (menu == 111) // Amarok - bloco visivel parcialmente
  {
    pulsoIndutivoHallFasePadrao(80, 87);
  }

  if (menu == 112) // Brava - reconstruido pela mesma logica
  {
    pulsoIndutivoHallFasePadrao(70, 71);
  }

  if (menu == 113) // Civic 1.6 - reconstruido por padrao de fase visivel
  {
    if (contador >= 1 && contador <= 17)
    {
      if (cont_Civic_1_6 == 2 || cont_Civic_1_6 == 3) digitalWrite(11, HIGH);
      if (cont_Civic_1_6 == 1) digitalWrite(11, LOW);
      cont_Civic_1_6 = cont_Civic_1_6 + 1;
      if (cont_Civic_1_6 > 3) cont_Civic_1_6 = 1;
      if (contador != 17) digitalWrite(3, HIGH);
      if (contador == 17) digitalWrite(3, LOW);
    }

    if (contador == 18)
    {
      digitalWrite(3, LOW);
      digitalWrite(11, HIGH);
      contador = 0;
      cont_Civic_1_6 = 1;
    }
  }

  if (menu == 114) // Sentra - parte visivel, completada
  {
    if (contador >= 0 && contador <= 73)
    {
      if (contador >= 1 && contador <= 19)
      {
        digitalWrite(11, !digitalRead(11));
      }

      if (contador > 19 && contador <= 25) digitalWrite(11, LOW);
      if (contador > 25 && contador <= 68) digitalWrite(11, !digitalRead(11));
      if (contador > 68) digitalWrite(11, LOW);

      if (contador2 == 1 && contador == 11) digitalWrite(3, HIGH);
      if (contador2 == 1 && contador == 16) digitalWrite(3, LOW);
      if (contador2 == 1 && contador == 61) digitalWrite(3, HIGH);
      if (contador2 == 1 && contador == 66) digitalWrite(3, LOW);
      if (contador2 == 2 && contador == 37) digitalWrite(3, HIGH);
      if (contador2 == 2 && contador == 42) digitalWrite(3, LOW);
      if (contador2 == 2 && contador == 45) digitalWrite(3, HIGH);
      if (contador2 == 2 && contador == 47) digitalWrite(3, LOW);
    }

    if (contador == 74)
    {
      contador = 0;
      contador2 = contador2 + 1;
      if (contador2 > 2) contador2 = 1;
    }
  }

  if (menu == 115) // Distribuidor / Palio em algumas telas
  {
    digitalWrite(11, !digitalRead(11));
  }

  if (menu == 117) // Roda personalizada dentes/folgas
  {
    rodaPersonalizada();
  }

  // Diesel / com fase - menus 550 em diante.
  // Estes blocos foram reconstruidos com o maximo visivel e completados por logica.

  if (menu == 550) // Frontier
  {
    if (contador >= 1 && contador <= 347)
    {
      if (cont_frontier == 0) digitalWrite(11, LOW);
      if (cont_frontier > 0 && cont_frontier <= 2) digitalWrite(11, HIGH);
      cont_frontier = cont_frontier + 1;
      if (cont_frontier > 2) cont_frontier = 0;

      if (contador2 == 1 && contador == 81) digitalWrite(3, LOW);
      if (contador2 == 1 && contador == 87) digitalWrite(3, HIGH);
      if (contador2 == 2 && contador == 81) digitalWrite(3, LOW);
      if (contador2 == 2 && contador == 87) digitalWrite(3, HIGH);
      if (contador2 == 2 && contador == 156) digitalWrite(3, LOW);
    }

    if (contador >= 348)
    {
      contador = 0;
      contador2 = contador2 + 1;
      if (contador2 > 2) contador2 = 1;
    }
  }

  if (menu == 551) // Amarok
  {
    if (contador >= 0 && contador < 57)
    {
      if (cont_amarok == 0x01 && contador == 20) digitalWrite(3, LOW);
      if (cont_amarok == 0x01 && contador == 25) digitalWrite(3, HIGH);
      if (cont_amarok == 0x02 && contador == 7) digitalWrite(3, LOW);
      if (cont_amarok == 0x02 && contador == 26) digitalWrite(3, HIGH);
      if (cont_amarok == 0x03 && contador == 16) digitalWrite(3, LOW);
      if (cont_amarok == 0x03 && contador == 25) digitalWrite(3, HIGH);
      if (cont_amarok == 0x04 && contador == 41) digitalWrite(3, LOW);

      digitalWrite(11, !digitalRead(11));
      if (contador == 0) digitalWrite(11, LOW);
    }

    if (contador >= 57)
    {
      if (contador >= 57 && contador < 59) digitalWrite(11, LOW);
      if (contador >= 59 && contador < 61) digitalWrite(11, HIGH);
    }

    if (contador == 60)
    {
      cont_amarok = cont_amarok + 1;
      if (cont_amarok > 4) cont_amarok = 1;
      contador = 0;
    }
  }

  if (menu == 552) // Brava - reconstruido
  {
    if (contador >= 0 && contador < 20)
    {
      digitalWrite(11, !digitalRead(11));
      if (contador == 17 && cont_brava == 1) digitalWrite(3, LOW);
      if (contador == 19 && cont_brava == 1) digitalWrite(3, HIGH);
      if (contador == 17 && cont_brava == 2) digitalWrite(3, LOW);
      if (contador == 17 && cont_brava == 3) digitalWrite(3, HIGH);
      if (contador == 19 && cont_brava == 3) digitalWrite(3, LOW);
      if (contador == 17 && cont_brava == 4) digitalWrite(3, HIGH);
    }

    if (contador >= 20) digitalWrite(11, LOW);

    if (contador == 21)
    {
      cont_brava = cont_brava + 1;
      if (cont_brava > 4) cont_brava = 1;
      contador = 0;
    }
  }

  if (menu == 553) // Civic 1.6 - reconstruido
  {
    if (contador >= 1 && contador <= 17)
    {
      if (cont_Civic_1_6 == 2 || cont_Civic_1_6 == 3) digitalWrite(11, HIGH);
      if (cont_Civic_1_6 == 1) digitalWrite(11, LOW);
      cont_Civic_1_6 = cont_Civic_1_6 + 1;
      if (cont_Civic_1_6 > 3) cont_Civic_1_6 = 1;
      if (contador != 17) digitalWrite(3, HIGH);
      if (contador == 17) digitalWrite(3, LOW);
    }

    if (contador == 18)
    {
      digitalWrite(3, LOW);
      digitalWrite(11, HIGH);
      contador = 0;
      cont_Civic_1_6 = 1;
    }
  }

  if (menu == 554) // Padrao capturado parcialmente
  {
    if (contador >= 0 && contador < 43)
    {
      if (contador == 1 || contador == 4 || contador == 11 || contador == 22 || contador == 33) digitalWrite(11, HIGH);
      else digitalWrite(11, LOW);

      if (contador == 2 || contador == 3 || contador == 4 || contador == 5 || contador == 7 || contador == 12 || contador == 13) digitalWrite(3, HIGH);
      else digitalWrite(3, LOW);
    }

    if (contador == 44)
    {
      contador = 0;
    }
  }

  if (menu == 555) // Civic 1.6 alternativa
  {
    if (contador >= 1 && contador <= 17)
    {
      if (cont_Civic_1_6 == 2 || cont_Civic_1_6 == 3) digitalWrite(11, HIGH);
      if (cont_Civic_1_6 == 1) digitalWrite(11, LOW);
      cont_Civic_1_6 = cont_Civic_1_6 + 1;
      if (cont_Civic_1_6 > 3) cont_Civic_1_6 = 1;
      if (contador != 17) digitalWrite(3, HIGH);
      if (contador == 17) digitalWrite(3, LOW);
    }

    if (contador == 18)
    {
      digitalWrite(3, LOW);
      digitalWrite(11, HIGH);
      contador = 0;
      cont_Civic_1_6 = 1;
    }
  }

  if (menu == 556) // Padrao com contador2 visto no video
  {
    if (contador >= 0 && contador < 114)
    {
      digitalWrite(11, !digitalRead(11));

      if (contador == 26 && contador2 == 1) digitalWrite(3, HIGH);
      if (contador == 37 && contador2 == 1) digitalWrite(3, LOW);
      if (contador == 51 && contador2 == 1) digitalWrite(3, HIGH);
      if (contador == 97 && contador2 == 1) digitalWrite(3, LOW);
      if (contador == 36 && contador2 == 2) digitalWrite(3, LOW);
    }

    if (contador >= 114)
    {
      digitalWrite(11, LOW);
      if (contador >= 119)
      {
        contador = 0;
        contador2 = contador2 + 1;
        if (contador2 > 3) contador2 = 1;
      }
    }
  }

  if (menu == 560) // Padrao 114/119 visto no video
  {
    if (contador >= 0 && contador < 114)
    {
      digitalWrite(11, !digitalRead(11));
      if (contador == 79) digitalWrite(3, !digitalRead(3));
    }

    if (contador >= 114)
    {
      if (contador == 114 || contador == 115) digitalWrite(11, LOW);
      if (contador == 116 || contador == 117 || contador == 118) digitalWrite(11, HIGH);
    }

    if (contador >= 119)
    {
      contador = 0;
    }
  }

  if (menu == 562) // Sentra, visivel parcialmente
  {
    if (contador >= 0 && contador <= 46)
    {
      if (cont_sentra == 1) digitalWrite(11, LOW);
      if (cont_sentra > 1 && cont_sentra <= 3) digitalWrite(11, HIGH);
      cont_sentra = cont_sentra + 1;
      if (cont_sentra > 3) cont_sentra = 1;

      if (contador2 == 1 && contador == 5) digitalWrite(3, HIGH);
      if (contador2 == 1 && contador == 10) digitalWrite(3, LOW);
      if (contador2 == 1 && contador == 32) digitalWrite(3, HIGH);
      if (contador2 == 1 && contador == 44) digitalWrite(3, LOW);
      if (contador2 == 2 && contador == 45) digitalWrite(3, HIGH);
      if (contador2 == 2 && contador == 47) digitalWrite(3, LOW);
    }

    if (contador == 74)
    {
      contador = 0;
      contador2 = contador2 + 1;
      if (contador2 > 2) contador2 = 1;
    }
  }

  if (menu == 563) // Ford KA 1.0
  {
    hallComFalha(116, 119, 11);
  }

  if (menu == 564) // Tucson 2.0
  {
    hallComFalha(116, 119, 11);
  }

  if (menu == 565) // Lancer 2.0
  {
    if (contador >= 0 && contador < 34)
    {
      digitalWrite(11, !digitalRead(11));
      if (contador == 24) digitalWrite(3, !digitalRead(3));
    }

    if (contador >= 34 && contador <= 35) digitalWrite(11, LOW);
    if (contador >= 36 && contador <= 66) digitalWrite(11, !digitalRead(11));
    if (contador >= 67 && contador <= 69) digitalWrite(11, LOW);

    if (contador == 70) contador = 0;
  }

  if (menu == 566) // Evoque 2.0
  {
    hallComFalha(116, 119, 11);
  }

  if (menu == 567) // Polo 1.6
  {
    if (contador >= 1 && contador <= 119)
    {
      digitalWrite(11, !digitalRead(11));
      fasePorTabela2(119, 2, 28, 40, 56, 100, 40, 89, 100, 100);
    }
  }

  if (menu == 568) // Honda Fit
  {
    if (contador >= 0 && contador <= 116)
    {
      if (contador == 1 || contador == 3 || contador == 6 || contador == 11 || contador == 16 || contador == 21 || contador == 26)
      {
        digitalWrite(11, HIGH);
      }
      else
      {
        digitalWrite(11, LOW);
      }

      if (contador2 == 1)
      {
        if (contador >= 1 && contador <= 3) digitalWrite(3, LOW);
        if (contador >= 4 && contador <= 22) digitalWrite(3, HIGH);
        if (contador >= 23 && contador <= 26) digitalWrite(3, LOW);
        if (contador >= 27 && contador <= 52) digitalWrite(3, HIGH);
        if (contador >= 53 && contador <= 56) digitalWrite(3, LOW);
        if (contador == 57) digitalWrite(3, HIGH);
      }

      if (contador2 == 2)
      {
        if (contador == 22) digitalWrite(3, LOW);
        if (contador >= 23 && contador <= 26) digitalWrite(3, LOW);
        if (contador >= 27 && contador <= 52) digitalWrite(3, HIGH);
        if (contador >= 53 && contador <= 56) digitalWrite(3, LOW);
        if (contador == 57) digitalWrite(3, HIGH);
      }
    }

    if (contador == 60)
    {
      contador = 0;
      contador2 = contador2 + 1;
      if (contador2 > 2) contador2 = 1;
    }
  }

  if (menu == 569) // Dodge Ram 5.9
  {
    pulsoIndutivoHallFasePadrao(116, 119);
  }

  if (menu == 570) // Saveiro 1.6
  {
    if (contador >= 1 && contador < 115)
    {
      digitalWrite(11, !digitalRead(11));

      if (contador2 == 1 && contador == 5) digitalWrite(3, LOW);
      if (contador2 == 1 && contador == 10) digitalWrite(3, HIGH);
      if (contador2 == 1 && contador == 25) digitalWrite(3, LOW);
      if (contador2 == 1 && contador == 30) digitalWrite(3, HIGH);
      if (contador2 == 1 && contador == 45) digitalWrite(3, LOW);
      if (contador2 == 1 && contador == 50) digitalWrite(3, HIGH);
      if (contador2 == 1 && contador == 65) digitalWrite(3, LOW);
      if (contador2 == 1 && contador == 70) digitalWrite(3, HIGH);
      if (contador2 == 1 && contador == 85) digitalWrite(3, LOW);
      if (contador2 == 1 && contador == 90) digitalWrite(3, HIGH);
      if (contador2 == 1 && contador == 105) digitalWrite(3, LOW);
      if (contador2 == 1 && contador == 110) digitalWrite(3, HIGH);

      if (contador2 == 2 && contador == 5) digitalWrite(3, LOW);
      if (contador2 == 2 && contador == 10) digitalWrite(3, HIGH);
      if (contador2 == 2 && contador == 25) digitalWrite(3, LOW);
      if (contador2 == 2 && contador == 30) digitalWrite(3, HIGH);
      if (contador2 == 2 && contador == 35) digitalWrite(3, LOW);
      if (contador2 == 2 && contador == 40) digitalWrite(3, HIGH);
      if (contador2 == 2 && contador == 45) digitalWrite(3, LOW);
      if (contador2 == 2 && contador == 50) digitalWrite(3, HIGH);
      if (contador2 == 2 && contador == 65) digitalWrite(3, LOW);
      if (contador2 == 2 && contador == 70) digitalWrite(3, HIGH);
      if (contador2 == 2 && contador == 85) digitalWrite(3, LOW);
      if (contador2 == 2 && contador == 90) digitalWrite(3, HIGH);
      if (contador2 == 2 && contador == 105) digitalWrite(3, LOW);
      if (contador2 == 2 && contador == 110) digitalWrite(3, HIGH);
    }

    if (contador > 116)
    {
      if (contador >= 119)
      {
        contador = 0;
        contador2 = contador2 + 1;
        if (contador2 > 2) contador2 = 1;
      }
    }
  }

  if (menu == 571) // Fusion 2.0
  {
    if (contador >= 1 && contador <= 347)
    {
      if (cont_fusion == 1) digitalWrite(11, LOW);
      if (cont_fusion > 1 && cont_fusion <= 8) digitalWrite(11, HIGH);
      cont_fusion = cont_fusion + 1;
      if (cont_fusion > 6) cont_fusion = 1;

      if (contador2 == 1 && contador == 105) digitalWrite(3, LOW);
      if (contador2 == 1 && contador == 237) digitalWrite(3, HIGH);
      if (contador2 == 1 && contador == 285) digitalWrite(3, LOW);
      if (contador2 == 2 && contador == 57) digitalWrite(3, HIGH);
      if (contador2 == 2 && contador == 102) digitalWrite(3, LOW);
      if (contador2 == 2 && contador == 144) digitalWrite(3, HIGH);
    }

    if (contador >= 348)
    {
      contador = 0;
      contador2 = contador2 + 1;
      if (contador2 > 2) contador2 = 1;
    }
  }
}

/////////////////////////////////////////////////
// Funcoes do codigo
/////////////////////////////////////////////////

void teclado()
{
  botao = analogRead(0); // Leitura do valor da porta analogica A0
  delay(50);

  if (botao < 100) { tecla = 1; }
  else if (botao < 200) { tecla = 2; }
  else if (botao < 400) { tecla = 3; }
  else if (botao < 600) { tecla = 4; }
  else if (botao < 800) { tecla = 5; }
  else if (botao < 10) { tecla = 0; } // mantido como aparece no video, embora seja inalcançavel
}

void mainfunction()
{
  if (tecla == 4)
  {
    delay(100);
    lcd.clear();
    menu = 1;
    tecla = 0;
  }

  float potenciometro = analogRead(A1);
  delay(30);
  tensao_pot = ((5.0 / 1023.0) * (potenciometro) * 1000.0);
  rpm = 500.0 + tensao_pot;

  rps = rpm / 60.0;
  tempo = (1 / (72.0 * rps)) * 1000000.0;
  Timer1.setPeriod(tempo);
}

void setup()
{
  lcd.begin(16, 2);

  pinMode(13, OUTPUT); // Indutivo
  pinMode(12, OUTPUT); // Indutivo
  pinMode(11, OUTPUT); // Hall
  pinMode(3, OUTPUT);  // Fase

  lcd.setCursor(0, 0);
  lcd.print("---Injetor de---");
  lcd.setCursor(0, 1);
  lcd.print("-----Sinais-----");
  delay(2000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("-By RJM eletron-");
  lcd.setCursor(0, 1);
  lcd.print("---17/05/2019---");
  delay(2000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("H --> Hall");
  lcd.setCursor(0, 1);
  lcd.print("I --> Indutivo");
  delay(2000);
  lcd.clear();

  Timer1.initialize(200000);
  Timer1.attachInterrupt(interrupcao);
}

void mostraRpm(byte coluna)
{
  if (rpm < 1000)
  {
    lcd.setCursor(coluna, 0);
    lcd.print("0");
    lcd.setCursor(coluna + 1, 0);
    lcd.print(rpm, 0);
  }
  else
  {
    lcd.setCursor(coluna, 0);
    lcd.print(rpm, 0);
  }
}

void telaSimulando(const char *nome, byte colunaRpm)
{
  lcd.setCursor(0, 0);
  lcd.print(nome);
  lcd.setCursor(0, 1);
  lcd.print("Simulando....");
  mostraRpm(colunaRpm);

  teclado();
  mainfunction();
}

void loop()
{
  /////////////// Menu principal ///////////////

  while (menu == 1)
  {
    lcd.setCursor(0, 0);
    lcd.print("Basicos     ->");
    lcd.setCursor(0, 1);
    lcd.print("Sin c/ fase");

    teclado();

    if (tecla == 2) { delay(100); lcd.clear(); menu = 105; tecla = 0; }
    if (tecla == 3) { delay(100); lcd.clear(); menu = 101; tecla = 0; }
    if (tecla == 5) { delay(100); lcd.clear(); menu = 110; tecla = 0; }
  }

  /////////////// Menu Basicos ///////////////

  while (menu == 100)
  {
    lcd.setCursor(0, 0);
    lcd.print("60-2(H/I)    ->");
    lcd.setCursor(0, 1);
    lcd.print("44-4(H/I)");

    teclado();

    if (tecla == 2) { delay(100); lcd.clear(); menu = 105; tecla = 0; }
    if (tecla == 3) { delay(100); lcd.clear(); menu = 101; tecla = 0; }
    if (tecla == 5) { delay(100); lcd.clear(); menu = 110; tecla = 0; }
  }

  while (menu == 101)
  {
    lcd.setCursor(0, 0);
    lcd.print("60-2(H/I)");
    lcd.setCursor(0, 1);
    lcd.print("44-4(H/I)    ->");

    teclado();

    if (tecla == 2) { delay(100); lcd.clear(); menu = 100; tecla = 0; }
    if (tecla == 3) { delay(100); lcd.clear(); menu = 102; tecla = 0; }
    if (tecla == 5) { delay(100); lcd.clear(); menu = 111; tecla = 0; }
  }

  while (menu == 102)
  {
    lcd.setCursor(0, 0);
    lcd.print("44-4(H/I)");
    lcd.setCursor(0, 1);
    lcd.print("36-1(H/I)   ->");

    teclado();

    if (tecla == 2) { delay(100); lcd.clear(); menu = 101; tecla = 0; }
    if (tecla == 3) { delay(100); lcd.clear(); menu = 103; tecla = 0; }
    if (tecla == 5) { delay(100); lcd.clear(); menu = 112; tecla = 0; }
  }

  while (menu == 103)
  {
    lcd.setCursor(0, 0);
    lcd.print("36-1(H/I)");
    lcd.setCursor(0, 1);
    lcd.print("36-2(H/I)    ->");

    teclado();

    if (tecla == 2) { delay(100); lcd.clear(); menu = 102; tecla = 0; }
    if (tecla == 3) { delay(100); lcd.clear(); menu = 104; tecla = 0; }
    if (tecla == 5) { delay(100); lcd.clear(); menu = 113; tecla = 0; }
  }

  while (menu == 104)
  {
    lcd.setCursor(0, 0);
    lcd.print("Person(H/I)  ->");
    lcd.setCursor(0, 1);
    lcd.print("Distribuidor");

    teclado();

    if (tecla == 2) { delay(100); lcd.clear(); menu = 103; tecla = 0; }
    if (tecla == 3) { delay(100); lcd.clear(); menu = 105; tecla = 0; }
    if (tecla == 5) { delay(100); lcd.clear(); menu = 114; tecla = 0; }
  }

  while (menu == 105)
  {
    telaSimulando("44-4(H/I)->", 11);
  }

  /////////////// Simulacoes basicas selecionadas ///////////////

  while (menu == 110) { telaSimulando("Frontier->", 11); }
  while (menu == 111) { telaSimulando("Amarok ->", 11); }
  while (menu == 112) { telaSimulando("Brava 1.0->", 11); }
  while (menu == 113) { telaSimulando("Civic 1.6->", 11); }
  while (menu == 114) { telaSimulando("Distrib->", 10); }
  while (menu == 115) { telaSimulando("Distrib->", 10); }

  while (menu == 116)
  {
    lcd.setCursor(0, 0);
    lcd.print("-Dentes||Folgas-");
    lcd.setCursor(2, 1);
    lcd.print("<");
    lcd.setCursor(3, 1);
    lcd.print(dentes);
    lcd.setCursor(5, 1);
    lcd.print(">");

    lcd.setCursor(10, 1);
    lcd.print("<");
    lcd.setCursor(13, 1);
    lcd.print(">");
    lcd.setCursor(11, 1);
    lcd.print(folgas);

    teclado();

    if (tecla == 1)
    {
      delay(70);
      lcd.clear();
      dentes = dentes + 1;
      if (dentes > 60) dentes = 1;
      tecla = 0;
    }

    if (tecla == 4)
    {
      delay(70);
      lcd.clear();
      dentes = dentes - 1;
      if (dentes < 1) dentes = 60;
      tecla = 0;
    }

    if (tecla == 2)
    {
      delay(70);
      lcd.clear();
      folgas = folgas + 1;
      if (folgas > dentes) folgas = 1;
      tecla = 0;
    }

    if (tecla == 3)
    {
      delay(70);
      lcd.clear();
      folgas = folgas - 1;
      if (folgas < 1) folgas = dentes;
      tecla = 0;
    }

    if (tecla == 5)
    {
      delay(70);
      lcd.clear();
      menu = 117;
      tecla = 0;
    }
  }

  while (menu == 117)
  {
    lcd.setCursor(0, 0);
    lcd.print(dentes);
    lcd.setCursor(2, 0);
    lcd.print("-");
    lcd.setCursor(3, 0);
    lcd.print(folgas);
    lcd.print("->");
    lcd.setCursor(0, 1);
    lcd.print("Simulando....");
    mostraRpm(12);

    teclado();
    mainfunction();
  }

  /////////////// Sinais Diesel ///////////////

  while (menu == 301)
  {
    telaSimulando("PLD ->", 7);
  }

  /////////////// Sinais com Fase ///////////////

  while (menu == 550) { telaSimulando("Frontier->", 11); }
  while (menu == 551) { telaSimulando("Amarok ->", 11); }
  while (menu == 552) { telaSimulando("Brava ->", 11); }
  while (menu == 553) { telaSimulando("Civic 1.6->", 11); }
  while (menu == 554) { telaSimulando("Golf 1.0->", 11); }
  while (menu == 555) { telaSimulando("Civic 1.6->", 11); }
  while (menu == 556) { telaSimulando("Civic 1.8->", 11); }
  while (menu == 560) { telaSimulando("Saveiro1.6->", 12); }
  while (menu == 562) { telaSimulando("Sentra ->", 11); }
  while (menu == 563) { telaSimulando("For KA 1.0->", 12); }
  while (menu == 564) { telaSimulando("Tucson 2.0->", 12); }
  while (menu == 565) { telaSimulando("Lancer 2.0->", 12); }
  while (menu == 566) { telaSimulando("Evoque 2.0->", 12); }
  while (menu == 567) { telaSimulando("Polo 1.6 ->", 12); }
  while (menu == 568) { telaSimulando("Honda Fit->", 12); }
  while (menu == 569) { telaSimulando("DodRam5.9->", 12); }
  while (menu == 570) { telaSimulando("Saveiro1.6->", 12); }
  while (menu == 571) { telaSimulando("Fusion 2.0->", 12); }

  // Menus vazios vistos no video.
  if (menu == 572)
  {
  }

  if (menu == 573)
  {
  }
}
